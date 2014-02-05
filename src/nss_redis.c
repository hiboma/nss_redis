#include "nss_redis.h"
#include "nss_redis_utils.h"

static pthread_mutex_t lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static bool atexit_isset  = false;
static bool loaded_config = false;

static void
_nss_redis_atexit_handler (void)
{
  pthread_mutex_lock(&lock);
  _nss_redis_redis_disconnect();
  _nss_redis_redis_clean_passwod();
  pthread_mutex_unlock(&lock);
}

void
_nss_redis_init () {

  if(loaded_config == false ) {
    _nss_redis_load_config(NULL);
    loaded_config = true;
  }

  if(atexit_isset == false) {
    if(atexit(_nss_redis_atexit_handler) == 0) {
      atexit_isset = true;
    }
  }

}

nss_status_t
_nss_redis_getpwuid_r (uid_t uid, struct passwd *result, char *buffer,
                       size_t buflen, int *errnop) {

  pthread_mutex_lock(&lock);
  nss_status_t status;

  _nss_redis_init();

  redisContext *redis = _nss_redis_redis_connect();
  if(redis == NULL) {
    status = NSS_STATUS_UNAVAIL;
  } else {
    status = _nss_redis_getpwuid(redis, uid, result, buffer, buflen);
  }

  pthread_mutex_unlock(&lock);
  return status;
}

nss_status_t
_nss_redis_getpwnam_r (const char *name, struct passwd *result, char * buffer,
                       size_t buflen, int *errnop) {

  pthread_mutex_lock(&lock);
  nss_status_t status;

  _nss_redis_init();

  redisContext *redis = _nss_redis_redis_connect();
  if(redis == NULL) {
    status = NSS_STATUS_UNAVAIL;
  } else { 
    status = _nss_redis_getpwnam(redis, name, result, buffer, buflen);
  }

  pthread_mutex_unlock(&lock);
  return status;
}

nss_status_t
_nss_redis_getgrnam_r (const char *name, struct group *result, char *buffer,
                       size_t buflen, int *errnop) {

  pthread_mutex_lock(&lock);
  nss_status_t status;

  _nss_redis_init();

  redisContext *redis = _nss_redis_redis_connect();
  if(redis == NULL) {
    status = NSS_STATUS_UNAVAIL;
  } else {
    status = _nss_redis_getgrnam(redis, name, result, buffer, buflen);
  }

  pthread_mutex_unlock(&lock);
  return status;
}

nss_status_t
_nss_redis_getgrgid_r (gid_t gid, struct group *result, char *buffer,
                       size_t buflen, int *errnop) {
  pthread_mutex_lock(&lock);
  nss_status_t status;

  _nss_redis_init();

  redisContext *redis = _nss_redis_redis_connect();
  if(redis == NULL) {
    status = NSS_STATUS_UNAVAIL;
  } else { 
    status = _nss_redis_getgrgid(redis, gid, result, buffer, buflen);
  }

  pthread_mutex_unlock(&lock);
  return status;
}

