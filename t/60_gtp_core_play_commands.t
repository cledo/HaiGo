
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 54;

use TLib qw( get_output ok_command get_pid );

my $pid = get_pid();

ok_command( 'play XYZ A1', 'invalid color', 1 );
ok_command( 'play BLACK A99', 'invalid coordinate', 1 );
ok_command( 'play BLACK ?9', 'invalid coordinate', 1 );
ok_command( 'play BLACK I9', 'invalid coordinate', 1 );
ok_command( 'play BLACK i9', 'invalid coordinate', 1 );
ok_command( 'play BLACK A-1', 'invalid coordinate', 1 );
ok_command( 'play BLACK A1' );
ok_command( 'play WHITE A1', 'illegal move', 1 );
ok_command( 'play WHITE A2' );
ok_command( 'play BLACK C1' );
ok_command( 'play WHITE B1' );

#
# Check ko:
#
ok_command( 'boardsize 5' );
ok_command( 'play BLACK A2' );
ok_command( 'play WHITE D2' );
ok_command( 'play BLACK B1' );
ok_command( 'play WHITE C1' );
ok_command( 'play BLACK B3' );
ok_command( 'play WHITE C3' );
ok_command( 'play BLACK C2' );
ok_command( 'play WHITE B2' );
ok_command( 'play BLACK C2', 'illegal move', 1 );
ok_command( 'play WHITE PASS' );
ok_command( 'play WHITE A3' );
ok_command( 'play BLACK A1', 'illegal move', 1 );

#
# Check genmove
#
ok_command( 'boardsize 3' );
ok_command( 'genmove', 'invalid color', 1 );
ok_command( 'genmove XYZ', 'invalid color', 1 );
ok_command( 'play WHITE b3' );
ok_command( 'play WHITE b2' );
ok_command( 'play WHITE c2' );
ok_command( 'play BLACK a3' );
ok_command( 'play BLACK a2' );
ok_command( 'play BLACK b1' );
ok_command( 'play BLACK c1' );
ok_command( 'genmove BLACK', 'C3' );
ok_command( 'clear_board' );
ok_command( 'play BLACK b3' );
ok_command( 'play BLACK b2' );
ok_command( 'play BLACK c2' );
ok_command( 'play WHITE a3' );
ok_command( 'play WHITE a2' );
ok_command( 'play WHITE b1' );
ok_command( 'play WHITE c1' );
ok_command( 'genmove WHITE', 'C3' );
ok_command( 'clear_board' );
ok_command( 'play BLACK A3' );
ok_command( 'play BLACK A2' );
ok_command( 'play WHITE B3' );
ok_command( 'play WHITE B2' );
ok_command( 'play WHITE B1' );
ok_command( 'play WHITE C2' );
ok_command( 'genmove BLACK', 'pass' );


#
# Quit:
#
ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

