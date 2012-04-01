
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 11;

use TLib qw( ok_command get_pid );

my $pid = get_pid();

my $board_19 = <<"END_BOARD_19";

    A B C D E F G H J K L M N O P Q R S T
 19 . . . . . . . . . . . . . . . . . . . 19
 18 . . . . . . . . . . . . . . . . . . . 18
 17 . . . . . . . . . . . . . . . . . . . 17
 16 . . . + . . . . . + . . . . . + . . . 16
 15 . . . . . . . . . . . . . . . . . . . 15
 14 . . . . . . . . . . . . . . . . . . . 14
 13 . . . . . . . . . . . . . . . . . . . 13
 12 . . . . . . . . . . . . . . . . . . . 12
 11 . . . . . . . . . . . . . . . . . . . 11\t    WHITE (0) has captured 0 stones
 10 . . . + . . . . . + . . . . . + . . . 10\t    BLACK (X) has captured 0 stones
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

    A B C D E F G H J K L M N
 13 . . . . . . . . . . . . . 13
 12 . . . . . . . . . . . . . 12
 11 . . . . . . . . . . . . . 11
 10 . . . + . . . . . + . . . 10
  9 . . . . . . . . . . . . . 9
  8 . . . . . . . . . . . . . 8
  7 . . . . . . + . . . . . . 7
  6 . . . . . . . . . . . . . 6
  5 . . . . . . . . . . . . . 5\t    WHITE (0) has captured 0 stones
  4 . . . + . . . . . + . . . 4\t    BLACK (X) has captured 0 stones
  3 . . . . . . . . . . . . . 3
  2 . . . . . . . . . . . . . 2
  1 . . . . . . . . . . . . . 1
    A B C D E F G H J K L M N
END_BOARD_13

my $board_9 = <<"END_BOARD_9";

    A B C D E F G H J
  9 . . . . . . . . . 9
  8 . . . . . . . . . 8
  7 . . + . . . + . . 7
  6 . . . . . . . . . 6
  5 . . . . + . . . . 5
  4 . . . . . . . . . 4
  3 . . + . . . + . . 3
  2 . . . . . . . . . 2\t    WHITE (0) has captured 0 stones
  1 . . . . . . . . . 1\t    BLACK (X) has captured 0 stones
    A B C D E F G H J
END_BOARD_9

my $board_min = <<"END_BOARD_MIN";

    A B
  2 . . 2\t    WHITE (0) has captured 0 stones
  1 . . 1\t    BLACK (X) has captured 0 stones
    A B
END_BOARD_MIN

my $board_max = <<"END_BOARD_MAX";

    A B C D E F G H J K L M N O P Q R S T U V
 21 . . . . . . . . . . . . . . . . . . . . . 21
 20 . . . . . . . . . . . . . . . . . . . . . 20
 19 . . . . . . . . . . . . . . . . . . . . . 19
 18 . . . . . . . . . . . . . . . . . . . . . 18
 17 . . . . . . . . . . . . . . . . . . . . . 17
 16 . . . . . . . . . . . . . . . . . . . . . 16
 15 . . . . . . . . . . . . . . . . . . . . . 15
 14 . . . . . . . . . . . . . . . . . . . . . 14
 13 . . . . . . . . . . . . . . . . . . . . . 13\t    WHITE (0) has captured 0 stones
 12 . . . . . . . . . . . . . . . . . . . . . 12\t    BLACK (X) has captured 0 stones
 11 . . . . . . . . . . . . . . . . . . . . . 11
 10 . . . . . . . . . . . . . . . . . . . . . 10
  9 . . . . . . . . . . . . . . . . . . . . . 9
  8 . . . . . . . . . . . . . . . . . . . . . 8
  7 . . . . . . . . . . . . . . . . . . . . . 7
  6 . . . . . . . . . . . . . . . . . . . . . 6
  5 . . . . . . . . . . . . . . . . . . . . . 5
  4 . . . . . . . . . . . . . . . . . . . . . 4
  3 . . . . . . . . . . . . . . . . . . . . . 3
  2 . . . . . . . . . . . . . . . . . . . . . 2
  1 . . . . . . . . . . . . . . . . . . . . . 1
    A B C D E F G H J K L M N O P Q R S T U V
END_BOARD_MAX


chomp $board_19;
ok_command( 'showboard', $board_19 );
ok_command( 'boardsize 13' );
chomp $board_13;
ok_command( 'showboard', $board_13 );
ok_command( 'boardsize 9' );
chomp $board_9;
ok_command( 'showboard', $board_9 );
ok_command( 'boardsize 2' );
chomp $board_min;
ok_command( 'showboard', $board_min );
ok_command( 'boardsize 21' );
chomp $board_max;
ok_command( 'showboard', $board_max );


ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

