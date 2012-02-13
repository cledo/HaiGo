#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "../src/global_tools.h"


START_TEST (test_str_toupper_1)
{
    char test_string1[] = "HELLO, WORLD!";
    char test_string2[] = "hello, world!";

    str_toupper( test_string2 );

    fail_if( strcmp( test_string1, test_string2 ) != 0, "string converted to upper case" );
}
END_TEST

START_TEST (test_str_toupper_2)
{
    char test_string1[] = "a";
    char test_string2[] = "A";

    char test_char;

    for ( test_char = test_string1[0]; test_char <= 'z'; test_char++ ) {
        test_string1[0] = test_char;
        str_toupper( test_string1 );
        fail_if( strcmp( test_string1, test_string2 ) != 0, "letter converted to upper case" );

        test_string2[0]++;
    }

}
END_TEST

START_TEST (test_my_strcpy_1)
{
    char test_string1[] = "looooooooong striiiiiiiing";
    char test_string2[] = "short string";

    // This copy should fail and exit with 1:
    my_strcpy( test_string2, test_string1, strlen(test_string2) );

}
END_TEST

START_TEST (test_my_strcpy_2)
{
    char test_string1[] = "looooooooong striiiiiiiing";
    char test_string2[] = "short string";

    my_strcpy( test_string1, test_string2, strlen(test_string1) );
    fail_if( strcmp( test_string1, test_string2 ) != 0, "string copied correctly" );

}
END_TEST


Suite *
global_tools_suite(void)
{
    Suite *s = suite_create("Global_Tools");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test( tc_core, test_str_toupper_1 );
    tcase_add_test( tc_core, test_str_toupper_2 );

    tcase_add_exit_test( tc_core, test_my_strcpy_1, 1 );
    tcase_add_test( tc_core, test_my_strcpy_2 );

    suite_add_tcase( s, tc_core );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s    = global_tools_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
