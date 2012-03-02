
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use Config::Std;
use Test::More;

use TLib qw( get_output ok_command get_pid set_timeout );

my $problems_dir = './t/problems/';
my @ini_files    = glob $problems_dir . '*.ini';
my $pid          = get_pid();

plan tests => ( scalar @ini_files * 2 ) + 3;

set_timeout(120);

ok_command( 'level 1' );

foreach my $ini (@ini_files) {
    my %config;
    read_config( $ini, %config );

    my $file     = $config{''}{'File'};
    my $player   = $config{''}{'Player'};
    my $solution = $config{''}{'Solution'};

    ok_command( "loadsgf $file"   );
    ok_command( "genmove $player", $solution );
}

ok_command( 'quit ' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit_status 0' );

