#include <stdlib.h>
#include <stdio.h>
#include "../src/global_const.h"
#include "new_board.h"
#include "new_board_intern.h"


bsize_t board_size = BOARD_SIZE_DEFAULT;

unsigned *board;


void init_board( bsize_t board_size )
{
    set_board_size(board_size);

    board = malloc( (size_t)( board_size * sizeof(unsigned) ) );

    return;
}


void free_board(void)
{
    free(board);

    board = NULL;

    return;
}


void set_board_size( bsize_t size )
{
    if ( size < BOARD_SIZE_MIN || size > BOARD_SIZE_MAX ) {
        fprintf( stderr, "set_board_size() has been given invalid size: %u\n", size );
        exit(1);
    }

    board_size = size;

    return;
}

bsize_t get_board_size(void)
{
    return board_size;
}

