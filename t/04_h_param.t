
use strict;
use warnings;
use Carp;

use Test::More tests => 2;

my $output;
my $return_value;

note $output = qx{./src/haigo -h 2>&1};
$return_value = $? >> 8;
ok( $return_value == 0, 'haigo -h returned 0' );
ok( $output, 'haigo -h returned some ouput' );

