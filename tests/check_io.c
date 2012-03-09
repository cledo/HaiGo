#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <check.h>
#include "../src/io.h"
#include "../src/global_tools.h"


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

START_TEST (test_identify_tokens_7)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "-1" );
    strcpy( tokens[1], "version" );
    strcpy( tokens[2], "arg1" );
    strcpy( tokens[3], "arg2" );
    strcpy( tokens[4], "arg3" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == INVALID, "id is INVALID (%d)", command_data.id );
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

START_TEST (test_identify_tokens_8)
{
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];
    struct command command_data;

    init_tokens(tokens);
    strcpy( tokens[0], "0" );
    strcpy( tokens[1], "version" );
    strcpy( tokens[2], "arg1" );
    strcpy( tokens[3], "arg2" );
    strcpy( tokens[4], "arg3" );

    identify_tokens( tokens, &command_data );

    fail_unless( command_data.id == 0, "id is 0 (%d)", command_data.id );
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

START_TEST (test_add_output_1)
{
    char to_output[] = "to_output";

    add_output(to_output);

}
END_TEST

START_TEST (test_add_output_2)
{
    int  k;
    char to_output[MAX_OUTPUT_LENGTH+1];

    for ( k = 0; k < MAX_OUTPUT_LENGTH+1; k++ ) {
        to_output[k] = 'X';
    }
    k--;
    to_output[k] = '\0';

    // This should fail:
    add_output(to_output);

}
END_TEST

START_TEST (test_print_output)
{
    int command_id = 1;

    print_output(-1);

    add_output("Hello, world!");
    print_output(command_id);

    mark_point();

    set_output_error();
    command_id = -1;
    add_output("Hello, world!");
    print_output(command_id);

    // Only exit value 0 can be tested here ...
}
END_TEST

START_TEST (test_output_error)
{
    bool is_error;

    is_error = get_output_error();
    fail_unless( is_error == false, "output error set to false" );

    set_output_error();
    is_error = get_output_error();
    fail_unless( is_error == true, "output error set to true" );

}
END_TEST

START_TEST (test_input)
{

    fail_unless( is_input_empty() == false, "input empty flag is false" );
}
END_TEST

START_TEST (test_parse_gtp_input_1)
{
    char command_input_buffer[SIZE_INPUT_BUFFER];
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];

    command_input_buffer[0] = '\0';

    parse_gtp_input( command_input_buffer, tokens );
    fail_unless( strlen( tokens[0] ) == 0, "tokens list empty" );

    my_strcpy( command_input_buffer, "token1 token2 token3 token4", SIZE_INPUT_BUFFER );
    parse_gtp_input( command_input_buffer, tokens );
    fail_unless( strcmp( tokens[0], "token1" ) == 0, "first token correct"  );
    fail_unless( strcmp( tokens[1], "token2" ) == 0, "second token correct" );
    fail_unless( strcmp( tokens[2], "token3" ) == 0, "third token correct"  );
    fail_unless( strcmp( tokens[3], "token4" ) == 0, "fourth token correct" );
}
END_TEST

START_TEST (test_parse_gtp_input_2)
{
    char command_input_buffer[SIZE_INPUT_BUFFER];
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];

    my_strcpy( command_input_buffer, "abcdefghijklmnopqrstuvwxyz", SIZE_INPUT_BUFFER );
    parse_gtp_input( command_input_buffer, tokens );

    parse_gtp_input( command_input_buffer, tokens );
    fail_unless( strlen( tokens[0] ) == 0, "tokens list empty" );
    fail_unless( get_output_error() == true, "output is error" );
}
END_TEST

START_TEST (test_parse_gtp_input_3)
{
    char command_input_buffer[SIZE_INPUT_BUFFER];
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];

    my_strcpy( command_input_buffer, "a b c d e f g h i j k l m n o p q r s t u v w x y z", SIZE_INPUT_BUFFER );
    parse_gtp_input( command_input_buffer, tokens );

    parse_gtp_input( command_input_buffer, tokens );
    fail_unless( strlen( tokens[0] ) == 0, "tokens list empty" );
    fail_unless( get_output_error() == true, "output is error" );
}
END_TEST


Suite * io_suite(void) {
    Suite *s = suite_create("IO");

    TCase *tc_drop_comment    = tcase_create("drop_comment");
    TCase *tc_trim            = tcase_create("trim");
    TCase *tc_init_tokens     = tcase_create("init_tokens");
    TCase *tc_identify_tokens = tcase_create("identify_tokens");
    TCase *tc_output          = tcase_create("output");
    TCase *tc_input           = tcase_create("input");
    TCase *tc_parse           = tcase_create("parse_gtp");

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
    tcase_add_test( tc_identify_tokens, test_identify_tokens_7 );
    tcase_add_test( tc_identify_tokens, test_identify_tokens_8 );

    tcase_add_test( tc_output, test_add_output_1 );
    tcase_add_exit_test( tc_output, test_add_output_2, 1 );
    tcase_add_exit_test( tc_output, test_print_output, 0 );
    tcase_add_test( tc_output, test_output_error );

    tcase_add_test( tc_input, test_input );

    tcase_add_test( tc_parse, test_parse_gtp_input_1 );
    tcase_add_test( tc_parse, test_parse_gtp_input_2 );
    tcase_add_test( tc_parse, test_parse_gtp_input_3 );

    suite_add_tcase( s, tc_drop_comment    );
    suite_add_tcase( s, tc_trim            );
    suite_add_tcase( s, tc_init_tokens     );
    suite_add_tcase( s, tc_identify_tokens );
    suite_add_tcase( s, tc_output          );
    suite_add_tcase( s, tc_input           );
    suite_add_tcase( s, tc_parse           );


    return s;
}

int main(void) {
    int number_failed;

    Suite *s     = io_suite();
    SRunner *sr  = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
