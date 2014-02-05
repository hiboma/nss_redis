# http://www.spa.is.uec.ac.jp/~kinuko/slidemaker/autotools/slide15-0.html
lib_LTLIBRARIES = libnss_redis.la
libnss_redis_la_SOURCES = \
nss_redis.c          \
nss_redis_utils.c    \
nss_redis_getpwnam.c \
nss_redis_getpwuid.c \
nss_redis_getgrnam.c \
nss_redis_getgrgid.c

libnss_redis_la_LIBADD  = -lpthread
libnss_redis_la_LDFLAGS = -module -version-info 0:0:0
