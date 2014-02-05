use strict;
use warnings;
use Test::More;

system("redis-cli mset 10000:name hoge 10000:passwd password 10000:uid 10000 10000:gid 10000 10000:shell /bin/bash 10000:dir /home/hoge");
system("redis-cli set hoge 10000");

my ($name, $passwd, $uid, $gid, undef, undef, undef, $dir, $shell) = getpwnam("hoge");

is $name, "hoge";
is $passwd, "password";
is $uid, 10000;
is $gid, 10000;
is $dir, "/home/hoge";
is $shell, "/bin/bash";

is getpwnam("fugafuga"), undef;

done_testing;

