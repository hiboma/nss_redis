
#include "nss_redis.h"
#include <stdio.h>
#include <nss.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>

enum nss_status
_nss_redis_init (uid_t uid, struct passwd *result, char *buffer, size_t buflen, int *errnop) {
  fprintf(stderr, "_nss_redis_getpwuid_r %d %p %ld %d\n", uid, buffer, buflen, *errnop);
}

enum nss_status
_nss_redis_getpwuid_r (uid_t uid, struct passwd *result, char *buffer, size_t buflen, int *errnop) {
  //fprintf(stderr, "_nss_redis_getpwuid_r %d %p %ld %d\n", uid, buffer, buflen, *errnop);
  const char *pw_name = "hiboma";
  memset (buffer, 0, buflen);
  result->pw_name  = strncpy(buffer, pw_name, strlen(pw_name));
  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_redis_getgrgid_r (uid_t gid, struct group *result, char *buffer,
                       size_t buflen, int *errnop) {
  fprintf(stderr, "_nss_redis_getgrgid_r %d %p %ld %d\n", gid, buffer, buflen, *errnop);
  const char *gr_name = "hiboma";
  memset (buffer, 0, buflen);
  result->gr_name = strncpy(buffer, gr_name, strlen(gr_name));
  return NSS_STATUS_SUCCESS;
}

enum nss_status
_nss_redis_getpwnam_r (const char *name, struct passwd *result, char * buffer, size_t buflen, int *errnop) {
  fprintf(stderr, "%s %s\n", __func__, name);

  enum nss_status status;
  int offset = 0;
  redisContext *redis;

  redis = nss_redis_redis_connect();
  if(redis == NULL) {
    return NSS_STATUS_UNAVAIL;
  }
  return nss_redis_getpwnam(redis, name, result, buffer, buflen);
}

enum nss_status _nss_redis_getpwent_r (struct passwd *results, char * buffer, size_t buflen, int *errnop) { 
  return NSS_STATUS_SUCCESS;
}
