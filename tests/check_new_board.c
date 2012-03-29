#include <stdlib.h>
#include <stdio.h>  // TEST
#include <stdbool.h>
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
        fail_unless( is_board_null(), "boards set to NULL" );
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

START_TEST (test_on_board_1)
{
    int i, j;
    int board_size;

    for ( board_size = BOARD_SIZE_MIN; board_size <= BOARD_SIZE_MAX; board_size++ ) {
        init_board(board_size);

        for ( i = -1; i <= board_size; i++ ) {
            for ( j = -1; j <= board_size; j++ ) {
                if ( i < 0 || i >= board_size || j < 0 || j >= board_size ) {
                    fail_unless( ! is_on_board( i, j ), "%d,%d off board", i, j );
                }
                else {
                    fail_unless( is_on_board( i, j ), "%d,%d on board", i, j );
                }
            }
        }

        free_board();
    }

}
END_TEST

START_TEST (test_hoshi_1)
{
    int i;

    bsize_t board_size = 9;

    int hoshi[] = {
        2,2,
        2,6,
        6,2,
        6,6,
        4,4,
    };

    init_board(board_size);

    for ( i = 0; i < 10; i += 2 ) {
        fail_unless( is_hoshi( hoshi[i], hoshi[i+1] ), "%d,%d is hoshi", hoshi[i], hoshi[i+1] );
    }

    free_board();
}
END_TEST

START_TEST (test_hoshi_2)
{
    int i;

    bsize_t board_size = 13;

    int hoshi[] = {
        3,3,
        3,9,
        9,3,
        9,9,
        6,6,
    };

    init_board(board_size);

    for ( i = 0; i < 10; i += 2 ) {
        fail_unless( is_hoshi( hoshi[i], hoshi[i+1] ), "%d,%d is hoshi", hoshi[i], hoshi[i+1] );
    }

    free_board();
}
END_TEST

START_TEST (test_hoshi_3)
{
    int i;

    bsize_t board_size = 19;

    int hoshi[] = {
        3,3,
        3,9,
        3,15,
        9,3,
        9,9,
        9,15,
        15,3,
        15,9,
        15,15,
    };

    init_board(board_size);

    for ( i = 0; i < 10; i += 2 ) {
        fail_unless( is_hoshi( hoshi[i], hoshi[i+1] ), "%d,%d is hoshi", hoshi[i], hoshi[i+1] );
    }

    free_board();
}
END_TEST

START_TEST (test_vertex)
{
    int i, j;
    bsize_t s;

    for ( s = BOARD_SIZE_MIN; s <= BOARD_SIZE_MAX; s++ ) {
        init_board(s);

        for ( i = 0; i < s; i++ ) {
            for ( j = 0; j < s; j++ ) {
                set_vertex( BLACK, i, j );
                fail_unless( get_vertex( i, j ) == BLACK );
                set_vertex( EMPTY, i, j );
                fail_unless( get_vertex( i, j ) == EMPTY );
                set_vertex( WHITE, i, j );
                fail_unless( get_vertex( i, j ) == WHITE );
            }
        }

        free_board();
    }

}
END_TEST

START_TEST (test_worms_1)
{
    bsize_t s = 3;

    init_board(s);

    set_vertex( BLACK, 0, 0 );
    set_vertex( BLACK, 0, 2 );
    set_vertex( BLACK, 2, 0 );
    set_vertex( BLACK, 2, 2 );

    scan_board();

    fail_unless(  get_worm( BLACK, 1 ).count  == 1 );
    fail_unless(  get_worm( BLACK, 1 ).number == 1 );
    fail_unless(  get_worm( BLACK, 2 ).count  == 1 );
    fail_unless(  get_worm( BLACK, 2 ).number == 2 );
    fail_unless(  get_worm( BLACK, 3 ).count  == 1 );
    fail_unless(  get_worm( BLACK, 3 ).number == 3 );
    fail_unless(  get_worm( BLACK, 4 ).count  == 1 );
    fail_unless(  get_worm( BLACK, 4 ).number == 4 );

    fail_unless( get_worm( WHITE, 1 ).count  == 0 );
    fail_unless( get_worm( WHITE, 1 ).number == 0 );

    fail_unless( get_worm( EMPTY, 1 ).count  == 5 );
    fail_unless( get_worm( EMPTY, 1 ).number == 1 );

    set_vertex( BLACK, 0, 1 );
    set_vertex( BLACK, 1, 0 );
    set_vertex( BLACK, 2, 1 );
    set_vertex( BLACK, 1, 2 );

    scan_board();

    fail_unless( get_worm( BLACK, 1 ).count  == 8 );
    fail_unless( get_worm( BLACK, 1 ).number == 1 );
    fail_unless( get_worm( BLACK, 2 ).count  == 0 );
    fail_unless( get_worm( BLACK, 2 ).number == 0 );

    fail_unless( get_worm( WHITE, 1 ).count  == 0 );
    fail_unless( get_worm( WHITE, 1 ).number == 0 );

    fail_unless( get_worm( EMPTY, 1 ).count  == 1 );
    fail_unless( get_worm( EMPTY, 1 ).number == 1 );

    free_board();
}
END_TEST

START_TEST (test_worms_2)
{
    bsize_t s = 3;

    init_board(s);

    set_vertex( WHITE, 0, 0 );
    set_vertex( WHITE, 0, 2 );
    set_vertex( WHITE, 2, 0 );
    set_vertex( WHITE, 2, 2 );

    scan_board();

    fail_unless(  get_worm( WHITE, 1 ).count  == 1 );
    fail_unless(  get_worm( WHITE, 1 ).number == 1 );
    fail_unless(  get_worm( WHITE, 2 ).count  == 1 );
    fail_unless(  get_worm( WHITE, 2 ).number == 2 );
    fail_unless(  get_worm( WHITE, 3 ).count  == 1 );
    fail_unless(  get_worm( WHITE, 3 ).number == 3 );
    fail_unless(  get_worm( WHITE, 4 ).count  == 1 );
    fail_unless(  get_worm( WHITE, 4 ).number == 4 );

    fail_unless( get_worm( BLACK, 1 ).count  == 0 );
    fail_unless( get_worm( BLACK, 1 ).number == 0 );

    fail_unless( get_worm( EMPTY, 1 ).count  == 5 );
    fail_unless( get_worm( EMPTY, 1 ).number == 1 );

    set_vertex( WHITE, 0, 1 );
    set_vertex( WHITE, 1, 0 );
    set_vertex( WHITE, 2, 1 );
    set_vertex( WHITE, 1, 2 );

    scan_board();

    fail_unless( get_worm( WHITE, 1 ).count  == 8 );
    fail_unless( get_worm( WHITE, 1 ).number == 1 );
    fail_unless( get_worm( WHITE, 2 ).count  == 0 );
    fail_unless( get_worm( WHITE, 2 ).number == 0 );

    fail_unless( get_worm( BLACK, 1 ).count  == 0 );
    fail_unless( get_worm( BLACK, 1 ).number == 0 );

    fail_unless( get_worm( EMPTY, 1 ).count  == 1 );
    fail_unless( get_worm( EMPTY, 1 ).number == 1 );

    free_board();
}
END_TEST


Suite * board_suite(void)
{
    Suite *s = suite_create("New Board");

    TCase *tc_init_board = tcase_create("init_board");
    TCase *tc_board_size = tcase_create("board_size");
    TCase *tc_hoshi      = tcase_create("hoshi");
    TCase *tc_vertex     = tcase_create("vertex");
    TCase *tc_worms      = tcase_create("worms");

    tcase_add_exit_test( tc_init_board, test_init_board_1, EXIT_SUCCESS );
    tcase_add_exit_test( tc_init_board, test_init_board_2, EXIT_FAILURE );
    tcase_add_exit_test( tc_init_board, test_init_board_3, EXIT_FAILURE );

    tcase_add_test( tc_board_size, test_board_size_1 );
    tcase_add_exit_test( tc_board_size, test_board_size_2, EXIT_FAILURE );
    tcase_add_exit_test( tc_board_size, test_board_size_3, EXIT_FAILURE );
    tcase_add_test( tc_board_size, test_on_board_1 );

    tcase_add_test( tc_hoshi, test_hoshi_1 );
    tcase_add_test( tc_hoshi, test_hoshi_2 );
    tcase_add_test( tc_hoshi, test_hoshi_3 );

    tcase_add_test( tc_vertex, test_vertex );

    tcase_add_test( tc_worms, test_worms_1 );
    tcase_add_test( tc_worms, test_worms_2 );

    suite_add_tcase( s, tc_init_board );
    suite_add_tcase( s, tc_board_size );
    suite_add_tcase( s, tc_hoshi      );
    suite_add_tcase( s, tc_vertex     );
    suite_add_tcase( s, tc_worms      );

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

