package TLib;

use strict;
use warnings;
use Carp;

use base qw(Exporter);
our @EXPORT_OK = qw(get_output);

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

