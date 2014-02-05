use strict;
use warnings;
use Test::More;

is system("t/22-getgrnam_r"), 0;

done_testing;
