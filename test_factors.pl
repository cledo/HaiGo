#!/usr/bin/env perl

use strict;
use warnings;
use Carp;

use lib qw( SC_Module );

use IPC::Open3;
use Time::HiRes qw( gettimeofday tv_interval );

use SC;

my $table = 'table.sc';
my $sc    = SC->new( { FILE => $table } );

my ( $stdin, $stdout, $stderr );

#my @factor       = qw( 80 20 1 1 1 1 1 1 );
my @factor       = qw( 800 200 100 1 1 1 3 1 );
#my @factor       = qw( 800 200 100 1 10 1 3 1 );
#my @factor       = qw( 50 50 0 0 0 0 0 0 );
#my @factor       = qw( 75 20 0 0 0 0 0 5 );
#my @factor       = qw( 100 0 0 0 0 0 0 0 );
#my @factor       = qw( 100 50 0 0 0 0 0 0 );
#my @factor       = qw( 100 50 25 5 10 2 1 1 );
my $count        = 0;
my $ok_count     = 0;
my $not_ok_count = 0;

my $start_time = [ gettimeofday() ];

my $pid = open3( $stdin, $stdout, $stderr
    , 'prove -v t/80_problems.t :: ' . join q{ }, @factor );

my ($row) = $sc->get_empty_cell('A') =~ qr{^[A-Z]+(\d+)$};

print 'Factors ' . join q{ }, @factor;
print "\n";
while ( my $line = <$stdout> ) {
    if ( $line =~ qr{^# File: (\S+) (\S+) (\S+)} ) {
        print "File $1: $2 ($3s)\n";
        my $column = $sc->get_column_name( $count + 5 );
        $sc->set_cell( $column . $row, " $2 ($3)", 'leftstring' );
        $sc->set_format( $column, '21 2 0' );
        $sc->set_cell( $column . '0' , $1, 'label' );

        $count++;
        if ($2) {
            $ok_count++;
        }
        else {
            $not_ok_count++;
        }
    }
}

my $diff_time = tv_interval( $start_time );
$diff_time = sprintf "%.2f", $diff_time;

print "Tests: $count\n";
print "OK:    $ok_count\n";
print "Fail:  $not_ok_count\n";
print "Time:  $diff_time sec\n";

$sc->set_format( 'A', '25 0 0' );
$sc->set_format( 'B', '7 0 0'  );
$sc->set_format( 'C', '7 0 0'  );
$sc->set_format( 'D', '7 0 0'  );
$sc->set_format( 'E', '7 2 0'  );

#$sc->set_cell( 'A0', 'Factors',     'label' );
$sc->set_cell( 'A' . $row, "@factor",     'leftstring' );
#$sc->set_cell( 'B0', 'Tests',       'label' );
$sc->set_cell( 'B' . $row, $count,        'label' );
#$sc->set_cell( 'C0', 'OK',          'label' );
$sc->set_cell( 'C' . $row, $ok_count,     'label' );
#$sc->set_cell( 'D0', 'Fail',        'label' );
$sc->set_cell( 'D' . $row, $not_ok_count, 'label' );
#$sc->set_cell( 'E0', 'Time',        'label' );
$sc->set_cell( 'E' . $row, $diff_time,    'rightstring' );

$sc->write_file();

