
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 6;

use TLib qw( ok_command get_pid );

my $pid = get_pid();

my $token1 = 'token1';
my $token2 = 'token2';
my $token3 = 'token3';

ok_command( $token1 . q{ } . $token2 . q{ } . $token3
    , 'unknown command', 1 );
ok_command( $token1 x 10 . q{ } . $token2 . q{ } . $token3
    , 'MAX_TOKEN_LENGTH exceeded', 1 );
ok_command( $token1 . q{ } . $token2 x 10 . q{ } . $token3
    , 'MAX_TOKEN_LENGTH exceeded', 1 );
ok_command( $token1 . q{ } . $token2 . q{ } . $token3 x 10
    , 'MAX_TOKEN_LENGTH exceeded', 1 );

ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )

