use strict;
use warnings;
use Test::More;

is system("t/21-getpwuid_r"), 0;

done_testing;
