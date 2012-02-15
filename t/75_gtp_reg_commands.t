
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use Test::More tests => 7;

use TLib qw( get_output ok_command get_pid );

my $pid = get_pid();

my $sgf_file = "testfile.sgf";
open my $fh, '>', $sgf_file
    or carp "cannot open testfile";
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

