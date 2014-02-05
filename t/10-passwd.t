use strict;
use warnings;
use t::nss_redis;
use Redis;

fork_redis(setup_redis_config);

system "t/10-passwd";
