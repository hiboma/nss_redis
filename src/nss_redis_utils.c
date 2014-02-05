#include "nss_redis.h"
#include "nss_redis_utils.h"

static const char GETPWNAM_COMMAND[]  = "MGET %d:name %d:passwd %d:uid %d:gid %d:gecos %d:dir %d:shell";
static const char GETPWUID_COMMAND[]  = "GET %s";
static const char GETPGRNAM_COMMAND[] = "MGET %d:name %d:passwd %d:gid";

_nss_redis_config_t _nss_redis_config = {
  .host     = "127.0.0.1",
  .port     = 6379,
  .password = NULL,
};

static struct redisReply redis_reply_empty = {
  .str = "",
  .len = 0,
};

nss_status_t
_nss_redis_load_config(const char *path) {

  FILE *f;

  char line[1024];
  char *ccil;
  char *cur;

  if(!path)  {
    path = "/etc/libnss-redis.cfg";
  }

  f = fopen(path, "r");
  if(!f) {
    return NSS_STATUS_NOTFOUND;
  }

  while(fgets(line, 1024, f) != NULL) {
    if(*line == '#')
      continue;

    cur = ccil = line;
    ccil += strcspn(ccil, "\n\r \t");
    if(ccil == cur)
      continue;

    *ccil = '\0';
    ++ccil;

    if(ccil[strlen(ccil)-1] == '\n') {
      ccil[strlen(ccil)-1] = '\0';
    }

    if(strcmp(cur, "host") == 0) {
      _nss_redis_config.host = strdup(ccil);
    }
    else if(strcmp(cur, "port") == 0) {
      _nss_redis_config.port = atoi(ccil);
    }
    else if(strcmp(cur, "password") == 0) {
      _nss_redis_config.password = strdup(ccil);
    }
  }

  return NSS_STATUS_SUCCESS;
}

static bool
is_valid_redis_reply(redisReply *r) {

  if(r == NULL) {
    return false;
  }

  if(r->type == REDIS_REPLY_ERROR || r->type != REDIS_REPLY_ARRAY) {
    return false;
  }

  return true;
}

redisReply *
_nss_redis_pw_redis_command(redisContext *c, uid_t uid) {
  return redisCommand(c, GETPWNAM_COMMAND, repeat7(uid));
}

redisReply *
_nss_redis_gr_redis_command(redisContext *c, gid_t gid) {
  return redisCommand(c, GETPGRNAM_COMMAND, repeat4(gid));
}

/* Singleton */
static redisContext *c = NULL;

static bool
_nss_redis_redis_auth(redisContext *c) {

  bool status = false;

  if(!_nss_redis_config.password) {
    return status;
  }

  redisReply *r = redisCommand(c, "AUTH %s", _nss_redis_config.password);
  if(r->type != REDIS_REPLY_STATUS) {
    fprintf(stderr, "failed auth\n");
    goto done;
  }

  if(strcmp(r->str, "OK") != 0) {
    fprintf(stderr, "status %s\n", r->str);
    goto done;
  }

  status = true;

 done:
  freeReplyObject(r);
  return status;
}

redisContext*
_nss_redis_redis_connect() {

  if (c)  {
    return c;
  }

  if(*_nss_redis_config.host == '/') {
    c = redisConnectUnix(_nss_redis_config.host);
  } else{
    c = redisConnect(_nss_redis_config.host, _nss_redis_config.port);
  }

  if(c->err) {
    redisFree(c);
    c = NULL;
    goto connected;
  }

  if(_nss_redis_redis_auth(c) == false) {
    c = NULL;
    goto connected;
  }

 connected:
  return c;
}

void
_nss_redis_redis_disconnect() {
  if (c)  {
    redisFree(c);
    c = NULL;
  }
}

void
_nss_redis_redis_clean_passwod() {
  if(_nss_redis_config.password) {
    memset(_nss_redis_config.password, 0, strlen(_nss_redis_config.password));
  }
}

nss_status_t
_nss_redis_fill_passwd(redisContext *c, uid_t uid, struct passwd *result,
                   char *buffer, size_t buflen) {

  nss_status_t status = NSS_STATUS_UNAVAIL;
  redisReply *r;
  redisReply *name, *dir, *shell, *passwd, *gecos; /* do not freeReplyObject */

  r = _nss_redis_pw_redis_command(c, uid);

  if(!is_valid_redis_reply(r)) {
    goto done;
  }

  if(r->elements != NSS_REDIS_PW_NUM_ENTRIES) {
    goto done;
  }

  status = _nss_redis_pw_uid(r, &result->pw_uid);
  if(status != NSS_STATUS_SUCCESS) {
    goto done;
  }

  status = _nss_redis_pw_gid(r, &result->pw_gid);
  if(status != NSS_STATUS_SUCCESS) {
    goto done;
  }

  name = _nss_redis_pw_name(r);
  if(name->len == 0) {
    goto done;
  }

  shell  = _nss_redis_pw_shell(r);
  dir    = _nss_redis_pw_dir(r);
  gecos  = _nss_redis_pw_gecos(r);
  passwd = _nss_redis_pw_passwd(r);

  if( !shell || !dir  || !gecos || !passwd ) {
    goto done;
  }

  if((name->len + shell->len + dir->len + gecos->len + passwd->len) > buflen) {
    status = NSS_STATUS_TRYAGAIN;
    goto done;
  }

  memset(buffer, 0, buflen);

  result->pw_name = strncpy(buffer, name->str, name->len);
  buffer += name->len + 1;

  result->pw_shell = strncpy(buffer, shell->str, shell->len);
  buffer += shell->len + 1;

  result->pw_gecos = strncpy(buffer, gecos->str, gecos->len);
  buffer += gecos->len + 1;

  result->pw_dir = strncpy(buffer, dir->str, dir->len);
  buffer += dir->len + 1;

  result->pw_passwd = strncpy(buffer, passwd->str, passwd->len);
  buffer += passwd->len + 1;

  status = NSS_STATUS_SUCCESS;

 done:
  if(r) {
    freeReplyObject(r);
  }
  return status;
}

static bool
is_buffer_size_enough(redisReply *r, size_t buflen) {
  size_t size = 0;
  for(int i = 0; i < r->elements; i++) {
    size += r->element[i]->len + 1; /* stringlen + \0 */
  }

  return buflen > size;
}

static void
fill_buffer_with_gr_mem(redisReply *r, struct group *result, char *buffer) {

  char **gr_mem = result->gr_mem = (char **)buffer;
  buffer += (r->elements + 1) * sizeof(char *);

  for( int i = 0; i< r->elements; i++) {
    redisReply *e = r->element[i];
    *gr_mem = strncpy(buffer, e->str, e->len);
    buffer += e->len + 1;
    gr_mem++;
  }
  *gr_mem = NULL;
}

nss_status_t
_nss_redis_fill_group_gr_mem(redisContext *c, gid_t gid, struct group *result,
                   char *buffer, size_t buflen) {

  nss_status_t status = NSS_STATUS_UNAVAIL;
  redisReply *r;

  r = redisCommand(c, "SMEMBERS gid:%d", gid);
  if(!is_valid_redis_reply(r)) {
    goto done;
  }

  if (r->elements == 0) {
    /* result->gr_mem == NULL is valid */
    result->gr_mem = NULL;
    status = NSS_STATUS_SUCCESS;
    goto done;
  }

  if (!is_buffer_size_enough(r, buflen)) {
    status = NSS_STATUS_TRYAGAIN;
    goto done;
  }

  fill_buffer_with_gr_mem(r, result, buffer);
  status = NSS_STATUS_SUCCESS;

 done:
  if(r) {
    freeReplyObject(r);
  }
  return status;
}

nss_status_t
_nss_redis_fill_group(redisContext *c, gid_t gid, struct group *result,
                   char *buffer, size_t buflen) {

  nss_status_t status = NSS_STATUS_UNAVAIL;
  redisReply *r;
  redisReply *gr_name, *gr_passwd; /* do not freeReplyObject */

  r = _nss_redis_gr_redis_command(c, gid);

  if(!is_valid_redis_reply(r)) {
    goto done;
  }

  if(r->elements != NSS_REDIS_GR_NUM_ENTRIES) {
    goto done;
  }

  gr_name = _nss_redis_gr_name(r);
  if(gr_name->len == 0) {
    status = NSS_STATUS_NOTFOUND;
    goto done;
  }

  gr_passwd = _nss_redis_gr_passwd(r);

  memset (buffer, 0, buflen);

  result->gr_name = strncpy(buffer, gr_name->str, gr_name->len);
  buffer += gr_name->len + 1;
  buflen -= gr_name->len + 1;

  result->gr_passwd = strncpy(buffer, gr_passwd->str, gr_passwd->len);
  buffer += gr_passwd->len + 1;
  buflen -= gr_passwd->len + 1;

  result->gr_gid = gid;

  status = _nss_redis_fill_group_gr_mem(c, gid, result, buffer, buflen);
 done:

  if(r) {
    freeReplyObject(r);
  }
  return status;
}


redisReply *
_nss_redis_get_string(redisReply *reply) {

  if( reply->type != REDIS_REPLY_STRING) {
    return &redis_reply_empty;
  }

  if( reply->len == 0 ) {
    return &redis_reply_empty;
  }

  return reply;
}

int
_nss_redis_get_uid_gid(redisReply *reply) {

  const char *uid_str;

 if(reply->type == REDIS_REPLY_STRING) {
    uid_str = reply->str;
    if(strcmp(uid_str, "0") == 0) {
      return 0;
    }

    uid_t uid = atoi(uid_str);
    if(uid == 0) {
      return NSS_REDIS_UID_GID_INVALID;
    }
    return uid;
  }

 if(reply->type == REDIS_REPLY_INTEGER) {
   return (uid_t)reply->integer;
 }

 return NSS_REDIS_UID_GID_NOTFOUND;
}

redisReply *
_nss_redis_pw_name(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_NAME]);
}

redisReply *
_nss_redis_pw_passwd(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_PASSWD]);
}

redisReply *
_nss_redis_pw_gecos(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_GECOS]);
}

redisReply *
_nss_redis_pw_dir(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_DIR]);
}

redisReply *
_nss_redis_pw_shell(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_SHELL]);
}

nss_status_t
_nss_redis_pw_uid(redisReply *r, uid_t *uid) {
  int id = _nss_redis_get_uid_gid(r->element[NSS_REDIS_PW_UID]);

  if(id == NSS_REDIS_UID_GID_INVALID) {
    return NSS_STATUS_UNAVAIL;
  }

  if(id == NSS_REDIS_UID_GID_NOTFOUND) {
    return NSS_STATUS_NOTFOUND;
  }

  *uid = (uid_t)id;
  return NSS_STATUS_SUCCESS;
}

nss_status_t
_nss_redis_pw_gid(redisReply *r, gid_t *gid) {
  int id = _nss_redis_get_uid_gid(r->element[NSS_REDIS_PW_GID]);

  if( id == NSS_REDIS_UID_GID_INVALID) {
    return NSS_STATUS_UNAVAIL;
  }

  if(id == NSS_REDIS_UID_GID_NOTFOUND) {
    return NSS_STATUS_NOTFOUND;
  }

  *gid = (gid_t)id;
  return NSS_STATUS_SUCCESS;
}

redisReply *
_nss_redis_gr_name(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_GR_NAME]);
}

redisReply *
_nss_redis_gr_passwd(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_GR_PASSWD]);
}

gid_t
_nss_redis_gr_gid(redisReply *r) {
  return (gid_t)_nss_redis_get_uid_gid(r->element[NSS_REDIS_GR_GID]);
}
