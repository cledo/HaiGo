#include "board.h"
#include "evaluate.h"

int evaluate_position(void)
{
    int value;

    value = get_black_captured() - get_white_captured();

    return value;
}

