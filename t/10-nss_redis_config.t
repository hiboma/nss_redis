use strict;
use warnings;
use Test::More;
use File::Temp qw/tempfile/;

sub tempconfig {
    my $config = shift;
    my $tmp = File::Temp->new; 
    $tmp->print($config);
    $tmp->close;
    $tmp;
}

sub test_config { 
    my ( $path, $expected ) = @_;
    my %config = map { split /\s+/ } split /\n/, `./t/10-nss_redis_config $path`;
    is_deeply \%config, $expected; 
}

{ 
    my $config = tempconfig(<<'....');
host /var/tmp/redis.sock
password password
port 6379
....

    test_config $config, +{ 
      host     => "/var/tmp/redis.sock", 
      password => "password", 
      port     => 6379 
    };
}


{
    my $config = tempconfig(<<'....');
host 192.168.0.1
password abcdefghijklmnopqrstuvwxyz0123456789 
port 6380
....

    test_config $config, +{
      host     => "192.168.0.1",
      password => "abcdefghijklmnopqrstuvwxyz0123456789",
      port     => 6380 
    };
}

{
    my $config = tempconfig(<<'....');
# host 127.0.0.1
host 192.168.0.1
# password password 
password abcdefghijklmnopqrstuvwxyz0123456789 
# prot 6379
port 6380
....

    test_config $config, +{
      host     => "192.168.0.1",
      password => "abcdefghijklmnopqrstuvwxyz0123456789",
      port     => 6380
    };
}

done_testing;
