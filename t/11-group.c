#include "src/nss_redis.h"
#include "src/nss_redis_utils.h"
#include "nanotap.h"

#include <stdlib.h>
#include <assert.h>
#include <err.h>

static bool
is(const char *a, const char *b, const char *message) {
  ok(strcmp(a, b) == 0, message);
}

static void
test_getgrnam(redisContext *c) {

  struct group grp;
  char buffer[4092];

  ok(_nss_redis_getgrnam(c, "hoge", &grp, buffer, strlen(buffer)) == NSS_STATUS_SUCCESS,
     "_nss_redis_getgrnam should return NSS_STATUS_SUCCESS");
  is(grp.gr_name, "hoge", "");
  is(grp.gr_passwd, "password", "");
  ok(grp.gr_gid == 10000, "");

  ok(_nss_redis_getgrnam(c, "notfound", &grp, buffer, strlen(buffer)) == NSS_STATUS_NOTFOUND,
     "_nss_redis_getgram should return NSS_STATUS_NOTFOUND");
}

static void
test_getgrgid(redisContext *c) {

  struct group grp;
  char buffer[4092];

  ok(_nss_redis_getgrgid(c, 10000, &grp, buffer, strlen(buffer)) == NSS_STATUS_SUCCESS,
     "_nss_redis_getgrnam should return NSS_STATUS_SUCCESS");
  is(grp.gr_name, "hoge", "");
  is(grp.gr_passwd, "password", "");
  ok(grp.gr_gid == 10000, "");

  ok(_nss_redis_getgrgid(c, 9999999, &grp, buffer, strlen(buffer)) == NSS_STATUS_NOTFOUND,
     "_nss_redis_getgram should return NSS_STATUS_NOTFOUND");
}

int main(){

  char *socket = getenv("REDIS_UNIX_SOCKET");
  if(!socket) {
    errx(1, "env REDIS_SOCKET is not defined");
  }

  redisContext *c = redisConnectUnix(socket);
  if(c->err) {
    errx(1, "failed redisConnectUnix: '%s' %s", socket, c->errstr);
  }

  test_getgrnam(c);
  test_getgrgid(c);
  done_testing();
  exit(0);
}

