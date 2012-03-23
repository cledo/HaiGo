#include <stdio.h>
#include <stdlib.h>
#include "new_board.h"
#include "../src/global_const.h"

int main(void)
{
    bsize_t board_size = 19;

    char board_output[MAX_OUTPUT_LENGTH];

    printf( "unsigned long: %d\n", sizeof(unsigned long));

    init_board(board_size);

    set_vertex( BLACK, 0, 0 );
    set_vertex( WHITE, 18, 18 );

    get_board_as_string(board_output);
    printf( "%s\n", board_output );

    free_board();

    return EXIT_SUCCESS;
}

