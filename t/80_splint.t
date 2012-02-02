
use strict;
use warnings;
use Carp;

use Test::More;

my @splint_args = qw(
    -weak
    -sizeofformalarray
    -warnposix
    -boolcompare
);

my $src_dir        = './src/';
my @all_c_files    = glob $src_dir . '*.c';
my $splint_command = 'splint ' . join q{ }, @splint_args;

plan tests => scalar @all_c_files;

my $return_value;

foreach my $c_file (@all_c_files) {
    note qx{$splint_command $c_file 2>/dev/null};
    $return_value = $? >> 8;
    ok( $return_value == 0, "splint returned 0 for $c_file" );
}


