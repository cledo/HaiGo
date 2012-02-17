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
    my_strcpy( command_data.name, "boardsize", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "0", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "boardsize with invalid size" );
}
END_TEST

START_TEST (test_select_command_6)
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

START_TEST (test_select_command_7)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "clear_board", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "clear_board command identified" );
}
END_TEST

START_TEST (test_select_command_8)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "list_commands", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "list_commands command identified" );
}
END_TEST

START_TEST (test_select_command_9)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "genmove", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "BLACK", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_board(BOARD_SIZE_DEFAULT);
    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "clear_board command identified" );
}
END_TEST

START_TEST (test_select_command_10)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "known_command", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "name", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "known_command command identified" );

}
END_TEST

START_TEST (test_select_command_11)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "known_command", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "known_command without arg is error" );

}
END_TEST

START_TEST (test_select_command_12)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "known_command", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "name", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "name", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "name", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 3;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "known_command with too many args is error" );

}
END_TEST

START_TEST (test_select_command_13)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "known_command", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "not_implemented", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "known_command with unknown command" );

}
END_TEST

START_TEST (test_select_command_14)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "protocol_version", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "protocol_version command identified" );

}
END_TEST

START_TEST (test_select_command_15)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "play", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "black", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[1], "d4", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 2;

    init_board(BOARD_SIZE_DEFAULT);
    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "play command identified" );

}
END_TEST

START_TEST (test_select_command_16)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "showboard", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_board(BOARD_SIZE_DEFAULT);
    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "showboard command identified" );

}
END_TEST

START_TEST (test_select_command_17)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "quit", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "quit command identified" );

}
END_TEST

START_TEST (test_select_command_18)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "loadsgf", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 0;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "loadsgf without arg filename" );

}
END_TEST

START_TEST (test_select_command_19)
{
    struct command command_data;

    command_data.id = 0;
    my_strcpy( command_data.name, "loadsgf", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], "not_a_file", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "loadsgf with invalid filename" );

}
END_TEST

START_TEST (test_select_command_20)
{
    struct command command_data;
    char filename[] = "testfile.sgf";
    FILE *sgf_file;

    sgf_file = fopen( filename, "w" );

    command_data.id = 0;
    my_strcpy( command_data.name, "loadsgf", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], filename, MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 1;
    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == true, "loadsgf with zero size file" );

    fclose(sgf_file);
    remove(filename);
}
END_TEST

START_TEST (test_select_command_21)
{
    struct command command_data;
    //char filename[] = "testfile.sgf";
    char filename[] = "game1.sgf";
    FILE *sgf_file;

    //sgf_file = fopen( filename, "w" );

    command_data.id = 0;
    my_strcpy( command_data.name, "loadsgf", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], filename, MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[1], "0", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 2;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "loadsgf with valid filename, invalid move_number" );

    //fclose(sgf_file);
    //remove(filename);
}
END_TEST

START_TEST (test_select_command_22)
{
    struct command command_data;
    //char filename[] = "testfile.sgf";
    char filename[] = "game1.sgf";
    FILE *sgf_file;

    //sgf_file = fopen( filename, "w" );

    command_data.id = 0;
    my_strcpy( command_data.name, "loadsgf", MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[0], filename, MAX_TOKEN_LENGTH );
    my_strcpy( command_data.gtp_argv[1], "10", MAX_TOKEN_LENGTH );
    command_data.gtp_argc = 2;

    init_known_commands();
    select_command(&command_data);
    fail_unless( get_output_error() == false, "loadsgf with valid filename, valid move_number" );

    //fclose(sgf_file);
    //remove(filename);
}
END_TEST

Suite * run_program_suite(void) {
    Suite *s = suite_create("Run");

    TCase *tc_core = tcase_create("Core");
    TCase *tc_gtp  = tcase_create("GTP");

    //tcase_add_test( tc_core, run_no_param );
    tcase_add_exit_test( tc_core, run_h_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_v_param, EXIT_SUCCESS );
    tcase_add_exit_test( tc_core, run_invalid_param, EXIT_FAILURE );

    tcase_add_test( tc_gtp, test_select_command_1  );
    tcase_add_test( tc_gtp, test_select_command_2  );
    tcase_add_test( tc_gtp, test_select_command_3  );
    tcase_add_test( tc_gtp, test_select_command_4  );
    tcase_add_test( tc_gtp, test_select_command_5  );
    tcase_add_test( tc_gtp, test_select_command_6  );
    tcase_add_test( tc_gtp, test_select_command_7  );
    tcase_add_test( tc_gtp, test_select_command_8  );
    tcase_add_test( tc_gtp, test_select_command_9  );
    tcase_add_test( tc_gtp, test_select_command_10 );
    tcase_add_test( tc_gtp, test_select_command_11 );
    tcase_add_test( tc_gtp, test_select_command_12 );
    tcase_add_test( tc_gtp, test_select_command_13 );
    tcase_add_test( tc_gtp, test_select_command_14 );
    tcase_add_test( tc_gtp, test_select_command_15 );
    tcase_add_test( tc_gtp, test_select_command_16 );
    tcase_add_test( tc_gtp, test_select_command_17 );
    tcase_add_test( tc_gtp, test_select_command_18 );
    tcase_add_test( tc_gtp, test_select_command_19 );
    tcase_add_test( tc_gtp, test_select_command_20 );
    tcase_add_test( tc_gtp, test_select_command_21 );
    tcase_add_test( tc_gtp, test_select_command_22 );

    suite_add_tcase( s, tc_core );
    suite_add_tcase( s, tc_gtp  );

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
