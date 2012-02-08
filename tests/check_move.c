#include <stdlib.h>
#include <stdbool.h>
#include <check.h>
#include "../src/global_const.h"
#include "../src/move.h"
#include "../src/board.h"

extern int move_number;

struct move {
    int  number;
    int  color;
    bool pass;
    int  ko[2];
    int  i;
    int  j;
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

} next_move;

struct move move_history[MOVE_HISTORY_MAX];

START_TEST (test_init_move_history_1)
{
    int k, l;

    init_move_history();

    fail_if( move_number != 0, "move number is 0" );

    for ( k = 0; k < MOVE_HISTORY_MAX; k++ ) {
        fail_if( move_history[k].number != INVALID, "number is INVALID after init" );
        fail_if( move_history[k].color  != EMPTY,   "color is EMPTY after init"    );
        fail_if( move_history[k].pass   != true,    "pass is true after init"      );
        fail_if( move_history[k].ko[0]  != INVALID, "ko[0] is INVALID after init"  );
        fail_if( move_history[k].ko[1]  != INVALID, "ko[1] is INVALID after init"  );
        fail_if( move_history[k].i      != INVALID, "i is INVALID after init"      );
        fail_if( move_history[k].j      != INVALID, "j is INVALID after init"      );
        for ( l = 0; l < BOARD_SIZE_MAX * BOARD_SIZE_MAX; l++ ) {
            fail_if( move_history[k].stones[l][0] != INVALID, "stones i is INVALID after init" );
            fail_if( move_history[k].stones[l][1] != INVALID, "stones j is INVALID after init" );
        }
    }

}
END_TEST

START_TEST (test_create_next_move_1)
{
    int k;
    int number = move_number;

    create_next_move();

    fail_if( next_move.number != number + 1, "next move number increased" );
    fail_if( next_move.color  != EMPTY,      "next move color EMPTY"      );
    fail_if( next_move.pass   != true,       "next move pass true"        );
    fail_if( next_move.i      != INVALID,    "next move i INVALID"        );
    fail_if( next_move.j      != INVALID,    "next move j INVALID"        );
    fail_if( next_move.ko[0]  != INVALID,    "next move ko[0] INVALID"    );
    fail_if( next_move.ko[1]  != INVALID,    "next move ko[1] INVALID"    );
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        fail_if( next_move.stones[k][0] != INVALID, "next move stone i INVALID" );
        fail_if( next_move.stones[k][1] != INVALID, "next move stone j INVALID" );
    }

}
END_TEST

START_TEST (test_set_move_vertex_1)
{
    int number = move_number;
    int color  = BLACK;
    int i      = 9;
    int j      = 9;

    create_next_move();
    set_move_vertex( color, i, j );

    fail_if( next_move.number != number + 1, "next move number increased" );

    fail_if( next_move.color != color, "next move color is %d",  color );
    fail_if( next_move.i     != i,     "next move i is %d",      i     );
    fail_if( next_move.j     != j,     "next move j is %d",      j     );
    fail_if( next_move.pass  != false, "next move pass is false"       );
}
END_TEST

START_TEST (test_set_move_captured_stones_1)
{
    int k;
    int i, j;
    int board_size = BOARD_SIZE_DEFAULT;
    int number = move_number;
    int captured_stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

    //
    // Captured stones list is empty:
    //
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        captured_stones[k][0] = INVALID;
        captured_stones[k][1] = INVALID;
    }

    create_next_move();
    set_move_captured_stones(captured_stones);

    fail_if( next_move.number != number + 1, "next move number increased" );

    fail_if( next_move.stones[0][0] != INVALID, "first stone has INVALID" );
    fail_if( next_move.stones[0][1] != INVALID, "first stone has INVALID" );

    //
    // One stone captured:
    //
    captured_stones[0][0] = 9;
    captured_stones[0][1] = 9;

    create_next_move();
    set_move_captured_stones(captured_stones);

    fail_if( next_move.number != number + 2, "next move number increased" );

    fail_if( next_move.stones[0][0] != 9,       "first stone has INVALID"  );
    fail_if( next_move.stones[0][1] != 9,       "first stone has INVALID"  );
    fail_if( next_move.stones[1][0] != INVALID, "second stone has INVALID" );
    fail_if( next_move.stones[1][1] != INVALID, "second stone has INVALID" );

    //
    // All stones captured (except one):
    //
    k = 0;
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( i == 0 && j == 0 ) {
                continue;
            }
            captured_stones[k][0] = i;
            captured_stones[k][1] = j;
            k++;
        }
    }

    create_next_move();
    set_move_captured_stones(captured_stones);

    fail_if( next_move.number != number + 3, "next move number increased" );

    k = 0;
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( i == 0 && j == 0 ) {
                continue;
            }
            fail_if( next_move.stones[k][0] != i && next_move.stones[k][1] != j, "stone saved as captured: %d,%d", i, j );
            k++;
        }
    }
    fail_if( next_move.stones[k][0] != INVALID && next_move.stones[k][1] != INVALID, "stones list ends with INVALID" );
    
}
END_TEST

START_TEST (test_set_move_ko_1)
{
    int number = move_number;
    int i      = 9;
    int j      = 9;

    create_next_move();
    set_move_ko( i, j );

    fail_if( next_move.number != number + 1, "next move number increased" );

    fail_if( next_move.ko[0] != i || next_move.ko[1] != j, "ko set" );
}
END_TEST

START_TEST (test_set_move_pass_1)
{
    int number = move_number;
    int color  = WHITE;

    create_next_move();
    set_move_pass(color);

    fail_if( next_move.number != number + 1, "next move number increased" );

    fail_if( next_move.color != color, "next move color set to %d", color );
    fail_if( next_move.pass  != true, "next move set to pass"             );
    fail_if( next_move.i     != INVALID, "next move i set to INVALID"     );
    fail_if( next_move.j     != INVALID, "next move j set to INVALID"     );

    color = BLACK;

    create_next_move();
    set_move_pass(color);

    fail_if( next_move.number != number + 2, "next move number increased" );

    fail_if( next_move.color != color,  "next move color set to %d", color );
    fail_if( next_move.pass  != true,    "next move set to pass"           );
    fail_if( next_move.i     != INVALID, "next move i set to INVALID"      );
    fail_if( next_move.j     != INVALID, "next move j set to INVALID"      );
}
END_TEST

START_TEST (test_get_move_last_ko)
{
    int number = move_number;
    int color  = BLACK;
    int i      = 9;
    int j      = 9;

    init_move_history();

    fail_if( get_move_last_ko_i()     != INVALID, "ko_i is INVALID"   );
    fail_if( get_move_last_ko_j()     != INVALID, "ko_j is INVALID"   );
    fail_if( get_move_last_ko_color() != EMPTY,   "ko_color is EMPTY" );

    create_next_move();
    set_move_vertex( color, i, j );
    set_move_ko( i-1, j );
    push_move();

    fail_if( next_move.number != number + 1, "next move number increased" );

    fail_if( get_move_last_ko_i()     != i-1,   "ko_i is correct"     );
    fail_if( get_move_last_ko_j()     != j,     "ko_j is correct"     );
    fail_if( get_move_last_ko_color() != color, "ko_color is correct" );

    fail_if( is_move_ko( WHITE, i-1, j ) != true,  "move is a ko"     );
    fail_if( is_move_ko( WHITE, i, j-1 ) != false, "move is not a ko" );
}
END_TEST

START_TEST (test_push_move)
{
    int k;
    int number = move_number;
    int color  = BLACK;
    int i      = 14;
    int j      = 16;

    init_move_history();

    create_next_move();
    set_move_vertex( color, i, j );
    push_move();

    number++;

    fail_if( next_move.number != number, "next move number increased" );

    fail_if( move_history[number].number != number,  "move history number correct" );
    fail_if( move_history[number].color  != color,   "move history color correct"  );
    fail_if( move_history[number].pass   != false,   "move history pass correct"   );
    fail_if( move_history[number].ko[0]  != INVALID, "move history ko[0] correct"  );
    fail_if( move_history[number].ko[1]  != INVALID, "move history ko[1] correct"  );
    fail_if( move_history[number].i      != i,       "move history i correct"      );
    fail_if( move_history[number].j      != j,       "move history j correct"      );
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        fail_if( move_history[number].stones[k][0] != INVALID, "move history stones list i correct" );
        fail_if( move_history[number].stones[k][1] != INVALID, "move history stones list j correct" );
    }

    color = WHITE;
    i = 4;
    j = 5;

    create_next_move();
    set_move_vertex( color, i, j );
    push_move();

    number++;

    fail_if( next_move.number != number, "next move number increased" );

    fail_if( move_history[number].number != number,  "move history number correct" );
    fail_if( move_history[number].color  != color,   "move history color correct"  );
    fail_if( move_history[number].pass   != false,   "move history pass correct"   );
    fail_if( move_history[number].ko[0]  != INVALID, "move history ko[0] correct"  );
    fail_if( move_history[number].ko[1]  != INVALID, "move history ko[1] correct"  );
    fail_if( move_history[number].i      != i,       "move history i correct"      );
    fail_if( move_history[number].j      != j,       "move history j correct"      );
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        fail_if( move_history[number].stones[k][0] != INVALID, "move history stones list i correct" );
        fail_if( move_history[number].stones[k][1] != INVALID, "move history stones list j correct" );
    }


}
END_TEST

START_TEST (test_push_move_fail)
{
    int color  = BLACK;
    int i      = 14;
    int j      = 16;

    move_number = MOVE_HISTORY_MAX;

    init_move_history();

    create_next_move();
    set_move_vertex( color, i, j );
    // This push_move() should fail:
    push_move();

}
END_TEST

START_TEST (test_get_valid_move_list)
{
    int k;
    int color      = BLACK;
    int board_size = 2;
    int valid_moves[board_size][2];

    for ( k = 0; k < board_size; k++ ) {
        valid_moves[k][0] = INVALID;
        valid_moves[k][1] = INVALID;
    }

    init_board(board_size);

    fail_if( true, "test for get_valid_move_list() still missing" );

}
END_TEST


Suite * move_suite(void) {
    Suite *s                    = suite_create("Run");

    TCase *tc_init_move_history        = tcase_create("init_move_history");
    TCase *tc_create_next_move         = tcase_create("create_next_move" );
    TCase *tc_set_move_vertex          = tcase_create("set_move_vertex"  );
    TCase *tc_set_move_captured_stones = tcase_create("set_move_captured_stones");
    TCase *tc_set_move_ko              = tcase_create("set_move_ko");
    TCase *tc_set_move_pass            = tcase_create("set_move_pass");
    TCase *tc_get_last_ko              = tcase_create("get_last_ko");
    TCase *tc_push_move                = tcase_create("push_move");
    TCase *tc_valid_move_list          = tcase_create("valid_move_list");

    tcase_add_test( tc_init_move_history,        test_init_move_history_1        );
    tcase_add_test( tc_create_next_move,         test_create_next_move_1         );
    tcase_add_test( tc_set_move_vertex,          test_set_move_vertex_1          );
    tcase_add_test( tc_set_move_captured_stones, test_set_move_captured_stones_1 );
    tcase_add_test( tc_set_move_ko,              test_set_move_ko_1              );
    tcase_add_test( tc_set_move_pass,            test_set_move_pass_1            );
    tcase_add_test( tc_get_last_ko,              test_get_move_last_ko           );
    tcase_add_test( tc_push_move,                test_push_move                  );
    tcase_add_test( tc_valid_move_list,          test_get_valid_move_list        );

    tcase_add_exit_test( tc_push_move, test_push_move_fail, EXIT_FAILURE );

    tcase_set_timeout( tc_init_move_history, 20 );

    suite_add_tcase( s, tc_init_move_history        );
    suite_add_tcase( s, tc_create_next_move         );
    suite_add_tcase( s, tc_set_move_vertex          );
    suite_add_tcase( s, tc_set_move_captured_stones );
    suite_add_tcase( s, tc_set_move_ko              );
    suite_add_tcase( s, tc_set_move_pass            );
    suite_add_tcase( s, tc_get_last_ko              );
    suite_add_tcase( s, tc_push_move                );
    suite_add_tcase( s, tc_valid_move_list          );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s = move_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
