
use strict;
use warnings;
use Carp;

use Test::More tests => 1;

my $return_value;

note qx{ make tags 2>&1 };
$return_value = $? >> 8;
ok( $return_value == 0, 'make tags returned 0' );


