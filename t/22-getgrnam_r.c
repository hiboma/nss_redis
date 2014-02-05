#include "test_redis_pthread.h"

static void *
thread_getgrnam_r(void *args) {

  char buffer[BUFFER_LENGTH];
  struct group grp;
  struct group *result;

  for ( int i = 0; i < LOOP_COUNT; i++ ) {
    if(getgrnam_r(NSS_REDIS_TEST_ACCOUNT, &grp, buffer, BUFFER_LENGTH, &result) != 0) {
      perror("getpwnam_r");
    }
    assert(result);
    assert(strcmp(result->gr_name, NSS_REDIS_TEST_GROUP) == 0);
    assert(result->gr_mem);
  }
   return NULL;
}

int main() {
  return threaded_test(thread_getgrnam_r);
}
