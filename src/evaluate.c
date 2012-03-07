#include "brains.h"
#include "evaluate.h"


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

