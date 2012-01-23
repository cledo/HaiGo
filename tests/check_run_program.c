#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/run_program.h"

/*
START_TEST (run_no_param)
{
    int argc = 1;
    char *argv[] = { "program_name", NULL };

    fail_unless( run( argc, argv )
        == EXIT_SUCCESS, "run() without parameter returned EXIT_SUCCESS" );
}
END_TEST
*/

START_TEST (run_h_param)
{
    int argc = 2;
    char *argv[] = { "program_name", "-h", NULL };

    fail_unless( run( argc, argv )
        == EXIT_SUCCESS, "run() with parameter -h returned EXIT_SUCCESS" );
}
END_TEST

START_TEST (run_v_param)
{
    int argc = 2;
    char *argv[] = { "program_name", "-v", NULL };

    fail_unless( run( argc, argv )
        == EXIT_SUCCESS, "run() with parameter -v returned EXIT_SUCCESS" );
}
END_TEST

START_TEST (run_invalid_param)
{
    int argc = 2;
    char *argv[] = { "program_name", "-x", NULL };

    fail_unless( run( argc, argv )
        == EXIT_FAILURE, "run() with invalid parameter -x returned EXIT_FAILURE" );
}
END_TEST

Suite * run_program_suite(void) {
    Suite *s = suite_create("Run");

    /* Core test case */
    TCase *tc_core = tcase_create("Core");
    //tcase_add_test( tc_core, run_no_param );
    tcase_add_exit_test( tc_core, run_h_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_v_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_invalid_param, EXIT_FAILURE );
    suite_add_tcase( s, tc_core );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s    = run_program_suite();
    SRunner *sr = srunner_create(s);

    //srunner_run_all( sr, CK_NORMAL );
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
