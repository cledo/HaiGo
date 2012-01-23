
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

my $version = "0.1";

print {$stdin} "1 version\n";
$output = get_output($stdout);
is( $output, "=1 $version\n\n", 'command id is 1' );

print {$stdin} "0 version\n";
$output = get_output($stdout);
is( $output, "=0 $version\n\n", 'command id is 0' );

print {$stdin} "-1 version\n";
$output = get_output($stdout);
is( $output, "= $version\n\n", 'command id is empty for -1' );

print {$stdin} "version\n";
$output = get_output($stdout);
is( $output, "= $version\n\n", 'command id not given' );

print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' )
