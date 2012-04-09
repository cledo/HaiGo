#include <stdlib.h> // DEBUG
#include <stdio.h>  // DEBUG
#include "global_const.h"
#include "board.h"
#include "evaluate.h"

/**
 * @file    evaluate.c
 *
 * @brief   Wraps all the different evaluating functions.
 *
 * Evaluates the board position.
 *
 */


/**
 * @brief   Struct that combines brain function with its multiply factor.
 *
 **/
typedef struct {
    int (*function)();  //!< Pointer to brain function.
    int factor;         //!< Multiply value to combine with brain function.
    int limit;          //!< Sets a limit to the brain value.
} brain_t;

static brain_t brains[COUNT_BRAINS];    //!< List of all brain functions.

int brain_capture(void);

// ----- Deactivated for now ------
int brain_atari(void);
int brain_avg_liberties(void);
int brain_edge_stones(void);
int brain_hoshi_stones(void);
// --------------------------------

/**
 * @brief       Initialises brains data structure.
 *
 * Creates an array of brain_t structs, which combines brain functions with a
 * multiply factor.
 *
 * @return      Nothing
 */
void init_brains(void)
{
    int i = 0;

    brains[i].function = (*brain_capture);
    brains[i].factor   = 1;
    brains[i++].limit  = 0;

    brains[i].function = (*brain_atari);
    brains[i].factor   = 0;
    brains[i++].limit  = 0;

    brains[i].function = (*brain_avg_liberties);
    brains[i].factor   = 0;
    brains[i++].limit  = 0;

    brains[i].function = (*brain_edge_stones);
    brains[i].factor   = 0;
    brains[i++].limit  = 0;

    brains[i].function = (*brain_hoshi_stones);
    brains[i].factor   = 0;
    brains[i++].limit  = 0;

    return;
}

/**
 * @brief       Sets factor for brain function.
 *
 * Sets the factor for the given brain identified by index.
 *
 * @param[in]   index   Index of brain in brains array.
 * @param[in]   factor  Value for the factor.
 */
void set_factor( int index, int factor )
{
    brains[index].factor = factor;

    return;
}

/**
 * @brief       Gets factor of brain function.
 *
 * Returns value of factor of given brain index.
 *
 * @param[in]   index   Index of brain in brains array.
 * @return      Value of the factor.
 */
int get_factor( int index )
{

    return brains[index].factor;
}

/**
 * @brief       Evaluates a position.
 *
 * Evaluates a position on the board by calling all the different brain_*
 * functions and returns a single value.
 *
 * @param[out]  value_list  List of different value parts (necessary for debugging).
 * @param[in]   do_full_eval    If true, does full evaluation.
 * @return      Value of position
 * @note        Full evaluation (with do_eval_full set to true) should not be
 *              done for move ordering, only at the end of the search tree.
 * @todo        Make sure that do_full_eval == false does not evaluate all
 *              functions!
 */
int evaluate_position( int value_list[], bool do_full_eval )
{
    int k;
    int value = 0;

    // TEST:
    if ( do_full_eval ) {
        scan_board_2();
    }

    for ( k = 0; k < COUNT_BRAINS; k++ ) {
        if ( brains[k].factor == 0 ) {
            value_list[k] = 0;
            continue;
        }

        value_list[k] = brains[k].function() * brains[k].factor;

        // Limit value:
        if ( brains[k].limit && value_list[k] > 0 && value_list[k] > brains[k].limit ) {
            value_list[k] = brains[k].limit;
        }
        else if ( brains[k].limit && value_list[k] < 0 && value_list[k] < brains[k].limit * -1 ) {
            value_list[k] = brains[k].limit * -1;
        }

        value += value_list[k];
    }

    return value;
}

/**
 * @brief       Determines value of captured stones.
 *
 * The returned value is determined by the difference of the number of
 * captured stones by black and white.
 *
 * @return      Value of position
 */
int brain_capture(void)
{
    int value = get_black_captured() - get_white_captured();

    return value;
}


/**
 * @brief       Determines value depending on atari.
 *
 * The returned value is determined by the number and size of groups being in
 * atari.
 *
 * @return      Value of position
 */
int brain_atari(void)
{
    //int k;
    int value             = 0;
    //int count_atari_white = 0;
    //int count_atari_black = 0;

    /*
    int last_white_group = get_last_group_nr(WHITE) * -1;
    int last_black_group = get_last_group_nr(BLACK);
    
    for ( k = 0; k <= last_white_group; k++ ) {
        if ( get_nr_of_liberties( k * -1 ) == 1 ) {
            count_atari_white += get_size_of_worm( k * -1 );
        }
    }

    for ( k = 0; k <= last_black_group; k++ ) {
        if ( get_nr_of_liberties(k) == 1 ) {
            count_atari_black += get_size_of_worm(k);
        }
    }

    value = count_atari_white - count_atari_black;
    */

    return value;
}

/**
 * @brief       Counts stones on edge.
 *
 * The returned value is determined by the number of stones on the edge of the
 * board.
 *
 * @return      Value of position
 */
int brain_edge_stones(void)
{
    int i, j;
    int value       = 0;
    int board_size  = get_board_size();

    for ( i = 0; i < board_size; i++ ) {
        value += get_vertex( i, 0 );
        value += get_vertex( i, board_size - 1 );
    }
    for ( j = 0; j < board_size; j++ ) {
        value += get_vertex( 0, j );
        value += get_vertex( board_size - 1, j );
    }

    return value * -1;
}

/**
 * @brief       Evaluates stones on star points
 *
 * Stones on star points get extra value.
 *
 * @return      Value of position
 * @todo        This should probably be only done in the opening phase.
 */
int brain_hoshi_stones(void)
{
    //int i, j;
    int value      = 0;
    /*
    int board_size = get_board_size();

    for ( i = 0; i < board_size; i ++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( is_hoshi( i, j ) ) {
                value += get_vertex( i, j );
            }
        }
    }
    */

    return value;
}

/**
 * @brief       Returns value depending on average liberties per group.
 *
 * The number of liberties is divided by the number of groups for black and
 * white. The average for black minus the average for white is the returned
 * value.
 *
 * @return      Black average minus white average.
 */
int brain_avg_liberties(void)
{
    int value;
    /*
    int value_black;
    int value_white;
    int count_groups_black;
    int count_groups_white;
    int count_liberties_black;
    int count_liberties_white;

    count_groups_black = get_last_group_nr(BLACK);
    count_groups_white = get_last_group_nr(WHITE) * -1;
    count_liberties_black = get_group_count_liberties(BLACK);
    count_liberties_white = get_group_count_liberties(WHITE);

    if ( count_groups_black == 0 ) {
        count_groups_black = 1;
    }
    if ( count_groups_white == 0 ) {
        count_groups_white = 1;
    }

    value_black = count_liberties_black / count_groups_black;
    value_white = count_liberties_white / count_groups_white;
    */

    /*
    if ( value_black > 4 ) {
        value_black = 4;
    }
    if ( value_white > 4 ) {
        value_white = 4;
    }
    */

    //value = value_black - value_white;
    value = 0;

    return value;
}

