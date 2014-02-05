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
test_getpwnam(redisContext *r) {

  struct passwd pwd;
  char buffer[1024];

  ok(_nss_redis_getpwnam(r, "hoge", &pwd, buffer, 1024) == NSS_STATUS_SUCCESS,
     "_nss_redis_getpwnam should return NSS_STATUS_SUCCESS");

  is(pwd.pw_name, "hoge", "_nss_redis_getpwnam should fill pw_passwd");
  is(pwd.pw_passwd, "password", "_nss_redis_getpwnam should fill pw_passwd");
  is(pwd.pw_dir, "/home/hoge", "_nss_redis_getpwnam should fill pw_dir");
  ok(pwd.pw_uid == 10000, "_nss_redis_getpwnam should fill pw_uid");
  ok(pwd.pw_gid == 10000, "_nss_redis_getpwnam should fill pw_gid");

  ok(_nss_redis_getpwnam(r, "notfound", &pwd, buffer, 1024) == NSS_STATUS_NOTFOUND,
     "_nss_redis_getpwnam should return NSS_STATUS_NOTFOUND");
}

static void
test_getpwuid(redisContext *r) {

  struct passwd pwd;
  char buffer[1024];

  ok(_nss_redis_getpwuid(r, 10000, &pwd, buffer, 1024) == NSS_STATUS_SUCCESS,
     "_nss_redis_getpwnuid should return NSS_STATUS_SUCCESS");
  is(pwd.pw_name, "hoge", "_nss_redis_getpwuid should fill pw_passwd");
  is(pwd.pw_passwd, "password", "_nss_redis_getpwuid should fill pw_passwd");
  is(pwd.pw_dir, "/home/hoge", "_nss_redis_getpwuid should fill pw_dir");
  ok(pwd.pw_uid == 10000, "_nss_redis_getpwuid should fill pw_uid");
  ok(pwd.pw_gid == 10000, "_nss_redis_getpwuid should fill pw_gid");

  ok(_nss_redis_getpwuid(r, 9999999, &pwd, buffer, 1024) == NSS_STATUS_NOTFOUND,
     "_nss_redis_getpwnuid should return NSS_STATUS_NOTFOUND");
}

int main(){

  char *socket = getenv("REDIS_UNIX_SOCKET");
  if(!socket) {
    errx(1, "env REDIS_SOCKET is not defined");
  }

  redisContext *r = redisConnectUnix(socket);
  if(r->err) {
    errx(1, "failed redisConnectUnix: '%s' %s", socket, r->errstr);
  }

  test_getpwnam(r);
  test_getpwuid(r);
  done_testing();
  exit(0);
}

