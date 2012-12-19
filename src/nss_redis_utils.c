#include "nss_redis.h"
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <sys/types.h>

static const char* nss_redis_redis_server = "127.0.0.1";
static int         nss_redis_redis_port   = 6379;

static struct redisReply redis_reply_empty = {
  .str = "",
  .len = 0,
};

redisContext*
nss_redis_redis_connect() {

  /* Singleton */
  static redisContext *c = NULL;
  if (c)  {
    return c;
  }

  c = redisConnect(nss_redis_redis_server, nss_redis_redis_port);
  if(c->err) {
    redisFree(c);
    c = NULL;
    return NULL;
  }

  return c;
}

redisReply *
_nss_redis_get_string(const redisReply *reply) {

  if( reply == NULL ) {
    return NULL;
  }

  if( reply->type != REDIS_REPLY_STRING) {
    return &redis_reply_empty;
  }

  return reply;
}

int
_nss_redis_get_uid_gid(const redisReply *reply) {

  const char *uid_str;

  switch( reply->type ) {
  case REDIS_REPLY_STRING:
    uid_str = reply->str;
    if(!strcmp(uid_str, "0")) {
      return 0;
    }
    uid_t uid = atoi(uid_str);
    if(uid == 0) {
      return -1;
    }
    return uid;
    break;
  case REDIS_REPLY_INTEGER:
    return (uid_t)reply->integer;
    break;
  case REDIS_REPLY_NIL:
  default:
   return -1;
  }

}

