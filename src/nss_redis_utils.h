#ifndef NSS_REDIS_UTILS_H
#define NSS_REDIS_UTILS_H

#define _GNU_SOURCE

#include <hiredis/hiredis.h>
#include <nss.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#ifdef DEBUG
#define D(str) fprintf(stderr, "%s", str);
#else
#define D(str)
#endif

typedef struct {
  char* host;
  int   port;
  char* password;
} _nss_redis_config_t;

extern _nss_redis_config_t _nss_redis_config;

enum {
  NSS_REDIS_PW_NAME,
  NSS_REDIS_PW_PASSWD,
  NSS_REDIS_PW_UID,
  NSS_REDIS_PW_GID,
  NSS_REDIS_PW_GECOS,
  NSS_REDIS_PW_DIR,
  NSS_REDIS_PW_SHELL,
  NSS_REDIS_PW_NUM_ENTRIES
} pw_enum;

enum {
  NSS_REDIS_GR_NAME,
  NSS_REDIS_GR_PASSWD,
  NSS_REDIS_GR_GID,
  NSS_REDIS_GR_NUM_ENTRIES
} gr_enum;

#define NSS_REDIS_UID_GID_INVALID  -1
#define NSS_REDIS_UID_GID_NOTFOUND -2


#define repeat7(s) s, s, s, s, s, s, s
#define repeat4(s) s, s, s

redisContext*
_nss_redis_redis_connect();

void
_nss_redis_redis_disconnect();

nss_status_t
_nss_redis_load_config(const char *);

void
_nss_redis_redis_clean_passwod();

redisReply *
_nss_redis_pw_redis_command(redisContext *, uid_t uid);

redisReply *
_nss_redis_gr_redis_command(redisContext *, gid_t uid);

nss_status_t
_nss_redis_fill_passwd(redisContext *c, uid_t, struct passwd *, char *, size_t );

nss_status_t
_nss_redis_fill_group(redisContext *c, gid_t, struct group *, char *, size_t );

/* getpwname , getpwuid */
redisReply *
_nss_redis_pw_name(redisReply *);

redisReply *
_nss_redis_pw_passwd(redisReply *);

redisReply *
_nss_redis_pw_gecos(redisReply *);

redisReply *
_nss_redis_pw_dir(redisReply *);

redisReply *
_nss_redis_pw_shell(redisReply *);

nss_status_t
_nss_redis_pw_uid(redisReply *, uid_t *);

nss_status_t
_nss_redis_pw_gid(redisReply *, gid_t *);

/* getgrnam, getpwuid */
redisReply *
_nss_redis_gr_name(redisReply *);

redisReply *
_nss_redis_gr_passwd(redisReply *);

gid_t
_nss_redis_gr_gid(redisReply *);

/* util */
redisReply *
_nss_redis_get_string(redisReply *);

int
_nss_redis_get_uid_gid(redisReply *);

#endif
