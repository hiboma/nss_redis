#ifndef NSS_REDIS_H
#define NSS_REDIS_H

#define _GNU_SOURCE 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <hiredis/hiredis.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <nss.h>

typedef enum nss_status nss_status_t;

nss_status_t
_nss_redis_getpwnam(redisContext *, const char *, struct passwd *, char *, size_t);

nss_status_t
_nss_redis_getpwuid(redisContext *, uid_t , struct passwd *, char *, size_t);

nss_status_t
_nss_redis_getgrnam(redisContext *, const char *, struct group *, char *, size_t);

nss_status_t
_nss_redis_getgrgid(redisContext *, gid_t, struct group *, char *, size_t);

#endif
