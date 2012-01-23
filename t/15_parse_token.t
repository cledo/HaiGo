
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

my $token1 = 'token1';
my $token2 = 'token2';
my $token3 = 'token3';

print {$stdin} $token1 . q{ } . $token2 . q{ } . $token3 . "\n";
$output = get_output($stdout);
is( $output, "? unknown command\n\n", 'short tokens ok' );

print {$stdin} q{ } . $token1 . q{ } . $token2 . q{ } . $token3 . "\n";
$output = get_output($stdout);
is( $output, "? unknown command\n\n", 'leading whitespace dropped' );

print {$stdin} qq{\t} . $token1 . q{ } . $token2 . q{ } . $token3 . "\n";
$output = get_output($stdout);
is( $output, "? unknown command\n\n", 'leading tab dropped' );

print {$stdin} $token1 . q{ } . $token2 . q{ } . $token3 . " \n";
$output = get_output($stdout);
is( $output, "? unknown command\n\n", 'trailing whitespace dropped' );

print {$stdin} $token1 . q{ } . $token2 . q{ } . $token3 . "\t\n";
$output = get_output($stdout);
is( $output, "? unknown command\n\n", 'trailing tab dropped' );


print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )
