#include <stdlib.h>
#include "new_board.h"

int main(void)
{
    bsize_t board_size = 19;

    init_board(board_size);

    free_board();

    return EXIT_SUCCESS;
}

