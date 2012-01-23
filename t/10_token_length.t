
use strict;
use warnings;
use Carp;

use lib qw(t/lib);

use IPC::Open3;

use Test::More tests => 6;

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

print {$stdin} $token1 x 10 . q{ } . $token2 . q{ } . $token3 . "\n";
$output = get_output($stdout);
is( $output, "? MAX_TOKEN_LENGTH exceeded\n\n", 'first token: length exceeded' );

print {$stdin} $token1 . q{ } . $token2 x 10 . q{ } . $token3 . "\n";
$output = get_output($stdout);
is( $output, "? MAX_TOKEN_LENGTH exceeded\n\n", 'second token: length exceeded' );

print {$stdin} $token1 . q{ } . $token2 . q{ } . $token3 x 10 . "\n";
$output = get_output($stdout);
is( $output, "? MAX_TOKEN_LENGTH exceeded\n\n", 'last token: length exceeded' );

print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )
