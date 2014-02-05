noinst_PROGRAMS             = 10-passwd 11-group 10-nss_redis_config 20-getpwnam_r 21-getpwuid_r 22-getgrnam_r 23-getgrgid_r
10_nss_redis_config_SOURCES = 10-nss_redis_config.c ../src/nss_redis_utils.c
10_nss_redis_config_LDADD   = -lpthread
# http://www.geocities.jp/fut_nis/html/automake-ja/Libtool-Issues.html 
#
#      object `foo.$(OBJEXT)' created both with libtool and without
#
10_nss_redis_config_CFLAGS  = $(AM_CFLAGS)

20_getpwnam_r_SOURCES = 20-getpwnam_r.c
20_getpwnam_r_LDADD   = -lpthread

21_getpwuid_r_SOURCES = 21-getpwuid_r.c
21_getpwuid_r_LDADD   = -lpthread

22_getgrnam_r_SOURCES = 22-getgrnam_r.c
22_getgrnam_r_LDADD   = -lpthread

23_getgrgid_r_SOURCES = 23-getgrgid_r.c
23_getgrgid_r_LDADD   = -lpthread

10_passwd_SOURCES = 10-passwd.c \
../src/nss_redis_utils.c    \
../src/nss_redis_getpwnam.c \
../src/nss_redis_getpwuid.c
10_passwd_LDADD = -lhiredis

11_group_SOURCES = 11-group.c \
../src/nss_redis_utils.c    \
../src/nss_redis_getgrnam.c \
../src/nss_redis_getgrgid.c
11_group_LDADD  = -lhiredis