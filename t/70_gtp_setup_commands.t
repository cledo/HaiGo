
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 12;

use TLib qw( get_output ok_command get_pid );

my $pid = get_pid();

my $board_size = 3;

my $board_3 = <<"END_BOARD_3";

    A B C
  3 . . . 3
  2 . . . 2\t    WHITE (0) has captured 0 stones
  1 . . . 1\t    BLACK (X) has captured 0 stones
    A B C
END_BOARD_3
chomp $board_3;

ok_command( 'komi 6.5' );
ok_command( 'boardsize 0', 'unacceptable size', 1 );
ok_command( 'boardsize 99', 'unacceptable size', 1 );
ok_command( "boardsize $board_size" );
ok_command( 'showboard', $board_3 );
ok_command( 'play b a2' );
ok_command( 'play w a1' );
# White stone gets captured:
ok_command( 'play b b1' );

ok_command( 'clear_board' );
ok_command( 'showboard', $board_3 );

#
# Quit:
#
ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

