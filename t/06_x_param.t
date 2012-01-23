
use strict;
use warnings;
use Carp;

use Test::More tests => 2;

my $output;
my $return_value;

note $output = qx{./src/haigo -x 2>&1};
$return_value = $? >> 8;
ok( $return_value != 0, 'haigo -x returned not 0' );
is( $output, "./src/haigo: invalid option -- 'x'\n"
    , 'haigo -x returned invalid option' );


