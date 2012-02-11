package TLib;

use strict;
use warnings;
use Carp;

use IPC::Open3;
use Test::Builder;

use base qw(Exporter);
our @EXPORT_OK = qw( get_output ok_command get_pid );

my $test = Test::Builder->new;

my ( $stdin, $stdout, $stderr );
my $output;

my $pid = open3( $stdin, $stdout, $stderr, './src/haigo' );

sub get_pid {
    return $pid;
}

sub ok_command {
    my ( $command, $return, $fail ) = @_;

    my $ok = ($fail) ? 'incorrect' : 'ok';

    $fail = ($fail) ? '? ' : '= ';
    if ( not defined $return ) {
        $return = q{};
    }

    my $name = $command . ': ' . $ok;

    print {$stdin} $command . "\n";
    $output = get_output($stdout);
    $test->is_eq( $output, $fail . $return . "\n\n", $name );

    #print {$stdin} "showboard\n";
    #$test->note( get_output($stdout) );

    return;
}

sub get_output {
    my ($pipe) = @_;

    my $output   = q{};
    my $line     = q{};
    my $time_out = 2;

    eval {
        local $SIG{'ALRM'} = sub { die "alarm\n" };
        alarm $time_out;
        while (1) {
            $line    = <$pipe>;
            last if not defined $line;
            $output .= $line;
            last if $line eq qq{\n};
        }
        alarm 0;
        1;
    };

    return $output;
}

1;

