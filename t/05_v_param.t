
use strict;
use warnings;
use Carp;

use Test::More tests => 2;

my $output;
my $return_value;

note $output = qx{./src/haigo -v 2>&1};
$return_value = $? >> 8;
ok( $return_value == 0, 'haigo -v returned 0' );
is( $output, "haigo 0.1\n", 'haigo -v returned name and version' );

