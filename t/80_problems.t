
use strict;
use warnings;
use Carp;

use lib qw( t/lib );

use Time::HiRes qw( gettimeofday tv_interval );
use Test::More;

use TLib qw( get_output ok_command get_pid set_timeout );

my $problems_table = './t/problems/problems.asc';
open my $fh, q{<}, $problems_table
    or croak "Cannot open file $problems_table";
my @problem;
while (<$fh>) {
    next if m{File\s+Player\s+Solution\s+Moves};
    next if m{^\s*$};
    next if m{^\s*#$};
    push @problem, [ ( split qr{\s+}, $_ )[1,2,3,4] ];
}
close $fh
    or croak "Cannot close file $problems_table";


my $pid = get_pid();

plan tests => ( scalar @problem * 2 ) + 6;

set_timeout(300);

ok_command( 'level 0' );

foreach my $p (@problem) {
    my $file     = $p->[0];
    my $player   = $p->[1];
    my $solution = $p->[2];
    my $moves    = $p->[3] || undef;

    ok_command( "loadsgf $file" );

    if ( $moves ) {
        my @moves = parse_moves($moves);
        foreach my $move (@moves) {
            ok_command( "play $move" );
        }
    }

    my $start_time = [ gettimeofday() ];
    ok_command( "genmove $player", $solution );

    my $diff_time = tv_interval( $start_time );
    note sprintf "%s: %.2fs", $solution, $diff_time;
}

ok_command( 'quit ' );

waitpid( $pid, 0 );
my $exit_status = $? >> 8;

ok( $exit_status == 0, 'exit_status 0' );


sub parse_moves {
    my ($moves) = @_;

    my @moves = map {
        $_ =~ q{^([wb])([A-Z]\d+)$};
        $1 . ' ' . $2;
    } ( split qr{,}, $moves );

    return @moves;
}

