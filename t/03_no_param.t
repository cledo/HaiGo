
use strict;
use warnings;
use Carp;

use IPC::Open3;

use Test::More tests => 3;

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

print {$stdin} "hello\n";
$output = <$stdout> . <$stdout>;
is( $output, "? unknown command\n\n", 'hello returned ? unknown command' );

print {$stdin} "quit\n";
$output = <$stdout> . <$stdout>;
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )
