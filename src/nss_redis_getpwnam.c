#include "nss_redis.h"

#define NSS_REDIS_PW_NAME   0
#define NSS_REDIS_PW_PASSWD 1
#define NSS_REDIS_PW_UID    2
#define NSS_REDIS_PW_GID    3
#define NSS_REDIS_PW_GECOS  4
#define NSS_REDIS_PW_DIR    5
#define NSS_REDIS_PW_SHELL  6

#define repeat7(s) s, s, s, s, s, s, s

static const char *GETPWNAM_COMMAND = "MGET %s:name %s:passwd %s:uid %s:gid %s:gecos %s:dir %s:shell";

static redisReply *
nss_redis_getpwnam_name(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_NAME]);
}

static redisReply *
nss_redis_getpwnam_passwd(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_PASSWD]);
}

static redisReply *
nss_redis_getpwnam_gecos(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_GECOS]);
}

static redisReply *
nss_redis_getpwnam_dir(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_DIR]);
}

static redisReply *
nss_redis_getpwnam_shell(redisReply *r) {
  return _nss_redis_get_string(r->element[NSS_REDIS_PW_SHELL]);
}

static uid_t
nss_redis_getpwnam_uid(redisReply *r) {
  return (uid_t)_nss_redis_get_uid_gid(r->element[NSS_REDIS_PW_UID]);
}

static gid_t
nss_redis_getpwnam_gid(redisReply *r) {
  return (gid_t)_nss_redis_get_uid_gid(r->element[NSS_REDIS_PW_GID]);
}

enum nss_status
nss_redis_getpwnam(redisContext *c, const char *account, struct passwd *result,
                   char *buffer, size_t buflen) {

  enum nss_status status = NSS_STATUS_UNAVAIL;
  redisReply *name, *dir, *shell, *passwd, *gecos;

  redisReply*r = redisCommand(c, GETPWNAM_COMMAND, repeat7(account));
  if(r == NULL) {
    goto processed;
  }

  if(r->type == REDIS_REPLY_ERROR ||
     r->type != REDIS_REPLY_ARRAY ||
     r->elements != 7 ) {
    goto processed;
  }

  result->pw_uid  = nss_redis_getpwnam_uid(r);
  result->pw_gid  = nss_redis_getpwnam_gid(r);
  if(result->pw_uid == -1 || result->pw_gid == -1 ) {
    goto processed;
  }

  name = nss_redis_getpwnam_name(r);
  if(name->len == 0) {
    goto processed;
  }

  shell  = nss_redis_getpwnam_shell(r);
  dir    = nss_redis_getpwnam_dir(r);
  gecos  = nss_redis_getpwnam_gecos(r);
  passwd = nss_redis_getpwnam_passwd(r);
  if(shell == NULL || dir   == NULL || gecos == NULL || passwd == NULL ) {
    goto processed;
  }

  if((name->len  + shell->len + dir->len + gecos->len + passwd->len) > buflen) {
    status = NSS_STATUS_TRYAGAIN;
    goto processed;
  }

  size_t offset = 0;
  memset(buffer, 0, buflen);

  result->pw_name = strncpy(buffer + offset, name->str, name->len);
  offset += name->len +1;

  result->pw_shell = strncpy(buffer + offset, shell->str, shell->len);
  offset += shell->len + 1;

  result->pw_gecos = strncpy(buffer + offset, gecos->str, gecos->len);
  offset += gecos->len + 1;

  result->pw_dir = strncpy(buffer + offset, dir->str, dir->len);
  offset += dir->len + 1;

  result->pw_passwd = strncpy(buffer + offset, passwd->str, passwd->len);

  status = NSS_STATUS_SUCCESS;

 processed:
  if(r != NULL) {
    freeReplyObject(r);
  }
  return status;
}

/* void */
/* _nss_redis_getpwnam_print(redisContext *c, const char *account) { */
/*     redisReply *r         = nss_redis_getpwnam(c, account); */
/*     const char *pw_name   = nss_redis_getpwnam_name(r); */
/*     const char *pw_passwd = nss_redis_getpwnam_passwd(r); */
/*     const char *pw_gecos  = nss_redis_getpwnam_gecos(r); */
/*     const char *pw_dir    = nss_redis_getpwnam_dir(r); */
/*     const char *pw_shell  = nss_redis_getpwnam_shell(r); */
/*     uid_t pw_uid          = nss_redis_getpwnam_uid(r); */
/*     uid_t pw_gid          = nss_redis_getpwnam_gid(r); */
/*     printf("%s:%s:%d:%d:%s:%s:%s\n", pw_name, pw_passwd, pw_uid, pw_gid, pw_gecos, pw_dir, pw_shell); */
/*     freeReplyObject(r); */
/* } */
