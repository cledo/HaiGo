package TLib;

use strict;
use warnings;
use Carp;

use IPC::Open3;
use Test::Builder;

use base qw(Exporter);
our @EXPORT_OK = qw(
    get_output
    ok_command
    get_pid
    get_timeout
    set_timeout
);

my $test = Test::Builder->new;

my ( $stdin, $stdout, $stderr );
my $output;
my $time_out = 2;

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

    my $name;
    if ( length $command > 30 ) {
        $name = substr( $command, 0, 25 ) . '... : ' . $ok;
    }
    else {
        $name = $command . ': ' . $ok;
    }

    print {$stdin} $command . "\n";
    $output = get_output($stdout);
    my $is_ok = $test->is_eq( $output, $fail . $return . "\n\n", $name );

    return $is_ok;
}

sub get_output {
    my ($pipe) = @_;

    my $output   = q{};
    my $line     = q{};

    eval {
        local $SIG{'ALRM'} = sub { die "alarm\n" };
        alarm $time_out;
        while (1) {
            $line    = <$pipe>;
            next if $line =~ m{^#};
            last if not defined $line;
            $output .= $line;
            last if $line eq qq{\n};
        }
        alarm 0;
        1;
    };

    return $output;
}

sub get_timeout {
    return $time_out;
}

sub set_timeout {
    ($time_out) = @_;

    return;
}

1;

