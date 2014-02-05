#include "src/nss_redis.h"
#include "src/nss_redis_utils.h"
#include <unistd.h>
#include <err.h>

int main(int argc, const char *argv[]) {

  if(argc != 2) {
    errx(1, "usage: 10-nss_redis_config /path/to/libnss-redis.cfg");
  }

  const char *path_to_conf = argv[1];
  if(access(path_to_conf, F_OK) == -1) {
    err(1, "failed to access:");
  }

  nss_status_t status;
  status = _nss_redis_load_config(path_to_conf);
  if( status != NSS_STATUS_SUCCESS) {
    errx(1, "failed _nss_redis_load_config");
  }

  printf("host %s\n", _nss_redis_config.host);
  printf("port %d\n", _nss_redis_config.port);
  printf("password %s\n", _nss_redis_config.password);

  return 0;
}
