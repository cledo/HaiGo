
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use IPC::Open3;

use Test::More tests => 10;

use TLib qw( get_output ok_command get_pid );

my $pid = get_pid();

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
    fixed_handicap
    level
    play
    showboard
    genmove
    undo
    loadsgf
    showgroups
};

ok_command( 'protocol_version', '2' );
ok_command( 'name', 'HaiGo' );
ok_command( 'version', '0.1' );
ok_command( 'known_command', 'missing argument: command_name', 1 );
ok_command( 'known_command name name'
    , 'only one argument required: command_name', 1 );

ok_command( 'known_command xyz', 'false' );
ok_command( 'known_command name', 'true' );

my $expected = q{};
foreach (@known_commands) {
    $expected .= $_ . "\n";
}
chomp $expected;
ok_command( 'list_commands', $expected );

ok_command( 'quit' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit status 0' );

