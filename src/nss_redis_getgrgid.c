#include "nss_redis.h"
#include "nss_redis_utils.h"

nss_status_t
_nss_redis_getgrgid(redisContext *c, gid_t gid, struct group *result,
                   char *buffer, size_t buflen) {
  return _nss_redis_fill_group(c, gid, result, buffer, buflen);
}
