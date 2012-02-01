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

START_TEST (test_init_board_1)
{
    int i = -1;
    int s = 0;
    int board_size[] = { BOARD_SIZE_MIN, 9, 13, 19, BOARD_SIZE_MAX };

    do {
        i++;
        s = board_size[i];

        init_board(s);

        fail_if( board == NULL, "board initialised (%dx%d)", s, s );
        fail_if( group == NULL, "group initialised (%dx%d)", s, s );
        fail_if( hoshi == NULL, "hoshi initialised (%dx%d)", s, s );

        free_board();

        fail_if( board != NULL, "board is NULL" );
        fail_if( group != NULL, "group is NULL" );
        fail_if( hoshi != NULL, "hoshi is NULL" );
    } while ( board_size[i] != BOARD_SIZE_MAX );
}
END_TEST

/*
START_TEST (test_get_board_as_string_1)
{



}
END_TEST
*/

Suite * board_suite(void) {
    Suite *s             = suite_create("Run");
    TCase *tc_init_board = tcase_create("init_board");

    tcase_add_test( tc_init_board, test_init_board_1 );

    suite_add_tcase( s, tc_init_board );

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

