#include "nss_redis.h"
#include "nss_redis_utils.h"

nss_status_t
_nss_redis_getpwuid(redisContext *c, uid_t uid, struct passwd *result,
                   char *buffer, size_t buflen) {
  return _nss_redis_fill_passwd(c, uid, result, buffer, buflen);
}
