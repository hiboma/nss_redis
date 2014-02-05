#ifndef TEST_REDIS_PTHREAD
#define TEST_REDIS_PTHREAD

#include <unistd.h>
#include <err.h>
#include <pthread.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define THREAD_COUNT  50
#define LOOP_COUNT    10
#define BUFFER_LENGTH 1024
#define NSS_REDIS_TEST_ACCOUNT "hoge"
#define NSS_REDIS_TEST_UID     10000
#define NSS_REDIS_TEST_GID     10000
#define NSS_REDIS_TEST_GROUP   "hoge"

int
threaded_test (void *(*func)(void *args)) {

  pthread_t thread[THREAD_COUNT];

  for(int i = 0; i < THREAD_COUNT; i++) {
    pthread_create(&thread[i], NULL, func, (void *)NULL);
  }

  for(int i = 0; i < THREAD_COUNT; i++) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}

#endif
