#include <hiredis/hiredis.h>
#include <sys/types.h>
#include <pwd.h>
#include <nss.h>
#include <string.h>

enum nss_status
nss_redis_getpwnam(redisContext *, const char *, struct passwd *, char *, size_t);

redisReply *
_nss_redis_get_string(const redisReply *);

int
_nss_redis_get_uid_gid(const redisReply *r);

