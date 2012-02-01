
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 4;

use TLib qw( get_output );

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

print {$stdin} "play BLACK A1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE A1\n";
$output = get_output($stdout);
is( $output, "? illegal move\n\n", 'move is illegal' );

print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

