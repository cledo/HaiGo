
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 3;

use TLib qw( ok_command get_pid );


my $pid = get_pid();

ok_command( 'hello', 'unknown command', 1 );
ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )
