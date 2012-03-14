#!/usr/bin/env perl

use strict;
use warnings;
use Carp;

use IPC::Open3;
use Time::HiRes qw( gettimeofday tv_interval );

my ( $stdin, $stdout, $stderr );

#my @factor       = qw( 80 20 1 1 1 1 1 1 );
#my @factor       = qw( 50 50 0 0 0 0 0 0 );
#my @factor       = qw( 75 20 0 0 0 0 0 5 );
#my @factor       = qw( 10 0 0 0 0 0 0 0 );
my @factor       = qw( 0 10 0 0 0 0 0 0 );
my $test_count   = 0;
my $ok_count     = 0;
my $not_ok_count = 0;

my $start_time = [ gettimeofday() ];

my $pid = open3( $stdin, $stdout, $stderr
    , 'prove -v t/80_problems.t :: ' . join q{ }, @factor );

print 'Factors ' . join q{ }, @factor;
print "\n";
while ( my $line = <$stdout> ) {
    if ( $line =~ qr{^# File: (\S+) (\S+) (\S+)} ) {
        print "Result for file $1: $2 ($3s)\n";
        if ($2) {
            $ok_count++;
        }
        else {
            $not_ok_count++;
        }
        $test_count++;
    }
}

my $diff_time = tv_interval( $start_time );
$diff_time = sprintf "%.2f", $diff_time;

print "Tests: $test_count\n";
print "OK:    $ok_count\n";
print "Fail:  $not_ok_count\n";
print "Time:  $diff_time sec\n";
