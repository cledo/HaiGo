
use strict;
use warnings;
use Carp;

use Test::More tests => 1 ;

my $return_value;

note qx{make clean};
$return_value = $? >> 8;
ok( $return_value == 0, 'make clean returned 0' );
