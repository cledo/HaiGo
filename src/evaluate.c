#include "brains.h"
#include "evaluate.h"

/**
 * @file    evaluate.c
 *
 * @brief   Wraps all the different evaluating functions.
 *
 * [Multi-line
 * description still missing ...]
 *
 * @todo Maybe this should be put into brain.c.
 *
 */

/**
 * @brief       Evaluates a position.
 *
 * Evaluates a position on the board by calling all the different brain_*
 * functions and returns a single value.
 *
 * @param[out]  value_list  List of different value parts (necessary for * debugging).
 * @return      Value of position
 */
int evaluate_position( int value_list[] )
{
    int value = 0;

    value_list[0] = brain_capture()       * 82;
    value_list[1] = brain_atari()         * 15;
    value_list[2] = brain_avg_liberties() * 1;
    value_list[3] = brain_kosumi()        * 1;
    value_list[4] = brain_edge_stones()   * 1;
    value_list[5] = brain_chains();
    value_list[6] = 0;

    value = value_list[0] + value_list[1] + value_list[2]
        + value_list[3] + value_list[4] + value_list[5] + value_list[6];

    return value;
}

