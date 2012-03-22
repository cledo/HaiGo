#include <stdio.h>
#include <stdlib.h>
#include "new_board.h"
#include "../src/global_const.h"

int main(void)
{
    bsize_t board_size = 19;

    printf( "unsigned long: %d\n", sizeof(unsigned long));

    init_board(board_size);

    set_vertex( BLACK, 0, 0 );

    free_board();

    return EXIT_SUCCESS;
}

