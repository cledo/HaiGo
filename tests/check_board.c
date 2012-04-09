#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <check.h>
#include "../src/global_const.h"
#include "../src/board_intern.h"
#include "../src/board.h"
#include "../src/move.h"
#include "../src/evaluate.h"


static const int board_count  = 5;
static const int board_size[] = {
    BOARD_SIZE_MIN,
    9,
    13,
    19,
    BOARD_SIZE_MAX
};

static char test_board_black[] = {
"\n"
"    A B\n"
"  2 . . 2\t    WHITE (0) has captured 0 stones\n"
"  1 X . 1\t    BLACK (X) has captured 0 stones\n"
"    A B"
};

static char test_board_white[] = {
"\n"
"    A B\n"
"  2 . . 2\t    WHITE (0) has captured 0 stones\n"
"  1 0 . 1\t    BLACK (X) has captured 0 stones\n"
"    A B"
};

static char *test_board[] = {
"\n"
"    A B\n"
"  2 . . 2\t    WHITE (0) has captured 0 stones\n"
"  1 . . 1\t    BLACK (X) has captured 0 stones\n"
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
"  2 . . . . . . . . . 2\t    WHITE (0) has captured 0 stones\n"
"  1 . . . . . . . . . 1\t    BLACK (X) has captured 0 stones\n"
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
"  5 . . . . . . . . . . . . . 5\t    WHITE (0) has captured 0 stones\n"
"  4 . . . + . . . . . + . . . 4\t    BLACK (X) has captured 0 stones\n"
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
" 11 . . . . . . . . . . . . . . . . . . . 11\t    WHITE (0) has captured 0 stones\n"
" 10 . . . + . . . . . + . . . . . + . . . 10\t    BLACK (X) has captured 0 stones\n"
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
"    A B C D E F G H J K L M N O P Q R S T U V\n"
" 21 . . . . . . . . . . . . . . . . . . . . . 21\n"
" 20 . . . . . . . . . . . . . . . . . . . . . 20\n"
" 19 . . . . . . . . . . . . . . . . . . . . . 19\n"
" 18 . . . . . . . . . . . . . . . . . . . . . 18\n"
" 17 . . . . . . . . . . . . . . . . . . . . . 17\n"
" 16 . . . . . . . . . . . . . . . . . . . . . 16\n"
" 15 . . . . . . . . . . . . . . . . . . . . . 15\n"
" 14 . . . . . . . . . . . . . . . . . . . . . 14\n"
" 13 . . . . . . . . . . . . . . . . . . . . . 13\t    WHITE (0) has captured 0 stones\n"
" 12 . . . . . . . . . . . . . . . . . . . . . 12\t    BLACK (X) has captured 0 stones\n"
" 11 . . . . . . . . . . . . . . . . . . . . . 11\n"
" 10 . . . . . . . . . . . . . . . . . . . . . 10\n"
"  9 . . . . . . . . . . . . . . . . . . . . . 9\n"
"  8 . . . . . . . . . . . . . . . . . . . . . 8\n"
"  7 . . . . . . . . . . . . . . . . . . . . . 7\n"
"  6 . . . . . . . . . . . . . . . . . . . . . 6\n"
"  5 . . . . . . . . . . . . . . . . . . . . . 5\n"
"  4 . . . . . . . . . . . . . . . . . . . . . 4\n"
"  3 . . . . . . . . . . . . . . . . . . . . . 3\n"
"  2 . . . . . . . . . . . . . . . . . . . . . 2\n"
"  1 . . . . . . . . . . . . . . . . . . . . . 1\n"
"    A B C D E F G H J K L M N O P Q R S T U V",
};

START_TEST (test_init_board_1)
{
    int i, j;
    int s = board_size[_i];

    init_board(s);

    fail_if( is_board_null(),       "board is initialized"       );

    for ( i = 0; i < s; i++ ) {
        for ( j = 0; j < s; j++ ) {
            fail_unless( get_vertex( i, j ) == EMPTY
                , "field %d,%d is EMPTY", i, j );
            fail_unless( get_worm_nr( i, j ) == 0
                , "group_nr for %d,%d is 0", i, j );
            fail_unless( is_hoshi( i, j ) == true || is_hoshi( i, j ) == false
                , "hoshi is initialized for %d,%d", i, j );
        }
    }

    fail_if( s != get_board_size(), "correct board size returned (%d,%d)", s, s );

    free_board();

    fail_unless( is_board_null(),       "board is NULL"       );
}
END_TEST

START_TEST (test_get_board_as_string_1)
{
    char board_output[MAX_OUTPUT_LENGTH];

    int s = board_size[_i];

    init_board(s);
    get_board_as_string(board_output);

    //printf( "%s", test_board[_i] );
    //printf( "%s", board_output );
    fail_if( strcmp( test_board[_i], board_output ) != 0, "board string received (%dx%d)", s, s );

    init_board(2);
    set_vertex( BLACK, 0, 0 );
    get_board_as_string(board_output);

    fail_if( strcmp( test_board_black, board_output ) != 0, "board string received with black" );

    set_vertex( WHITE, 0, 0 );
    get_board_as_string(board_output);

    fail_if( strcmp( test_board_white, board_output ) != 0, "board string received with white" );
}
END_TEST

START_TEST (test_groups_1)
{
    int s = BOARD_SIZE_DEFAULT;

    init_board(s);

    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 1, "next worm for black is 1" );
    fail_if( get_free_worm_nr(WHITE) != 1, "next worm for white is 1" );
    fail_if( get_free_worm_nr(EMPTY) != 2, "next worm for empty is 2" );

    set_vertex( BLACK, 0, 0 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 2, "next worm for black is 2" );
    fail_if( get_free_worm_nr(WHITE) != 1, "next worm for white is 1" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    set_vertex( WHITE, 0, 1 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 2, "next worm for black is 2" );
    fail_if( get_free_worm_nr(WHITE) != 2, "next worm for white is 2" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    set_vertex( BLACK, 10, 10 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 3, "next worm for black is 3" );
    fail_if( get_free_worm_nr(WHITE) != 2, "next worm for white is 2" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    set_vertex( WHITE, 15, 15 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 3, "next worm for black is 3" );
    fail_if( get_free_worm_nr(WHITE) != 3, "next worm for white is 3" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    set_vertex( BLACK, 10, 11 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 3, "next worm for black is 3" );
    fail_if( get_free_worm_nr(WHITE) != 3, "next worm for white is 3" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    set_vertex( WHITE, 15, 16 );
    scan_board_1();

    fail_if( get_free_worm_nr(BLACK) != 3, "next worm for black is 3" );
    fail_if( get_free_worm_nr(WHITE) != 3, "next worm for white is 3" );
    fail_if( get_free_worm_nr(EMPTY) != 3, "next worm for empty is 3" );

    free_board();
}
END_TEST

START_TEST (test_groups_2)
{
    int i, j;
    int s = BOARD_SIZE_DEFAULT;
    int neighbour[4][2];

    for ( i = 0; i < 4; i++ ) {
        for ( j = 0; j < 2; j++ ) {
            neighbour[i][j] = -1;
        }
    }

    init_board(s);

    // No neighbour
    set_vertex( BLACK, 0,  0 );
    fail_if( has_neighbour( 0, 0, neighbour ) != 0, "no neighbours" );
    fail_if( neighbour[0][0] != -1, "no neighbour found" );


    // One neighbour
    set_vertex( BLACK, 0, 18 );
    set_vertex( BLACK, 0, 17 );
    fail_if( has_neighbour( 0, 18, neighbour ) != 1, "one neighbours" );
    fail_if( neighbour[0][0] != 0 || neighbour[0][1] != 17, "first neighbour found" );

    // Two neighbours
    set_vertex( BLACK, 18, 18 );
    set_vertex( BLACK, 18, 17 );
    set_vertex( BLACK, 17, 18 );
    fail_if( has_neighbour( 18, 18, neighbour ) != 2, "two neighbours" );
    fail_if( neighbour[0][0] != 18 || neighbour[0][1] != 17, "first neighbour found" );
    fail_if( neighbour[1][0] != 17 || neighbour[1][1] != 18, "second neighbour found" );

    // Three neighbours
    set_vertex( BLACK, 17,  0 );
    set_vertex( BLACK, 17,  1 );
    set_vertex( BLACK, 18,  0 );
    set_vertex( BLACK, 16,  0 );
    fail_if( has_neighbour( 17, 0, neighbour ) != 3, "three neighbours" );
    fail_if( neighbour[0][0] != 17 || neighbour[0][1] != 1, "first neighbour found" );
    fail_if( neighbour[1][0] != 18 || neighbour[1][1] != 0, "second neighbour found" );
    fail_if( neighbour[2][0] != 16 || neighbour[2][1] != 0, "third neighbour found" );

    // Four neighbours
    set_vertex( BLACK,  9,  9 );
    set_vertex( BLACK,  9, 10 );
    set_vertex( BLACK, 10,  9 );
    set_vertex( BLACK,  9,  8 );
    set_vertex( BLACK,  8,  9 );
    fail_if( has_neighbour( 9, 9, neighbour ) != 4, "four neighbours" );
    fail_if( neighbour[0][0] != 9  || neighbour[0][1] != 10, "first neighbour found" );
    fail_if( neighbour[1][0] != 10 || neighbour[1][1] != 9,  "second neighbour found" );
    fail_if( neighbour[2][0] != 9  || neighbour[2][1] != 8,  "third neighbour found" );
    fail_if( neighbour[3][0] != 8  || neighbour[3][1] != 9,  "fourth neighbour found" );

}
END_TEST

START_TEST (test_group_size_1)
{
    int i, j;
    int s = BOARD_SIZE_DEFAULT;
    int group_nr;
    int group_size;

    init_board(s);

    i = 9;
    j = 9;

    // Black group:
    set_vertex( BLACK, i, j );
    scan_board_1();

    group_nr   = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 1, "black group has size 1" );

    set_vertex( BLACK, i+1, j+1 );
    scan_board_1();

    group_nr   = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 1, "black group has size 1" );

    set_vertex( BLACK, i+1, j );
    set_vertex( BLACK, i, j+1 );
    scan_board_1();

    group_nr   = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 4, "black group has size 4" );

    // White group:
    set_vertex( WHITE, i, j );
    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 1, "white group has size 1" );

    set_vertex( WHITE, i+1, j );
    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 2, "white group has size 2" );

    set_vertex( WHITE, i, j+1 );
    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 3, "white group has size 3" );

    set_vertex( WHITE, i+1, j+1 );
    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    group_size = get_size_of_worm(group_nr);

    fail_unless( group_size == 4, "white group has size 4" );
}
END_TEST

START_TEST (test_vertex_1)
{
    int i, j;
    int s = BOARD_SIZE_DEFAULT;

    init_board(s);

    // Set BLACK
    for ( i = 0; i < s; i++ ) {
        for ( j = 0; j < s; j++ ) {
            fail_if( get_vertex( i, j ) != EMPTY, "%d,%d is empty", i, j );
            set_vertex( BLACK, i, j );
            fail_if( get_vertex( i, j ) != BLACK, "%d,%d is black", i, j );
        }
    }

    // Set EMPTY
    for ( i = 0; i < s; i++ ) {
        for ( j = 0; j < s; j++ ) {
            set_vertex( EMPTY, i, j );
            fail_if( get_vertex( i, j ) != EMPTY, "%d,%d is empty", i, j );
        }
    }

    // Set WHITE
    for ( i = 0; i < s; i++ ) {
        for ( j = 0; j < s; j++ ) {
            fail_if( get_vertex( i, j ) != EMPTY, "%d,%d is empty", i, j );
            set_vertex( WHITE, i, j );
            fail_if( get_vertex( i, j ) != WHITE, "%d,%d is white", i, j );
        }
    }

    free_board();
}
END_TEST

START_TEST (test_count_liberties_1)
{
    int i, j;
    int s = BOARD_SIZE_DEFAULT;
    int group_nr;
    int liberties;


    init_board(s);

    // Set corner stone
    i = 0;
    j = 0;
    set_vertex( BLACK, i, j );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 2, "%d,%d has 2 liberties (%d)", i, j, liberties );

    i = 1;
    j = 0;
    set_vertex( BLACK, i, j );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 3, "%d,%d has 3 liberties", i, j );

    set_vertex( WHITE, 0, 1 );
    set_vertex( WHITE, 1, 1 );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 1, "%d,%d has 1 liberty", i, j );

    set_vertex( WHITE, 2, 0 );
    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 0, "%d,%d has no liberties", i, j );

    // Set center stone:

    i = 9;
    j = 9;
    set_vertex( BLACK, i, j );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 4, "%d,%d has 4 liberties", i, j );

    set_vertex( WHITE, i-1, j );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 3, "%d,%d has 3 liberties", i, j );

    set_vertex( WHITE, i, j-1 );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 2, "%d,%d has 2 liberties", i, j );

    set_vertex( WHITE, i+1, j );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 1, "%d,%d has 1 liberties", i, j );

    set_vertex( WHITE, i, j+1 );

    scan_board_1();

    group_nr  = get_worm_nr( i, j );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 0, "%d,%d has no liberties", i, j );

    // Set white stone:
    init_board(2);

    set_vertex( WHITE, 0, 0 );
    scan_board_1();

    group_nr  = get_worm_nr( 0, 0 );
    liberties = get_nr_of_liberties(group_nr);

    fail_unless( liberties == 2, "0,0 has 2 liberties" );
}
END_TEST

START_TEST (test_remove_stones_1)
{
    int i, j;
    int removed;
    int s = BOARD_SIZE_DEFAULT;
    int captured[2][2];

    init_board(s);

    // Capture one black stone:
    i = 18;
    j = 18;

    set_vertex( BLACK, i, j );
    set_vertex( WHITE, i-1, j );
    set_vertex( WHITE, i, j-1 );

    scan_board_1();
    removed = remove_stones(BLACK);
    removed = get_captured_now(captured);

    fail_unless( removed == 1, "one black stone removed" );
    fail_unless( captured[0][0] == i, "captured i is %d (%d)", i, captured[0][0] );
    fail_unless( captured[0][1] == j, "captured j is %d (%d)", j, captured[0][1] );

    removed = remove_stones(WHITE);
    removed = get_captured_now(captured);

    fail_unless( removed == 0, "no white stones removed" );
    fail_unless( captured[0][0] == INVALID && captured[0][1] == INVALID
        , "captured_now list is empty" );
    fail_unless( get_black_captured() == 0, "black has captured 0 stones" );
    fail_unless( get_white_captured() == 1, "white has captured 1 stone"  );


    // Capture all white stones:
    for ( i = 0; i < s; i++ ) {
        for ( j = 0; j < s ; j++ ) {
            set_vertex( WHITE, i, j );
        }
    }
    i--;
    j--;
    set_vertex( EMPTY, i, j );
    set_vertex( BLACK, i, j );

    scan_board_1();
    removed = remove_stones(WHITE);

    fail_unless( removed = s * s - 1, "all white stones removed" );

    fail_unless( get_black_captured() == s * s - 1, "black has captured %d stones", s*s-1 );
    fail_unless( get_white_captured() == 1, "white has captured 1 stone" );

    set_black_captured(0);
    set_white_captured(0);

    fail_unless( get_black_captured() == 0, "black_captured set to 0" );
    fail_unless( get_white_captured() == 0, "white_captured set to 0" );
}
END_TEST

START_TEST (test_atari_1)
{
    int board_size = 5;

    init_board(board_size);

    scan_board_1();

    fail_unless( get_worm_count_atari(BLACK) == 0, "no black groups in atari" );
    fail_unless( get_worm_count_atari(WHITE) == 0, "no white groups in atari" );

    set_vertex( BLACK, 0, 0 );
    set_vertex( WHITE, board_size - 1, board_size - 1 );

    scan_board_1();

    fail_unless( get_worm_count_atari(BLACK) == 0, "no black groups in atari" );
    fail_unless( get_worm_count_atari(WHITE) == 0, "no white groups in atari" );

    set_vertex( WHITE, 0, 1 );
    set_vertex( BLACK, board_size - 1, board_size - 2 );

    scan_board_1();

    fail_unless( get_worm_count_atari(BLACK) == 1, "one black group in atari" );
    fail_unless( get_worm_count_atari(WHITE) == 1, "one white group in atari" );

    set_vertex( BLACK, 0, 2 );
    set_vertex( WHITE, board_size - 1, board_size - 3 );

    scan_board_1();

    fail_unless( get_worm_count_atari(BLACK) == 2, "two black groups in atari" );
    fail_unless( get_worm_count_atari(WHITE) == 2, "two white groups in atari" );

    set_vertex( WHITE, 0, 3 );
    set_vertex( BLACK, board_size - 1, board_size - 4 );

    scan_board_1();

    fail_unless( get_worm_count_atari(BLACK) == 3, "three black groups in atari" );
    fail_unless( get_worm_count_atari(WHITE) == 3, "three white groups in atari" );
}
END_TEST


Suite * board_suite(void) {
    Suite *s                      = suite_create("Board");

    TCase *tc_init_board          = tcase_create("init_board");
    TCase *tc_get_board_as_string = tcase_create("get_board_as_string");
    TCase *tc_groups              = tcase_create("groups");
    TCase *tc_group_size          = tcase_create("group_size");
    TCase *tc_vertex              = tcase_create("vertex");
    TCase *tc_liberties           = tcase_create("liberties");
    TCase *tc_remove_stones       = tcase_create("remove");
    TCase *tc_atari_groups        = tcase_create("atari");

    tcase_add_loop_test( tc_init_board, test_init_board_1, 0, board_count );
    tcase_add_loop_test( tc_get_board_as_string, test_get_board_as_string_1, 0, board_count );
    tcase_add_test( tc_groups,        test_groups_1          );
    tcase_add_test( tc_groups,        test_groups_2          );
    tcase_add_test( tc_group_size,    test_group_size_1      );
    tcase_add_test( tc_vertex,        test_vertex_1          );
    tcase_add_test( tc_liberties,     test_count_liberties_1 );
    tcase_add_test( tc_remove_stones, test_remove_stones_1   );
    tcase_add_test( tc_atari_groups,  test_atari_1           );

    suite_add_tcase( s, tc_init_board          );
    suite_add_tcase( s, tc_get_board_as_string );
    suite_add_tcase( s, tc_groups              );
    suite_add_tcase( s, tc_group_size          );
    suite_add_tcase( s, tc_vertex              );
    suite_add_tcase( s, tc_liberties           );
    suite_add_tcase( s, tc_remove_stones       );
    suite_add_tcase( s, tc_atari_groups        );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s = board_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

