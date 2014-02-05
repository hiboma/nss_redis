use strict;
use warnings;
use Test::More;
use IO::File;
use File::Temp qw/tempdir tempfile/;
use Redis;

my $on_end;

sub setup_redis_config {
    my $dir = tempdir;
    my $fh  = IO::File->new("$dir/redis.cfg", "w") or die $!;
    $fh->print(<<"....");
port 0
unixsocket $dir/redis.sock
loglevel warning
....
    $fh->close;

    $ENV{REDIS_UNIX_SOCKET} = "$dir/redis.sock";
    $dir;
}

sub fork_redis {
    my $config_dir = shift;
    my $pid = fork;
    if($pid) {
        select undef, undef, undef, 0.5;

        my $redis = Redis->new( sock => "$config_dir/redis.sock" );

        my %key_values = (
            # passwd
            'hoge'         => 10000,
            '10000:name'   => 'hoge',
            '10000:passwd' => 'password',
            '10000:uid'    => 10000,
            '10000:gid'    => 10000,
            '10000:shell'  => '/bin/bash',
            '10000:dir'    => '/home/hoge',
            # group
            'group:hoge'   => 10000,
            '10000:name'   => 'hoge',
            '10000:passwd' => 'password',
            '10000:gid'    => 10000,
        );

        while (my ($key, $value) = each %key_values) {
            $redis->set($key, $value);
        }
        $redis->sadd('gid:1000',"hoge");

        $on_end = sub { kill 9, $pid };
        return $pid;
    }
    exec "redis-server $config_dir/redis.cfg";
}

END {
  File::Temp::cleanup();
  $on_end->();
}

1;
