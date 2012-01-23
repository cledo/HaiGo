#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "../src/board.h"

START_TEST (test_init_board_1)
{
    int **board;
    int **hoshi;

    init_board(19);

    fail_if( board == NULL, "board initialised" );

}

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

