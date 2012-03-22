#include <stdlib.h>
#include <check.h>
#include "../src/global_const.h"
#include "../new_src/new_board.h"
#include "../new_src/new_board_intern.h"


START_TEST (test_init_board_1)
{
    bsize_t s;

    for ( s = BOARD_SIZE_MIN; s <= BOARD_SIZE_MAX; s++ ) {
        init_board(s);
        free_board();
    }
}
END_TEST

START_TEST (test_init_board_2)
{
    bsize_t s = BOARD_SIZE_MAX + 1;
    init_board(s);
    free_board();
}
END_TEST

START_TEST (test_init_board_3)
{
    bsize_t s = BOARD_SIZE_MIN - 1;
    init_board(s);
    free_board();
}
END_TEST

START_TEST (test_board_size_1)
{
    bsize_t s;

    for ( s = BOARD_SIZE_MIN; s <= BOARD_SIZE_MAX; s++ ) {
        set_board_size(s);
        fail_unless( s == get_board_size(), "board size set to %u", s );
    }

}
END_TEST

START_TEST (test_board_size_2)
{
    bsize_t s = BOARD_SIZE_MAX + 1;

    set_board_size(s);
}
END_TEST

START_TEST (test_board_size_3)
{
    bsize_t s = BOARD_SIZE_MIN - 1;

    set_board_size(s);
}
END_TEST



Suite * board_suite(void)
{
    Suite *s = suite_create("New Board");

    TCase *tc_init_board = tcase_create("init_board");
    TCase *tc_board_size = tcase_create("board_size");

    tcase_add_exit_test( tc_init_board, test_init_board_1, EXIT_SUCCESS );
    tcase_add_exit_test( tc_init_board, test_init_board_2, EXIT_FAILURE );
    tcase_add_exit_test( tc_init_board, test_init_board_3, EXIT_FAILURE );

    tcase_add_test( tc_board_size, test_board_size_1 );
    tcase_add_exit_test( tc_board_size, test_board_size_2, EXIT_FAILURE );
    tcase_add_exit_test( tc_board_size, test_board_size_3, EXIT_FAILURE );

    suite_add_tcase( s, tc_init_board );
    suite_add_tcase( s, tc_board_size );

    return s;
}

int main(void)
{
    int number_failed;

    Suite *s = board_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;;
}

