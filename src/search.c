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
 */

static int search_level = DEFAULT_SEARCH_LEVEL; //!< Sets depth of search tree.

static bool do_log = false;                     //!< Defines if logging is turned on or off.

static unsigned long long int node_count;       //!< Counts the number of nodes in move tree.
static FILE *log_file = NULL;                   //!< Log file handler

static int  add_node( int color, int tree_level );
static void make_move( int color, int i, int j );
static void undo_move(void);

static int compare_value_black( const void *move1, const void *move2 );
static int compare_value_white( const void *move1, const void *move2 );

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
    int    k;
    int    i, j;
    int    valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][3];
    int    nr_of_valid_moves;
    int    tree_level = 0;
    time_t start;
    time_t stop;
    time_t diff_time;
    char   x[2];
    char   y[3];
    //int    best_move_i = INVALID;
    //int    best_move_j = INVALID;
    int    value;

    value = ( color == BLACK ) ? INT_MIN : INT_MAX;

    node_count = 0;

    if ( do_log ) {
        log_file = fopen( LOG_FILE, "w" );
    }

    // Steps for root node:
    // 1. Get pseudo valid moves
    // 2. Get list valid moves
    // 3. Go through move list
    //      4. Make move
    //      ....
    //      5. Undo move
 
    (void) time(&start);

    // Get list of pseudo valid moves:
    nr_of_valid_moves = get_pseudo_valid_move_list( color, valid_moves );
    // Remove zero liberty moves from pseudo valid moves:
    nr_of_valid_moves = get_valid_move_list( color, nr_of_valid_moves, valid_moves );


    // Go through move list:
    for ( k = 0; k < nr_of_valid_moves; k++ ) {
        i = valid_moves[k][0];
        j = valid_moves[k][1];

        // Make move:
        node_count++;
        //printf( "# Level: %d make: %d,%d value: %d\n", tree_level, i, j, value );
        make_move( color, i, j );

        // Start recursion:
        valid_moves[k][2] = add_node( color * -1, tree_level );

        if ( color == BLACK ) {
            // For black: remember highest value
            if ( valid_moves[k][2] > value ) {
                value = valid_moves[k][2];
                //best_move_i = i;
                //best_move_j = j;
            }
        }
        else {
            // For white: remember lowest value
            if ( valid_moves[k][2] < value ) {
                value = valid_moves[k][2];
                //best_move_i = i;
                //best_move_j = j;
            }
        }

        if ( do_log ) {
            i_to_x( i, x );
            j_to_y( j, y );
            fprintf( log_file, "%s%s (%d)\n", x, y, valid_moves[k][2] );
        }

        // Undo move:
        //printf( "# Level: %d undo: %d,%d value: %d\n", tree_level, i, j, value );
        undo_move();
    }

    // Sort move list by value:
    if ( color == BLACK ) {
        qsort( valid_moves, (size_t)nr_of_valid_moves, sizeof(valid_moves[0]), compare_value_black );
    }
    else {
        qsort( valid_moves, (size_t)nr_of_valid_moves, sizeof(valid_moves[0]), compare_value_white );
    }

    (void) time(&stop);

    diff_time = stop - start;
    if ( diff_time == 0 ) {
        diff_time = 1;
    }
    //printf( "#### Node count: %llu ####\n", node_count );
    //printf( "Level:      %d\n", search_level );
    //printf( "Duration:   %ld\n", stop - start );
    //printf( "Nodes/sec.: %llu\n", node_count / diff_time );

    *i_selected = valid_moves[0][0];
    *j_selected = valid_moves[0][1];
    // *i_selected = best_move_i;
    // *j_selected = best_move_j;

    if ( log_file != NULL ) {
        fclose(log_file);
    }

    return;
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

    set_vertex( color, i, j );
    create_groups();
    count_liberties();
    set_groups_size();
    nr_of_removed_stones = remove_stones( color * -1 );
    if ( nr_of_removed_stones > 0 ) {
        create_groups();
        count_liberties();
        set_groups_size();
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
 * @brief       Adds a new node to the move tree.
 *
 * A new node is added to the move tree. For every move in the generated move
 * list, a new node is added recursively. If a certain level is reached, the
 * recusrion is stopped.
 *
 * @param[in]   color       Color of move to set.
 * @param[in]   tree_level  Counter that shows the level in the move tree.
 * @return      Value of position
 */
int add_node( int color, int tree_level )
{
    int  k, l;
    int  i, j;
    int  valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][3];
    int  nr_of_valid_moves;
    int  value;
    char x[2];
    char y[3];
    char indent[10];

    value = ( color == BLACK ) ? INT_MIN : INT_MAX;

    if ( tree_level == search_level ) {
        value = evaluate_position();

        return value;
    }
    tree_level++;

    // Get list of pseudo valid moves:
    nr_of_valid_moves = get_pseudo_valid_move_list( color, valid_moves );
    // Remove zero liberty moves from pseudo valid moves:
    nr_of_valid_moves = get_valid_move_list( color, nr_of_valid_moves, valid_moves );


    // Go through move list:
    for ( k = 0; k < nr_of_valid_moves; k++ ) {
        i = valid_moves[k][0];
        j = valid_moves[k][1];
        // Make move:
        node_count++;
        //printf( "# Level: %d make: %d,%d\n", tree_level, i, j );
        make_move( color, i, j );

        // Start recursion:
        valid_moves[k][2] = add_node( color * -1, tree_level );

        if ( color == BLACK ) {
            // For black: remember highest value
            if ( valid_moves[k][2] > value ) {
                value = valid_moves[k][2];
            }
        }
        else {
            // For white: remember lowest value
            if ( valid_moves[k][2] < value ) {
                value = valid_moves[k][2];
            }
        }

        if ( do_log ) {
            i_to_x( i, x );
            j_to_y( j, y );
            indent[0] = '\0';
            for ( l = 1; l <= tree_level; l++ ) {
                strcat( indent, "\t" );
            }
            fprintf( log_file, "%s%s%s (%d)\n", indent, x, y, valid_moves[k][2] );
        }

        // Undo move:
        //printf( "# Level %d undo: %d,%d\n", tree_level, i, j );
        undo_move();
    }

    // Sort move list by value:
    /*
    if ( color == BLACK ) {
        qsort( valid_moves, (size_t)nr_of_valid_moves, sizeof(valid_moves[0]), compare_value_black );
    }
    else {
        qsort( valid_moves, (size_t)nr_of_valid_moves, sizeof(valid_moves[0]), compare_value_white );
    }
    */

    //value = valid_moves[0][2];

    return value;
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

