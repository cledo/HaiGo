#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "global_const.h"
#include "board.h"
#include "move.h"
#include "evaluate.h"
#include "global_tools.h"
#include "search.h"


/**
 * @file    search.c
 *
 * @brief   Engine for building the move tree.
 *
 * [Multi-line
 * description still missing ...]
 *
 * @todo Description still missing!
 *
 */

static unsigned hash_hit;   //!< Counts the hits in the hash table.
static int alpha_break;     //!< Count alpha breaks.
static int beta_break;      //!< Count beta breaks.

static int count_quiet_search;  //!< Counts the nodes in quiescence search.

static int search_level = DEFAULT_SEARCH_LEVEL; //!< Sets depth of search tree.

static bool do_log = false;                     //!< Defines if logging is turned on or off.

static unsigned long long int node_count;       //!< Counts the number of nodes in move tree.
static FILE *log_file = NULL;                   //!< Log file handler

static int  add_node( int color, int tree_level, int alpha, int beta );
static void make_move( int color, int i, int j );
static void undo_move(void);


/**
 * @brief       Builds move tree.
 *
 * Builds a complete move tree recursively.
 *
 * @param[in]   color       Color to move
 * @param[out]  *i_selected Pointer to horizontal coordinate of selected move.
 * @param[out]  *j_selected Pointer to vertical coordinate of selected move.
 * @return      Nothing
 * @note        If no move is selected i and j are INVALID.
 */
void search_tree( int color, int *i_selected, int *j_selected )
{
    // Index variables:
    int k;
    int l;
    int m;   //DEBUG
    int i, j;

    // Variables for search tree:
    int tree_level = 0;
    int best_value;
    int search_level_incr;
    int alpha;
    int beta;

    // Variables for move list:
    int valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][4];
    int nr_of_valid_moves;
    int nr_of_valid_moves_cut;

    // Variables for measuring time:
    time_t start;
    time_t stop;
    time_t diff_time;

    // Variables needed for logging:
    char x[2];
    char y[3];

    // Setting to root level values:
    alpha = INT_MIN;
    beta  = INT_MAX;
    hash_hit    = 0;
    alpha_break = 0;
    beta_break  = 0;
    search_level_incr = 0;
    count_quiet_search = 0;

    init_hash_table();

    best_value = ( color == BLACK ) ? INT_MIN : INT_MAX;

    node_count = 0;

    if ( do_log ) {
        log_file = fopen( LOG_FILE, "w" );
        if ( log_file == NULL ) {
            printf( "# Cannot open log file\n" );
            exit(1);
        }
    }

    (void) time(&start);

    nr_of_valid_moves     = get_valid_move_list( color, valid_moves );
    nr_of_valid_moves_cut = nr_of_valid_moves;

    // Loop start:
    search_level_incr = get_search_level();
    for ( l = 0; l <= search_level_incr; l++ ) {
        set_search_level(l);
        //set_search_level(search_level_incr);
        //l = search_level_incr;

        // Go through move list:
        for ( k = 0; k < nr_of_valid_moves_cut; k++ ) {
            i = valid_moves[k][0];
            j = valid_moves[k][1];

            // Make move:
            node_count++;
            //printf( "# Level: %d make: %d,%d value: %d\n", l, i, j, best_value );
            make_move( color, i, j );
            //i_to_x( i, x );
            //j_to_y( j, y );
            //printf( "## %s%s\n", x, y );

            // Start recursion:
            valid_moves[k][2] = add_node( color * -1, tree_level, alpha, beta );

            if ( color  == BLACK ) {
                // For black: remember highest value
                if ( valid_moves[k][2] > best_value ) {
                    best_value = valid_moves[k][2];
                    if ( best_value > alpha ) {
                        alpha = best_value;
                    }
                }
            }
            else {
                // For white: remember lowest value
                if ( valid_moves[k][2] < best_value ) {
                    best_value = valid_moves[k][2];
                    if ( best_value < beta ) {
                        beta = best_value;
                    }
                }
            }

            if ( do_log ) {
                i_to_x( i, x );
                j_to_y( j, y );
                fprintf( log_file, "%s%s (%d)\n", x, y, valid_moves[k][2] );
            }

            undo_move();
        }

        // Sort move list by value:
        if ( color == BLACK ) {
            qsort( valid_moves, (size_t)nr_of_valid_moves_cut, sizeof(valid_moves[0]), compare_value_black );
        }
        else {
            qsort( valid_moves, (size_t)nr_of_valid_moves_cut, sizeof(valid_moves[0]), compare_value_white );
        }

        // DEBUG:
        printf( "# Level: %d (%d) - ", l, nr_of_valid_moves_cut );
        for ( m = 0; m < nr_of_valid_moves_cut; m++ ) {
            i_to_x( valid_moves[m][0], x );
            j_to_y( valid_moves[m][1], y );
            printf( "%s%s (%d,%d), ", x, y, valid_moves[m][2], valid_moves[m][3] );
        }
        printf("\n");

        if ( nr_of_valid_moves_cut / 2 > 5 ) {
            nr_of_valid_moves_cut = nr_of_valid_moves_cut / 2;
        }

    }
    // Loop end

    (void) time(&stop);

    diff_time = stop - start;
    if ( diff_time == 0 ) {
        diff_time = 1;
    }

    printf( "#### Node count: %llu ####\n", node_count );
    printf( "# Level:       %d\n", search_level );
    printf( "# Duration:    %ld\n", stop - start );
    printf( "# Nodes/sec.:  %llu\n", node_count / diff_time );
    printf( "# HashHit:     %u\n", hash_hit );
    printf( "# Alpha break: %d\n", alpha_break );
    printf( "# Beta break:  %d\n", beta_break );
    printf( "# Q-Search:    %d\n", count_quiet_search );
    printf( "# Value: (%d)\n", valid_moves[0][2] );

    *i_selected = valid_moves[0][0];
    *j_selected = valid_moves[0][1];

    if ( log_file != NULL ) {
        fclose(log_file);
    }

    return;
}

/**
 * @brief       Adds a new node to the move tree.
 *
 * A new node is added to the move tree. For every move in the generated move
 * list, a new node is added recursively. If a certain level is reached, the
 * recusrion is stopped.
 *
 * @param[in]   color       Color of move to set.
 * @param[in]   tree_level  Counter that shows the level in the move tree.
 * @param[in]   alpha       Alpha-Beta pruning
 * @param[in]   beta        Alpha-Beta pruning
 * @return      Value of position
 */
int add_node( int color, int tree_level, int alpha, int beta )
{
    int  k, l;
    int  i, j;
    int  valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][4];
    int  nr_of_valid_moves;
    int  best_value;
    char x[2];
    char y[3];
    char indent[10];
    int  tactic_move = 0;
    int  qsearch = MAX_QSEARCH_DEPTH;
    //unsigned hash_id;
    int value_list[5] = { 1, 2, 3, 4, 5 };


    best_value = ( color == BLACK ) ? INT_MIN : INT_MAX;
    if ( ! ( ( search_level + MAX_QSEARCH_DEPTH ) % 2 ) ) {
        qsearch++;
    }

    tree_level++;

    nr_of_valid_moves = get_valid_move_list( color, valid_moves );

    // PASS if no valid move is possible:
    if ( nr_of_valid_moves == 0 ) {
        make_move( color, INVALID, INVALID );
        best_value = evaluate_position(value_list);
        undo_move();
    }

    // Count tactic moves:
    for ( l = 0; l < nr_of_valid_moves; l++ ) {
        if ( valid_moves[l][3] > 0 ) {
            tactic_move++;
        }
    }

    // Go through move list:
    for ( k = 0; k < nr_of_valid_moves; k++ ) {
        i = valid_moves[k][0];
        j = valid_moves[k][1];

        // Skip non-tactical moves in quiescense search:
        if ( tree_level >= search_level && tactic_move > 0) {
            if ( valid_moves[k][3] == 0 ) {
                continue;
            }
        }

        // Make move:
        node_count++;
        make_move( color, i, j );


        if ( tree_level < search_level ) {
            // Start recursion:
            valid_moves[k][2] = add_node( color * -1, tree_level, alpha, beta );
        }
        else {
            //insert_hash_table( hash_id, best_value );
            if ( tactic_move && tree_level < search_level + qsearch ) {
                valid_moves[k][2] = add_node( color * -1, tree_level, alpha, beta );
                count_quiet_search++;
            }
            else {
                valid_moves[k][2] = evaluate_position(value_list);
            }
        }

        if ( color  == BLACK ) {
            // For black: remember highest value
            if ( valid_moves[k][2] > best_value ) {
                best_value = valid_moves[k][2];
                if ( best_value > alpha ) {
                    alpha = best_value;
                }
            }
        }
        else {
            // For white: remember lowest value
            if ( valid_moves[k][2] < best_value ) {
                best_value = valid_moves[k][2];
                if ( best_value < beta ) {
                    beta = best_value;
                }
            }
        }

        if ( do_log ) {
            i_to_x( i, x );
            j_to_y( j, y );
            indent[0] = '\0';
            for ( l = 1; l <= tree_level; l++ ) {
                strcat( indent, "\t" );
            }
            fprintf( log_file, "%s%s%s (%d) (T: %d) (%d,%d,%d,%d,%d)\n", indent, x, y
                , valid_moves[k][2], valid_moves[k][3], value_list[0],
                value_list[1], value_list[2], value_list[3], value_list[4] );
        }

        undo_move();

        if ( color == BLACK ) {
            if ( valid_moves[k][2] > beta ) {  // Maybe only '>' is correct?!
                beta_break++;
                break;
            }
        }
        else {
            if ( valid_moves[k][2] < alpha ) { // Maybe only '<' is correct?!
                alpha_break++;
                break;
            }
        }
    }

    return best_value;
}

/**
 * @brief       Performs move
 *
 * Performs a move on the board and adds it to the move history.
 *
 * @param[in]   color   Color of stone to move
 * @param[in]   i       Horizontal coordinate of move
 * @param[in]   j       Vertical coordinate of move
 * @return      Nothing
 * @sa          undo_move()
 */
void make_move( int color, int i, int j )
{
    int nr_of_removed_stones;
    int group_nr;
    int nr_of_liberties;
    int group_size;
    int captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

    // Check for pass:
    if ( i == INVALID && j == INVALID ) {
        create_next_move();
        set_move_pass(color);
        push_move();

        return;
    }

    set_vertex( color, i, j );
    create_groups();
    set_groups_size();
    count_liberties();
    nr_of_removed_stones = remove_stones( color * -1 );
    if ( nr_of_removed_stones > 0 ) {
        create_groups();
        set_groups_size();
        count_liberties();
    }

    nr_of_removed_stones = get_captured_now(captured_now);

    create_next_move();
    set_move_vertex( color, i, j );
    set_move_captured_stones(captured_now);

    group_nr        = get_group_nr( i, j );
    nr_of_liberties = get_nr_of_liberties(group_nr);
    group_size      = get_size_of_group(group_nr);

    // Check if this move is a ko:
    if ( nr_of_removed_stones == 1 && group_size == 1 && nr_of_liberties == 1 ) {
        // If only one stone has been captured it must be the first in the
        // captured_now list:
        set_move_ko( captured_now[0][0], captured_now[0][1] );
    }

    push_move();

    return;
}

/**
 * @brief       Performs undo move.
 *
 * Takes back last move from move history and on the board.
 *
 * @return      Nothing
 * @sa          make_move()
 */
void undo_move(void)
{
    int k;
    int i            = get_last_move_i();
    int j            = get_last_move_j();
    int color        = get_last_move_color();
    int count_stones = get_last_move_count_stones();
    int stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

    if ( get_last_move_pass() ) {
        pop_move();

        return;
    }

    get_last_move_stones(stones);

    set_vertex( EMPTY, i, j );
    if ( count_stones > 0 ) {
        for ( k = 0; k < count_stones; k++ ) {
            set_vertex( color * -1, stones[k][0], stones[k][1] );
        }

        if ( color == BLACK ) {
            set_black_captured( get_black_captured() - count_stones );
        }
        else {
            set_white_captured( get_white_captured() - count_stones );
        }
    }

    pop_move();

    return;
}

/**
 * @brief       Sets level of search tree.
 *
 * Determines the maximum level of the search depth.
 *
 * @param[in]   level   Search tree depth
 * @return      Nothing
 */
void set_search_level( int level )
{

    search_level = level;

    return;
}

/**
 * @brief       Returns level of search tree.
 *
 * Returns the currently set depth of the search tree.
 *
 * @return      Level of search tree
 */
int get_search_level(void)
{

    return search_level;
}

/**
 * @brief       Helper function for qsort().
 *
 * This is a helper function for qsort(), that makes it possible to sort the
 * move list by move value. The best move for black is sorted first.
 *
 * @param[in]   move1   Pointer to first move
 * @param[in]   move2   Pointer to second move
 * @return      1|0|-1
 * @sa          man 3 qsort
 */
int compare_value_black( const void *move1, const void *move2 )
{
    if ( ((int *)move1)[2] > ( (int *)move2)[2] ) {
        return -1;
    }
    else if ( ( (int *)move1)[2] < ( (int *)move2)[2] ) {
        return 1;
    }

    return 0;
}

/**
 * @brief       Helper function for qsort().
 *
 * This is a helper function for qsort(), that makes it possible to sort the
 * move list by move value. The best move for white is sorted first.
 *
 * @param[in]   move1   Pointer to first move
 * @param[in]   move2   Pointer to second move
 * @return      1|0|-1
 * @sa          man 3 qsort
 */
int compare_value_white( const void *move1, const void *move2 )
{
    if ( ((int *)move1)[2] > ( (int *)move2)[2] ) {
        return 1;
    }
    else if ( ( (int *)move1)[2] < ( (int *)move2)[2] ) {
        return -1;
    }

    return 0;
}

/**
 * @brief       Checks if logging is turned on or off.
 *
 * Returns the value of the do_log flag, to determine if logging is currently
 * turned on or off.
 *
 * @return      true|false
 * @sa          set_do_log()
 */
bool get_do_log(void)
{

    return do_log;
}

/**
 * @brief       Turns logging of search tree on or off.
 *
 * If logging is currently disabled, this function turns logging on, and vice
 * versa.
 *
 * @return      Nothing.
 * @sa          get_do_log()
 */
void set_do_log(void)
{

    do_log = get_do_log() ? false : true;

    return;
}

