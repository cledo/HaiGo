#include "global_const.h"
#include "board.h"
#include "brains.h"

/**
 * @file    brains.c
 *
 * @brief   Contains functions that evaluate position on different topic.
 *
 * Contains different functions that evaluate the current position depending
 * on different factors, for example on captured stones, groups being in
 * atari, and so on.
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


/**
 * @brief       Determines value depending on atari.
 *
 * The returned value is determined by the number and size of groups bein in
 * atari.
 *
 * @return      Value of position
 */
int brain_atari(void)
{
    int k;
    int value             = 0;
    int count_atari_white = 0;
    int count_atari_black = 0;
    
    for ( k = 0; k <= get_last_group_nr(WHITE) * -1; k++ ) {
        if ( get_nr_of_liberties( k * -1 ) == 1 ) {
            count_atari_white += get_size_of_group( k * -1 );
        }
    }

    for ( k = 0; k <= get_last_group_nr(BLACK); k++ ) {
        if ( get_nr_of_liberties(k) == 1 ) {
            count_atari_black += get_size_of_group(k);
        }
    }

    value = count_atari_white - count_atari_black;

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
    int i, j;
    int value      = 0;
    int board_size = get_board_size();

    for ( i = 0; i < board_size; i ++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( is_hoshi( i, j ) ) {
                value += get_vertex( i, j );
            }
        }
    }

    return value;
}

