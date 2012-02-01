#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <check.h>
#include "../src/global_const.h"
#include "../src/board.h"

int  **board = NULL;
int  **group = NULL;
bool **hoshi = NULL;

static const int board_count  = 5;
static const int board_size[] = {
    BOARD_SIZE_MIN,
    9,
    13,
    19,
    BOARD_SIZE_MAX
};

static char *test_board[] = { "\n"
"    A B\n"
"  2 . . 2\n"
"  1 . . 1\n"
"    A B",
"\n"
"    A B C D E F G H J\n"
"  9 . . . . . . . . . 9\n"
"  8 . . . . . . . . . 8\n"
"  7 . . + . . . + . . 7\n"
"  6 . . . . . . . . . 6\n"
"  5 . . . . + . . . . 5\n"
"  4 . . . . . . . . . 4\n"
"  3 . . + . . . + . . 3\n"
"  2 . . . . . . . . . 2\n"
"  1 . . . . . . . . . 1\n"
"    A B C D E F G H J",
"\n"
"    A B C D E F G H J K L M N\n"
" 13 . . . . . . . . . . . . . 13\n"
" 12 . . . . . . . . . . . . . 12\n"
" 11 . . . . . . . . . . . . . 11\n"
" 10 . . . + . . . . . + . . . 10\n"
"  9 . . . . . . . . . . . . . 9\n"
"  8 . . . . . . . . . . . . . 8\n"
"  7 . . . . . . + . . . . . . 7\n"
"  6 . . . . . . . . . . . . . 6\n"
"  5 . . . . . . . . . . . . . 5\n"
"  4 . . . + . . . . . + . . . 4\n"
"  3 . . . . . . . . . . . . . 3\n"
"  2 . . . . . . . . . . . . . 2\n"
"  1 . . . . . . . . . . . . . 1\n"
"    A B C D E F G H J K L M N",
"\n"
"    A B C D E F G H J K L M N O P Q R S T\n"
" 19 . . . . . . . . . . . . . . . . . . . 19\n"
" 18 . . . . . . . . . . . . . . . . . . . 18\n"
" 17 . . . . . . . . . . . . . . . . . . . 17\n"
" 16 . . . + . . . . . + . . . . . + . . . 16\n"
" 15 . . . . . . . . . . . . . . . . . . . 15\n"
" 14 . . . . . . . . . . . . . . . . . . . 14\n"
" 13 . . . . . . . . . . . . . . . . . . . 13\n"
" 12 . . . . . . . . . . . . . . . . . . . 12\n"
" 11 . . . . . . . . . . . . . . . . . . . 11\n"
" 10 . . . + . . . . . + . . . . . + . . . 10\n"
"  9 . . . . . . . . . . . . . . . . . . . 9\n"
"  8 . . . . . . . . . . . . . . . . . . . 8\n"
"  7 . . . . . . . . . . . . . . . . . . . 7\n"
"  6 . . . . . . . . . . . . . . . . . . . 6\n"
"  5 . . . . . . . . . . . . . . . . . . . 5\n"
"  4 . . . + . . . . . + . . . . . + . . . 4\n"
"  3 . . . . . . . . . . . . . . . . . . . 3\n"
"  2 . . . . . . . . . . . . . . . . . . . 2\n"
"  1 . . . . . . . . . . . . . . . . . . . 1\n"
"    A B C D E F G H J K L M N O P Q R S T",
"\n"
"    A B C D E F G H J K L M N O P Q R S T U V W X Y Z\n"
" 25 . . . . . . . . . . . . . . . . . . . . . . . . . 25\n"
" 24 . . . . . . . . . . . . . . . . . . . . . . . . . 24\n"
" 23 . . . . . . . . . . . . . . . . . . . . . . . . . 23\n"
" 22 . . . . . . . . . . . . . . . . . . . . . . . . . 22\n"
" 21 . . . . . . . . . . . . . . . . . . . . . . . . . 21\n"
" 20 . . . . . . . . . . . . . . . . . . . . . . . . . 20\n"
" 19 . . . . . . . . . . . . . . . . . . . . . . . . . 19\n"
" 18 . . . . . . . . . . . . . . . . . . . . . . . . . 18\n"
" 17 . . . . . . . . . . . . . . . . . . . . . . . . . 17\n"
" 16 . . . . . . . . . . . . . . . . . . . . . . . . . 16\n"
" 15 . . . . . . . . . . . . . . . . . . . . . . . . . 15\n"
" 14 . . . . . . . . . . . . . . . . . . . . . . . . . 14\n"
" 13 . . . . . . . . . . . . . . . . . . . . . . . . . 13\n"
" 12 . . . . . . . . . . . . . . . . . . . . . . . . . 12\n"
" 11 . . . . . . . . . . . . . . . . . . . . . . . . . 11\n"
" 10 . . . . . . . . . . . . . . . . . . . . . . . . . 10\n"
"  9 . . . . . . . . . . . . . . . . . . . . . . . . . 9\n"
"  8 . . . . . . . . . . . . . . . . . . . . . . . . . 8\n"
"  7 . . . . . . . . . . . . . . . . . . . . . . . . . 7\n"
"  6 . . . . . . . . . . . . . . . . . . . . . . . . . 6\n"
"  5 . . . . . . . . . . . . . . . . . . . . . . . . . 5\n"
"  4 . . . . . . . . . . . . . . . . . . . . . . . . . 4\n"
"  3 . . . . . . . . . . . . . . . . . . . . . . . . . 3\n"
"  2 . . . . . . . . . . . . . . . . . . . . . . . . . 2\n"
"  1 . . . . . . . . . . . . . . . . . . . . . . . . . 1\n"
"    A B C D E F G H J K L M N O P Q R S T U V W X Y Z"
};

START_TEST (test_init_board_1)
{
    int s = board_size[_i];

    init_board(s);

    fail_if( board == NULL, "board initialised (%dx%d)", s, s );
    fail_if( group == NULL, "group initialised (%dx%d)", s, s );
    fail_if( hoshi == NULL, "hoshi initialised (%dx%d)", s, s );

    free_board();

    fail_if( board != NULL, "board is NULL" );
    fail_if( group != NULL, "group is NULL" );
    fail_if( hoshi != NULL, "hoshi is NULL" );
}
END_TEST

START_TEST (test_get_board_as_string_1)
{
    char board_output[MAX_OUTPUT_LENGTH];
    //FILE *f1, *f2;

    int s = board_size[_i];

    init_board(s);
    get_board_as_string(board_output);

    /*
    f1 = fopen( "f1.txt", "w" );
    f2 = fopen( "f2.txt", "w" );
    fprintf( f1, board_min );
    fprintf( f2, board_output );
    fclose(f1);
    fclose(f2);
    */
    fail_if( strcmp( test_board[_i], board_output ) != 0, "board string received (%dx%d)", s, s );
}
END_TEST


Suite * board_suite(void) {
    Suite *s                      = suite_create("Run");
    TCase *tc_init_board          = tcase_create("init_board");
    TCase *tc_get_board_as_string = tcase_create("get_board_as_string");

    tcase_add_loop_test( tc_init_board, test_init_board_1, 0, board_count );
    tcase_add_loop_test( tc_get_board_as_string, test_get_board_as_string_1, 0, board_count );

    suite_add_tcase( s, tc_init_board );
    suite_add_tcase( s, tc_get_board_as_string );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s = board_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

