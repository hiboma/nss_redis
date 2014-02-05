#include "nss_redis.h"
#include "nss_redis_utils.h"

nss_status_t
_nss_redis_getgrnam(redisContext *c, const char *name, struct group *result,
                   char *buffer, size_t buflen) {

  nss_status_t status = NSS_STATUS_UNAVAIL;
  redisReply *r;

  r = redisCommand(c, "GET group:%s", name);
  if(r == NULL) {
    return status;
  }

  gid_t gid = (gid_t)_nss_redis_get_uid_gid(r);

  switch(gid) {
  case NSS_REDIS_UID_GID_INVALID:
    status = NSS_STATUS_UNAVAIL;
    break;
  case NSS_REDIS_UID_GID_NOTFOUND:
    status = NSS_STATUS_NOTFOUND;
    break;
  default:
    status = _nss_redis_fill_group(c, gid, result, buffer, buflen);
  }

  if(r) {
    freeReplyObject(r);
  }

  return status;
}
