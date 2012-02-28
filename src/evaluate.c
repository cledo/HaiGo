#include "board.h"
#include "brains.h"
#include "evaluate.h"


int evaluate_position(void)
{
    int value;

    value =  brain_capture();
    //value += brain_atari()   * 10;
    //value += brain_edge_stones();
    //value += brain_hoshi_stones();

    return value;
}

