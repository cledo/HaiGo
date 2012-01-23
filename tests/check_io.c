#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <check.h>
#include "../src/io.h"

START_TEST (test_drop_comment_1)
{
    char command[SIZE_INPUT_BUFFER] = "help";

    drop_comment(command);
    fail_unless( strcmp( command, "help" ) == 0
        , "no comment to drop" );

}
END_TEST
START_TEST (test_drop_comment_2)
{
    char command[SIZE_INPUT_BUFFER] = "help # comment";

    drop_comment(command);
    fail_unless( strcmp( command, "help " ) == 0
        , "comment dropped" );
}
END_TEST
START_TEST (test_drop_comment_3)
{
    char command[SIZE_INPUT_BUFFER] = " help # comment ";

    drop_comment(command);
    fail_unless( strcmp( command, " help " ) == 0
        , "comment dropped" );
}
END_TEST

START_TEST (test_trim_1)
{
    char command[SIZE_INPUT_BUFFER] = "help";

    trim(command);
    fail_unless( strcmp( command, "help" ) == 0
        , "nothing to trim" );
}
END_TEST
START_TEST (test_trim_2)
{
    char command[SIZE_INPUT_BUFFER] = "help  ";

    trim(command);
    fail_unless( strcmp( command, "help" ) == 0
        , "trimmed trailing whitespace" );
}
END_TEST

START_TEST (test_trim_3)
{
    char command[SIZE_INPUT_BUFFER] = "  help";

    trim(command);
    fail_unless( strcmp( command, "help" ) == 0
        , "trimmed leading whitespace" );
}
END_TEST
START_TEST (test_trim_4)
{
    char command[SIZE_INPUT_BUFFER] = "  help  ";

    trim(command);
    fail_unless( strcmp( command, "help" ) == 0
        , "trimmed leading and trailing whitespace" );
}
END_TEST
START_TEST (test_trim_5)
{
    char command[SIZE_INPUT_BUFFER] = "hello world";

    trim(command);
    fail_unless( strcmp( command, "hello world" ) == 0
        , "trimmed nothing" );
}
END_TEST
START_TEST (test_trim_6)
{
    char command[SIZE_INPUT_BUFFER] = "hello  world";

    trim(command);
    fail_unless( strcmp( command, "hello world" ) == 0
        , "trimmed inner whitespace" );
}
END_TEST
START_TEST (test_trim_7)
{
    char command[SIZE_INPUT_BUFFER] = "  hello  world  ";

    trim(command);
    fail_unless( strcmp( command, "hello world" ) == 0
        , "trimmed all whitespace" );
}
END_TEST
START_TEST (test_trim_8)
{
    char command[SIZE_INPUT_BUFFER] = "  hello  to    the whole       world  ";

    trim(command);
    fail_unless( strcmp( command, "hello to the whole world" ) == 0
        , "trimmed all whitespace 2" );
}
END_TEST
START_TEST (test_init_tokens_1)
{
    int ok = 1;
    int i;
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];

    init_tokens(tokens);
    for ( i = 0; i < MAX_TOKEN_COUNT; i++ ) {
        if ( tokens[i][0] != '\0' ) {
            ok = 0;
            break;
        }
    }
    fail_unless( ok == 1, "tokens all null" );
}
END_TEST

START_TEST (test_identify_tokens_1)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "1" );
    strcpy( tokens[1], "version" );
    strcpy( tokens[2], "\0" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == 1, "id is 1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 0, "no arguments" );
}
END_TEST

START_TEST (test_identify_tokens_2)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "version" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == -1, "id is -1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 0, "no arguments" );
}
END_TEST

START_TEST (test_identify_tokens_3)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "1" );
    strcpy( tokens[1], "version" );
    strcpy( tokens[2], "arg1" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == 1, "id is 1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 1, "one argument" );
    fail_unless( strcmp( command_data.gtp_argv[0], "arg1" ) == 0
        , "arg1 is arg1 (%s)", command_data.gtp_argv[0] );
}
END_TEST

START_TEST (test_identify_tokens_4)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "version" );
    strcpy( tokens[1], "arg1" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == -1, "id is -1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 1, "one argument" );
    fail_unless( strcmp( command_data.gtp_argv[0], "arg1" ) == 0
        , "arg1 is arg1 (%s)", command_data.gtp_argv[0] );
}
END_TEST

START_TEST (test_identify_tokens_5)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "1" );
    strcpy( tokens[1], "version" );
    strcpy( tokens[2], "arg1" );
    strcpy( tokens[3], "arg2" );
    strcpy( tokens[4], "arg3" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == 1, "id is 1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 3, "three arguments" );
    fail_unless( strcmp( command_data.gtp_argv[0], "arg1" ) == 0
        , "arg1 is arg1 (%s)", command_data.gtp_argv[0] );
    fail_unless( strcmp( command_data.gtp_argv[1], "arg2" ) == 0
        , "arg2 is arg2 (%s)", command_data.gtp_argv[1] );
    fail_unless( strcmp( command_data.gtp_argv[2], "arg3" ) == 0
        , "arg3 is arg3 (%s)", command_data.gtp_argv[2] );
}
END_TEST

START_TEST (test_identify_tokens_6)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "version" );
    strcpy( tokens[1], "arg1" );
    strcpy( tokens[2], "arg2" );
    strcpy( tokens[3], "arg3" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == -1, "id is -1 (%d)", command_data.id );
    fail_unless( strcmp( command_data.name, "version" ) == 0
        , "name is version (%s)", command_data.name );
    fail_unless( command_data.gtp_argc == 3, "three arguments" );
    fail_unless( strcmp( command_data.gtp_argv[0], "arg1" ) == 0
        , "arg1 is arg1 (%s)", command_data.gtp_argv[0] );
    fail_unless( strcmp( command_data.gtp_argv[1], "arg2" ) == 0
        , "arg2 is arg2 (%s)", command_data.gtp_argv[1] );
    fail_unless( strcmp( command_data.gtp_argv[2], "arg3" ) == 0
        , "arg3 is arg3 (%s)", command_data.gtp_argv[2] );
}
END_TEST

Suite * io_suite(void) {
    Suite *s = suite_create("Run");

    TCase *tc_drop_comment = tcase_create("drop_comment");
    TCase *tc_trim         = tcase_create("trim");
    TCase *tc_init_tokens  = tcase_create("init_tokens");
    TCase *tc_identify_tokens = tcase_create("identify_tokens");

    tcase_add_test( tc_drop_comment, test_drop_comment_1 );
    tcase_add_test( tc_drop_comment, test_drop_comment_2 );
    tcase_add_test( tc_drop_comment, test_drop_comment_3 );

    tcase_add_test( tc_trim, test_trim_1 );
    tcase_add_test( tc_trim, test_trim_2 );
    tcase_add_test( tc_trim, test_trim_3 );
    tcase_add_test( tc_trim, test_trim_4 );
    tcase_add_test( tc_trim, test_trim_5 );
    tcase_add_test( tc_trim, test_trim_6 );
    tcase_add_test( tc_trim, test_trim_7 );
    tcase_add_test( tc_trim, test_trim_8 );

    tcase_add_test( tc_init_tokens, test_init_tokens_1 );

    tcase_add_test( tc_identify_tokens, test_identify_tokens_1 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_2 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_3 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_4 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_5 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_6 );

    suite_add_tcase( s, tc_drop_comment );
    suite_add_tcase( s, tc_trim );
    suite_add_tcase( s, tc_init_tokens );
    suite_add_tcase( s, tc_identify_tokens );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s     = io_suite();
    SRunner *sr  = srunner_create(s);
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
