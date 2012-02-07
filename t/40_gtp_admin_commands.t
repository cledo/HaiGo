
use strict;
use warnings;
use Carp;

use lib qw(t/lib);

use IPC::Open3;

use Test::More tests => 8;

use TLib qw(get_output);

$|++;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

# showgroups is only for debugging!
my @known_commands = qw{
    protocol_version
    name
    version
    known_command
    list_commands
    quit
    boardsize
    clear_board
    komi
    play
    showboard
    genmove
    showgroups
};

print {$stdin} "protocol_version\n";
$output = get_output($stdout);
is( $output, "= 2\n\n", 'protocol_version returned 2' );

print {$stdin} "name\n";
$output = get_output($stdout);
is( $output, "= HaiGo\n\n", 'name returned haigo' );

print {$stdin} "version\n";
$output = get_output($stdout);
is( $output, "= 0.1\n\n", 'version returned 0.1' );

print {$stdin} "known_command xyz\n";
$output = get_output($stdout);
is( $output, "= false\n\n", 'known_command: xyz unknown' );

print {$stdin} "known_command name\n";
$output = get_output($stdout);
is( $output, "= true\n\n", 'known_command: name known' );

print {$stdin} "list_commands\n";
$output = q{};
my $expected = '= ';
foreach (@known_commands) {
    $expected .= $_ . "\n";
}
$expected .= "\n";
$output = get_output($stdout);
is( $output, $expected, 'list_commands: correct list' );



print {$stdin} "quit\n";
$output = get_output($stdout);
is( $output, "= \n\n", 'quit returned =' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

