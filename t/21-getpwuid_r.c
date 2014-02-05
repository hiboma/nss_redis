#include "test_redis_pthread.h"

static void *
thread_getpwuid_r(void *args) {

  char buffer[BUFFER_LENGTH];
  struct passwd pwd;
  struct passwd *result;

  for ( int i = 0; i < LOOP_COUNT; i++ ) {
    if(getpwuid_r(NSS_REDIS_TEST_UID, &pwd, buffer, BUFFER_LENGTH, &result) != 0) {
      perror("getpwnam_r");
    }
    assert(result);
    assert(strcmp(result->pw_name, NSS_REDIS_TEST_ACCOUNT) == 0);
  }
   return NULL;
}

int main() {
  return threaded_test(thread_getpwuid_r);
}
