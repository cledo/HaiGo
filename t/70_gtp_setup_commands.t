
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 9;

use TLib qw( get_output );

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

my $board_size = 3;

my $board_3 = <<"END_BOARD_3";
= 
    A B C
  3 . . . 3
  2 . . . 2\t    WHITE (0) has captured 0 stones
  1 . . . 1\t    BLACK (X) has captured 0 stones
    A B C

END_BOARD_3

print {$stdin} "komi 6.5\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'komi set' );

print {$stdin} "boardsize 0\n";
$output = get_output($stdout);
is( $output, "? unacceptable size\n\n", 'unacceptable size 0' );

print {$stdin} "boardsize 99\n";
$output = get_output($stdout);
is( $output, "? unacceptable size\n\n", 'unacceptable size 99' );

print {$stdin} "boardsize $board_size\n";
$output = get_output($stdout);
is( $output, "= \n\n", "board size set to $board_size" );
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_3, 'board size correct' );

print {$stdin} "play b a2\n";
get_output($stdout);
print {$stdin} "play w a1\n";
get_output($stdout);
# White stone gets captured:
print {$stdin} "play b b1\n";
get_output($stdout);

print {$stdin} "clear_board\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'board cleared' );
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_3, 'board size correct' );


#
# Quit:
#
print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

