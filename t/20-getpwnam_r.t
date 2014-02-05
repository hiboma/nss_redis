use strict;
use warnings;
use Test::More;

is system("t/20-getpwnam_r"), 0;

done_testing;
