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
 * The returned value is determined by the number and size of groups being in
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

    int last_white_group = get_last_group_nr(WHITE) * -1;
    int last_black_group = get_last_group_nr(BLACK);
    
    for ( k = 0; k <= last_white_group; k++ ) {
        if ( get_nr_of_liberties( k * -1 ) == 1 ) {
            count_atari_white += get_size_of_group( k * -1 );
        }
    }

    for ( k = 0; k <= last_black_group; k++ ) {
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

/**
 * @brief       returns value depending on average liberties per group.
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

    if ( value_black > 4 ) {
        value_black = 4;
    }
    if ( value_white > 4 ) {
        value_white = 4;
    }

    value = value_black - value_white;

    return value;
}

/**
 * @brief       Returns value based on number of kosumis.
 *
 * The number of kosumis for black and white are counted. The returned value
 * is number of black kosumis minus number of white kosumis.
 *
 * @return      Difference in number of kosumis
 */
int brain_kosumi(void)
{
    int value;
    int i, j;
    int count_kosumi_black = 0;
    int count_kosumi_white = 0;
    int board_size = get_board_size();

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++) {
            if ( get_vertex( i, j ) == BLACK ) {
                // North-east:
                if ( ( i + 1 < board_size - 1 ) && ( j + 1 < board_size - 1 ) ) {
                    if ( get_vertex( i+1, j+1 ) == BLACK &&  get_vertex( i, j+1 ) != BLACK && get_vertex( i+1, j ) != BLACK ) {
                        count_kosumi_black++;
                    }
                }
                // South-east:
                if ( ( i + 1 < board_size - 1 ) && ( j - 1 >= 0 ) ) {
                    if ( get_vertex( i+1, j-1 ) == BLACK && get_vertex( i, j-1 ) != BLACK && get_vertex( i+1, j ) != BLACK ) {
                        count_kosumi_black++;
                    }
                }
                // South-west:
                if ( ( i - 1 >= 0 ) && ( j - 1 >= 0 ) ) {
                    if ( get_vertex( i-1, j-1 ) == BLACK && get_vertex( i, j-1 ) != BLACK && get_vertex( i-1, j ) != BLACK ) {
                        count_kosumi_black++;
                    }
                }
                // North-west:
                if ( ( i - 1 >= 0 ) && ( j + 1 < board_size - 1 ) ) {
                    if ( get_vertex( i-1, j+1 ) == BLACK && get_vertex( i, j+1 ) != BLACK && get_vertex( i-1, j ) != BLACK ) {
                        count_kosumi_black++;
                    }
                }
            }
            else if ( get_vertex( i, j ) == WHITE ) {
                // North-east:
                if ( ( i + 1 < board_size - 1 ) && ( j + 1 < board_size - 1 ) ) {
                    if ( get_vertex( i+1, j+1 ) == WHITE && get_vertex( i, j+1 ) != WHITE && get_vertex( i+1, j ) != WHITE ) {
                        count_kosumi_white++;
                    }
                }
                // South-east:
                if ( ( i + 1 < board_size - 1 ) && ( j - 1 >= 0 ) ) {
                    if ( get_vertex( i+1, j-1 ) == WHITE && get_vertex( i, j-1 ) != WHITE && get_vertex( i+1, j ) != WHITE ) {
                        count_kosumi_white++;
                    }
                }
                // South-west:
                if ( ( i - 1 >= 0 ) && ( j - 1 >= 0 ) ) {
                    if ( get_vertex( i-1, j-1 ) == WHITE && get_vertex( i, j-1 ) != WHITE && get_vertex( i-1, j ) != WHITE ) {
                        count_kosumi_white++;
                    }
                }
                // North-west:
                if ( ( i - 1 >= 0 ) && ( j + 1 < board_size - 1 ) ) {
                    if ( get_vertex( i-1, j+1 ) == WHITE && get_vertex( i, j+1 ) != WHITE && get_vertex( i-1, j ) != WHITE ) {
                        count_kosumi_white++;
                    }
                }
            }
        }
    }

    value = ( count_kosumi_black / 2 ) - ( count_kosumi_white / 2 );

    return value;
}
