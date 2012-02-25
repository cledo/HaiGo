#include "brains/all_brains.h"
#include "evaluate.h"

int evaluate_position(void)
{
    int value;

    value = brain_capture();

    return value;
}

