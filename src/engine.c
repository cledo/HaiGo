#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "global_const.h"
#include "engine.h"
#include "board.h"
#include "move.h"


/**
 * @file    engine.c
 *
 * @brief   Engine for building the move tree.
 *
 * [Multi-line
 * description still missing ...]
 *
 */

static unsigned long long int node_count;

void add_node( int color, int tree_level );
void add_move( int color, int i, int j );
void undo_move(void);

/**
 * @brief       Builds move tree.
 *
 * Builds a complete move tree recursively.
 *
 * @param[in]   color   Color to move
 * @return      Nothing
 * @note        This function currently only serves as a proof of concept.
 * @todo        Must think about proper parameters ...
 */
void build_tree( int color )
{
    int k;
    int i, j;
    int valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int nr_of_valid_moves;
    int tree_level = 0;
    time_t start;
    time_t stop;

    node_count = 0;

    // Steps for root node:
    // 1. Get pseudo valid moves
    // 2. Get list valid moves
    // 3. Go through move list
    //      4. Make move
    //      ....
    //      5. Undo move
 
    time(&start);

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
        add_move( color, i, j );

        // Start recursion:
        add_node( color * -1, tree_level );

        // Undo move:
        //printf( "# Level: %d undo: %d,%d\n", tree_level, i, j );
        undo_move();

    }

    time(&stop);

    printf( "#### Node count: %lld ####\n", node_count );
    printf( "Duration: %ld\n", stop - start );
    printf( "Nodes/sec.: %lld\n", node_count / ( stop - start ) );

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
void add_move( int color, int i, int j )
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
 * @sa          add_move()
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
 * @return      Nothing
 */
void add_node( int color, int tree_level )
{
    int k;
    int i, j;
    int valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int nr_of_valid_moves;

    if ( tree_level == 2 ) {
        return;
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
        add_move( color, i, j );

        // Start recursion:
        add_node( color * -1, tree_level );

        // Undo move:
        //printf( "# Level %d undo: %d,%d\n", tree_level, i, j );
        undo_move();

    }


    return;
}

