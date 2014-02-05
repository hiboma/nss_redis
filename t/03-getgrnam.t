use strict;
use warnings;
use Test::More;

system("redis-cli set group:hoge 10000");
system("redis-cli sadd gid:10000 hoge");

my ($name, $passwd, $gid, $members) = getgrnam("hoge");

my %members = map { $_ => 1 }split /\s/, $members;

is $name, "hoge";
is $passwd, "password";
is $gid, 10000;
is_deeply \%members, +{ fuga => 1, hoge => 1, hige => 1 };

is getgrnam("fugafuga"), undef;

done_testing;


