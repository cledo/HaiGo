#include <stdlib.h>
#include <time.h>
#include "random.h"

/**
 * @file    brains/random.c
 *
 * @brief   Brain that chooses a random move.
 *
 * From the given list of valid moves a
 * random move is chosen.
 *
 */

/**
 * @brief       Selects random move
 *
 * Selects a random move vertex from the given list of valid moves. The
 * estimated value of this move is returned.
 *
 * @param[out]  i   Pointer to horizontal coordinate
 * @param[out]  j   Pointer to vertical coordinate
 * @param[in]   valid_moves The vertex list of all valid moves in current position
 * @param[in]   nr_of_valid_moves   Number of elements in list of valid moves
 * @return      Estimated value of selected move
 */
int brain_random( int *i, int *j, int valid_moves[][3], int nr_of_valid_moves )
{
    int move_value = 0;
    int rand_index;

    srand( (unsigned) time(NULL) );
    rand_index = rand() % nr_of_valid_moves;
    *i = valid_moves[rand_index][0];
    *j = valid_moves[rand_index][1];

    return move_value;
}

