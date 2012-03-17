#include <stdlib.h>
#include <stdbool.h>
#include <check.h>
#include "../src/board.h"
#include "../src/move.h"
#include "../src/evaluate.h"
#include "../src/search.h"

START_TEST ( test_init )
{
    search_stats_t search_stats;

    init_search_stats();

    search_stats = get_search_stats();

    fail_unless( search_stats.color[0]      == '\0', "color is empty"        );
    fail_unless( search_stats.move[0]       == '\0', "move is empty"         );
    fail_unless( search_stats.level         == 0,    "level is zero"         );
    fail_unless( search_stats.duration      == 0,    "level is zero"         );
    fail_unless( search_stats.node_count    == 0,    "level is zero"         );
    fail_unless( search_stats.nodes_per_sec == 0,    "nodes_per_sec is zero" );
    fail_unless( search_stats.hash_hit      == 0,    "hash_hit is zero"      );
    fail_unless( search_stats.alpha_cut     == 0,    "alpha_cut is zero"     );
    fail_unless( search_stats.beta_cut      == 0,    "beta_cut is zero"      );
    fail_unless( search_stats.value         == 0,    "value is zero"         );
}
END_TEST

START_TEST ( test_log )
{
    bool has_log;

    has_log = get_do_log();
    fail_unless( has_log == false, "logging turned off" );

    set_do_log();
    has_log = get_do_log();
    fail_unless( has_log == true, "logging turned on" );

    set_do_log();
    has_log = get_do_log();
    fail_unless( has_log == false, "logging turned off" );


}
END_TEST

START_TEST ( test_sort_by_value )
{
    int move1[] = { 1, 1, 0 };
    int move2[] = { 2, 2, 0 };

    int result;

    result = compare_value_black( (void *) move1, (void *) move2 );
    fail_unless( result == 0, "both moves equal" );
    result = compare_value_white( (void *) move1, (void *) move2 );
    fail_unless( result == 0, "both moves equal" );

    move1[2] = 10;

    result = compare_value_black( (void *) move1, (void *) move2 );
    fail_unless( result == -1, "move 1 has higher value" );
    result = compare_value_white( (void *) move1, (void *) move2 );
    fail_unless( result == 1, "move 1 has higher value" );

    move1[2] = -10;

    result = compare_value_black( (void *) move1, (void *) move2 );
    fail_unless( result == 1, "move 2 has higher value" );
    result = compare_value_white( (void *) move1, (void *) move2 );
    fail_unless( result == -1, "move 2 has higher value" );

}
END_TEST

START_TEST ( test_search_depth )
{
    int level;

    fail_unless( get_search_depth() == DEFAULT_SEARCH_DEPTH, "depth is default" );

    for ( level = 0; level <= MAX_SEARCH_DEPTH; level++ ) {
        set_search_depth(level);
        fail_unless( get_search_depth() == level, "depth is set to %d", level );
    }
}
END_TEST

START_TEST ( test_search_valid )
{
    int board_size;
    int level;
    int i, j;
    int color;
    //search_stats_t search_stats;

    board_size = BOARD_SIZE_MIN;

    // Check with level 0:
    level = 0;

    init_board(board_size);
    init_move_history();
    init_search_stats();

    set_search_depth(level);
    
    color = BLACK;
    search_tree( color, &i, &j );

    fail_unless( i != INVALID && j != INVALID, "valid move returned" );

    // Check with level 9:
    level = MAX_SEARCH_DEPTH;

    init_board(board_size);
    init_move_history();
    init_search_stats();

    set_search_depth(level);
    
    color = BLACK;
    search_tree( color, &i, &j );

    fail_unless( i != INVALID && j != INVALID, "valid move returned" );
}
END_TEST

START_TEST ( test_search_pass )
{
    int board_size;
    int level;
    int i, j;
    int color;
    //search_stats_t search_stats;

    board_size = BOARD_SIZE_MIN;
    level      = 0;
    color      = BLACK;

    init_board(board_size);
    init_move_history();
    init_search_stats();

    set_search_depth(level);

    set_vertex( WHITE, 0, 0 );
    set_vertex( WHITE, 1, 0 );
    set_vertex( WHITE, 0, 1 );
    set_vertex( WHITE, 1, 1 );

    search_tree( color, &i, &j );

    fail_unless( i == INVALID && j == INVALID, "pass move returned" );

    set_vertex( EMPTY, 0, 0 );
    
    color = WHITE;

    search_tree( color, &i, &j );

    fail_unless( i == INVALID && j == INVALID, "pass move returned" );
}
END_TEST

Suite * search_suite(void) {
    Suite *s = suite_create("Search");

    TCase *tc_misc   = tcase_create("misc");
    TCase *tc_search = tcase_create("search");

    tcase_add_test( tc_misc, test_init           );
    tcase_add_test( tc_misc, test_log            );
    tcase_add_test( tc_misc, test_sort_by_value  );
    tcase_add_test( tc_misc, test_search_depth   );

    tcase_add_test( tc_search, test_search_valid );
    tcase_add_test( tc_search, test_search_pass  );

    suite_add_tcase( s, tc_misc   );
    suite_add_tcase( s, tc_search );

    return s;
}

int main(void) {
    int number_failed;

    Suite *s = search_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all( sr, CK_NORMAL );
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}

