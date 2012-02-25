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
