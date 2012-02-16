
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use Test::More tests => 7;

use TLib qw( get_output ok_command get_pid );

my $pid = get_pid();

my $sgf = <<"END_SGF";
(
;FF[4]GM[1]SZ[19] GN[Copyright goproblems.com] PB[Black] HA[0] PW[White] KM[5.5] DT[1999-07-21] TM[1800] RU[Japanese]
;AW[bb][cb][cc][cd][de][df][cg][ch][dh][ai][bi][ci]AB[ba][ab][ac][bc][bd][be][cf][bg][bh]C[Black to play and live.]
( ;B[af] ;W[ah] ( ;B[ce] ;W[ag]C[only one eye this way]) ( ;B[ag] ;W[ce])) ( ;B[ah] ;W[af] ( ;B[ae] ;W[bf] ;B[ag] ;W[bf]
( ;B[af] ;W[ce]C[oops! you can't take this stone]) ( ;B[ce] ;W[af] ;B[bg]C[RIGHT black plays under the stones and lives]))
( ;B[bf] ;W[ae])) ( ;B[ae] ;W[ag]))
END_SGF

my $sgf_file = "testfile.sgf";
open my $fh, '>', $sgf_file
    or carp "cannot open testfile";
print {$fh} $sgf;
close $fh
    or carp "cannot close testfile";

ok_command( 'loadsgf',              'missing argument: filename', 1 );
ok_command( 'loadsgf not_a_file',   'cannot load file',           1 );
ok_command( 'loadsgf ' . $sgf_file                                  );
ok_command( 'loadsgf ' . $sgf_file . ' 0'                           );
ok_command( 'loadsgf ' . $sgf_file . ' 1'                           );

ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit_status 0' );

unlink $sgf_file;

