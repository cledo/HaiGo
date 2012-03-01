#include "board.h"
#include "brains.h"
#include "evaluate.h"


int evaluate_position(void)
{
    int value = 0;

    value =  brain_capture() * 4;
    value += brain_atari();
    //value += brain_edge_stones();
    //value += brain_hoshi_stones();

    return value;
}

