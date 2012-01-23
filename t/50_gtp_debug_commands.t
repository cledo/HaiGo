
use strict;
use warnings;
use Carp;

use lib qw(t/lib);

use IPC::Open3;

use Test::More tests => 7;

use TLib qw(get_output);

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

my $board_19 = <<"END_BOARD_19";
= 
    A B C D E F G H J K L M N O P Q R S T
 19 . . . . . . . . . . . . . . . . . . . 19
 18 . . . . . . . . . . . . . . . . . . . 18
 17 . . . . . . . . . . . . . . . . . . . 17
 16 . . . + . . . . . + . . . . . + . . . 16
 15 . . . . . . . . . . . . . . . . . . . 15
 14 . . . . . . . . . . . . . . . . . . . 14
 13 . . . . . . . . . . . . . . . . . . . 13
 12 . . . . . . . . . . . . . . . . . . . 12
 11 . . . . . . . . . . . . . . . . . . . 11
 10 . . . + . . . . . + . . . . . + . . . 10
  9 . . . . . . . . . . . . . . . . . . . 9
  8 . . . . . . . . . . . . . . . . . . . 8
  7 . . . . . . . . . . . . . . . . . . . 7
  6 . . . . . . . . . . . . . . . . . . . 6
  5 . . . . . . . . . . . . . . . . . . . 5
  4 . . . + . . . . . + . . . . . + . . . 4
  3 . . . . . . . . . . . . . . . . . . . 3
  2 . . . . . . . . . . . . . . . . . . . 2
  1 . . . . . . . . . . . . . . . . . . . 1
    A B C D E F G H J K L M N O P Q R S T

END_BOARD_19

my $board_13 = <<"END_BOARD_13";
= 
    A B C D E F G H J K L M N
 13 . . . . . . . . . . . . . 13
 12 . . . . . . . . . . . . . 12
 11 . . . . . . . . . . . . . 11
 10 . . . + . . . . . + . . . 10
  9 . . . . . . . . . . . . . 9
  8 . . . . . . . . . . . . . 8
  7 . . . . . . + . . . . . . 7
  6 . . . . . . . . . . . . . 6
  5 . . . . . . . . . . . . . 5
  4 . . . + . . . . . + . . . 4
  3 . . . . . . . . . . . . . 3
  2 . . . . . . . . . . . . . 2
  1 . . . . . . . . . . . . . 1
    A B C D E F G H J K L M N

END_BOARD_13

my $board_9 = <<"END_BOARD_9";
= 
    A B C D E F G H J
  9 . . . . . . . . . 9
  8 . . . . . . . . . 8
  7 . . + . . . + . . 7
  6 . . . . . . . . . 6
  5 . . . . + . . . . 5
  4 . . . . . . . . . 4
  3 . . + . . . + . . 3
  2 . . . . . . . . . 2
  1 . . . . . . . . . 1
    A B C D E F G H J

END_BOARD_9

my $board_min = <<"END_BOARD_MIN";
= 
    A B
  2 . . 2
  1 . . 1
    A B

END_BOARD_MIN

my $board_max = <<"END_BOARD_MAX";
= 
    A B C D E F G H J K L M N O P Q R S T U V W X Y Z
 25 . . . . . . . . . . . . . . . . . . . . . . . . . 25
 24 . . . . . . . . . . . . . . . . . . . . . . . . . 24
 23 . . . . . . . . . . . . . . . . . . . . . . . . . 23
 22 . . . . . . . . . . . . . . . . . . . . . . . . . 22
 21 . . . . . . . . . . . . . . . . . . . . . . . . . 21
 20 . . . . . . . . . . . . . . . . . . . . . . . . . 20
 19 . . . . . . . . . . . . . . . . . . . . . . . . . 19
 18 . . . . . . . . . . . . . . . . . . . . . . . . . 18
 17 . . . . . . . . . . . . . . . . . . . . . . . . . 17
 16 . . . . . . . . . . . . . . . . . . . . . . . . . 16
 15 . . . . . . . . . . . . . . . . . . . . . . . . . 15
 14 . . . . . . . . . . . . . . . . . . . . . . . . . 14
 13 . . . . . . . . . . . . . . . . . . . . . . . . . 13
 12 . . . . . . . . . . . . . . . . . . . . . . . . . 12
 11 . . . . . . . . . . . . . . . . . . . . . . . . . 11
 10 . . . . . . . . . . . . . . . . . . . . . . . . . 10
  9 . . . . . . . . . . . . . . . . . . . . . . . . . 9
  8 . . . . . . . . . . . . . . . . . . . . . . . . . 8
  7 . . . . . . . . . . . . . . . . . . . . . . . . . 7
  6 . . . . . . . . . . . . . . . . . . . . . . . . . 6
  5 . . . . . . . . . . . . . . . . . . . . . . . . . 5
  4 . . . . . . . . . . . . . . . . . . . . . . . . . 4
  3 . . . . . . . . . . . . . . . . . . . . . . . . . 3
  2 . . . . . . . . . . . . . . . . . . . . . . . . . 2
  1 . . . . . . . . . . . . . . . . . . . . . . . . . 1
    A B C D E F G H J K L M N O P Q R S T U V W X Y Z

END_BOARD_MAX


print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_19, 'correct ASCII board shown (19x19)' );

print {$stdin} "boardsize 13\n";
get_output($stdout);
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_13, 'correct ASCII board shown (13x13)' );

print {$stdin} "boardsize 9\n";
get_output($stdout);
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_9, 'correct ASCII board shown (9x9)' );

print {$stdin} "boardsize 2\n";
get_output($stdout);
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_min, 'correct ASCII board shown (2x2)' );

print {$stdin} "boardsize 25\n";
get_output($stdout);
print {$stdin} "showboard\n";
$output = get_output($stdout);
is( $output, $board_max, 'correct ASCII board shown (25x25)' );



print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );
