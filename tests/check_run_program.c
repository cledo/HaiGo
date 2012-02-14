#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/global_const.h"
#include "../src/run_program.h"
#include "../src/io.h"
#include "../src/global_tools.h"


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

    fail_unless( run( argc, argv ) == EXIT_SUCCESS
        , "run() with parameter -h returned EXIT_SUCCESS" );
}
END_TEST

START_TEST (run_v_param)
{
    int argc = 2;
    char *argv[] = { "program_name", "-v", NULL };

    fail_unless( run( argc, argv ) == EXIT_SUCCESS
        , "run() with parameter -v returned EXIT_SUCCESS" );
}
END_TEST

START_TEST (run_invalid_param)
{
    int argc = 2;
    char *argv[] = { "program_name", "-x", NULL };

    fail_unless( run( argc, argv ) == EXIT_FAILURE
        , "run() with invalid parameter -x returned EXIT_FAILURE" );
}
END_TEST

START_TEST (test_select_command_1)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "invalid", MAX_TOKEN_LENGTH );

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "invalid command identified" );
}
END_TEST


START_TEST (test_select_command_2)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "version", MAX_TOKEN_LENGTH );

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "version command identified" );
}
END_TEST

START_TEST (test_select_command_3)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "name", MAX_TOKEN_LENGTH );

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "name command identified" );
}
END_TEST

START_TEST (test_select_command_4)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "boardsize", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "2", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "boardsize command identified" );
}
END_TEST

START_TEST (test_select_command_5)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "komi", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "6.5", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "komi command identified" );
}
END_TEST

Suite * run_program_suite(void) {
    Suite *s = suite_create("Run");

    TCase *tc_core        = tcase_create("Core");

    //tcase_add_test( tc_core, run_no_param );
    tcase_add_exit_test( tc_core, run_h_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_v_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_invalid_param, EXIT_FAILURE );

    tcase_add_test( tc_core, test_select_command_1 );
    tcase_add_test( tc_core, test_select_command_2 );
    tcase_add_test( tc_core, test_select_command_3 );
    tcase_add_test( tc_core, test_select_command_4 );
    tcase_add_test( tc_core, test_select_command_5 );

    suite_add_tcase( s, tc_core );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s    = run_program_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
