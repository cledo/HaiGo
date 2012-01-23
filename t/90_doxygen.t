
use strict;
use warnings;
use Carp;

use Test::More tests => 2;

my $return_value;
my $warnings_file = 'doxygen_warnings';

qx{doxygen Doxyfile};
$return_value = $? >> 8;
ok( $return_value == 0, 'doxygen returned no errors' );

ok( -z $warnings_file, 'doxygen returned no warnings' );

if ( -s $warnings_file > 0 ) {
    open my $fh, '<', $warnings_file
        or croak "Cannot open file $warnings_file";
    my @warnings = do { local $\ = undef; <$fh> };
    close $fh
        or croak "Cannot close file $warnings_file";

    note join '', @warnings;
}
