#include "nss_redis.h"
#include "nss_redis_utils.h"

nss_status_t
_nss_redis_getpwnam(redisContext *c, const char *account, struct passwd *result,
                   char *buffer, size_t buflen) {

  nss_status_t status = NSS_STATUS_UNAVAIL;
  redisReply *r;

  r = redisCommand(c, "GET %s", account);
  if(r == NULL) {
    return status;
  }

  uid_t uid = (uid_t)_nss_redis_get_uid_gid(r);
  switch(uid) {
  case NSS_REDIS_UID_GID_INVALID:
    status = NSS_STATUS_UNAVAIL;
    break;
  case NSS_REDIS_UID_GID_NOTFOUND:
    status = NSS_STATUS_NOTFOUND;
    break;
  default:
    status = _nss_redis_fill_passwd(c, uid, result, buffer, buflen);
  }

  if(r) {
    freeReplyObject(r);
  }

  return status;
}
