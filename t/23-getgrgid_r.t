use strict;
use warnings;
use Test::More;

is system("t/23-getgrgid_r"), 0;

done_testing;
