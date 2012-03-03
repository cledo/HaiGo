#include "board.h"
#include "brains.h"
#include "evaluate.h"


int evaluate_position( int value_list[] )
{
    int value  = 0;

    value_list[0] = brain_capture()       * 78;
    value_list[1] = brain_atari()         * 15;
    value_list[2] = brain_avg_liberties() * 1;
    value_list[3] = 0;
    //value += brain_kosumi()        * 1;
    /*
    if ( kosumi > 0 ) {
        value++;
    }
    else if ( kosumi < 0 ) {
        value--;
    }
    */
    //value += brain_edge_stones()   * 3;
    //value += brain_hoshi_stones();

    value = value_list[0] + value_list[1] + value_list[2] + value_list[3];

    return value;
}

