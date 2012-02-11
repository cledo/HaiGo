
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 26;

use TLib qw( get_output );

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

print {$stdin} "play XYZ A1\n";
$output = get_output($stdout);
is( $output, "? invalid color\n\n", 'invalid color' );

print {$stdin} "play BLACK A99\n";
$output = get_output($stdout);
is( $output, "? invalid coordinate\n\n", 'invalid coordinate' );

print {$stdin} "play BLACK ?9\n";
$output = get_output($stdout);
is( $output, "? invalid coordinate\n\n", 'invalid coordinate' );

print {$stdin} "play BLACK I9\n";
$output = get_output($stdout);
is( $output, "? invalid coordinate\n\n", 'invalid coordinate' );

print {$stdin} "play BLACK i0\n";
$output = get_output($stdout);
is( $output, "? invalid coordinate\n\n", 'invalid coordinate' );

print {$stdin} "play BLACK A-1\n";
$output = get_output($stdout);
is( $output, "? invalid coordinate\n\n", 'invalid coordinate' );

print {$stdin} "play BLACK A1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE A1\n";
$output = get_output($stdout);
is( $output, "? illegal move\n\n", 'move is illegal' );

print {$stdin} "play WHITE A2\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK C1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE B1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

#
# Check ko:
#
print {$stdin} "boardsize 5\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'boardsize changed' );

print {$stdin} "play BLACK A2\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE D2\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK B1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE C1\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK B3\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE C3\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK C2\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play WHITE B2\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK C2\n";
$output = get_output($stdout);
is( $output, "? illegal move\n\n", 'ko is illegal' );

print {$stdin} "play WHITE PASS\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'white passed' );

print {$stdin} "play WHITE A3\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'move played' );

print {$stdin} "play BLACK A1\n";
$output = get_output($stdout);
is( $output, "? illegal move\n\n", 'illegal move' );

# print {$stdin} "showboard\n";
# note $output = get_output($stdout);
# is( $output, "= \n\n", 'move played' );



#
# Quit:
#
print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

