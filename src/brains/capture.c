#include "../board.h"
#include "capture.h"

/**
 * @file    brains/capture.c
 *
 * @brief   Brain that chooses capturing moves.
 *
 * Moves that capture stones, or moves that avoid being captured are evaluated
 * with a higher score.
 *
 */

/**
 * @brief       Determines value on captured stones.
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

