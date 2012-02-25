#include "brains.h"
#include "evaluate.h"

int evaluate_position(void)
{
    int value;

    value = brain_capture();
    value += brain_atari();

    return value;
}

