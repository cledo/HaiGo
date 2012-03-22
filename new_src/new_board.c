#include <stdlib.h>
#include <stdio.h>
#include "../src/global_const.h"
#include "new_board.h"
#include "new_board_intern.h"

typedef unsigned long row_t;

bsize_t board_size = BOARD_SIZE_DEFAULT;

row_t *board_black;
row_t *board_white;
row_t *board_off;


void init_board( bsize_t board_size )
{
    row_t i;

    set_board_size(board_size);

    // Add a lower and an upper boundary row; therefore we take board_size + 2
    board_black = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    board_white = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    board_off   = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    if ( board_black == NULL || board_white == NULL ) {
        fprintf( stderr, "cannot allocate memory\n" );
        exit(1);
    }

    for ( i = 0; i < board_size + 2; i++ ) {
        board_black[i] = 0;
        board_white[i] = 0;
        board_off[i]   = 0;
    }

    return;
}


void free_board(void)
{
    free(board_black);
    free(board_white);
    free(board_off);

    board_black = NULL;
    board_white = NULL;
    board_off   = NULL;

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

void set_vertex( int color, int i, int j )
{
    int   J = j + 1;
    row_t I = 0x80000000 >> ( i + 1 );    // Bit 31 set to 1 is 0x80000000

    switch (color) {
        case BLACK:
            board_black[J] |= I;
            break;
        case WHITE:
            board_white[J] |= I;
            break;
        case EMPTY:
            board_black[J] &= ~I;
            board_white[J] &= ~I;
            break;
        default:
            break;
    }

    return;
}

int get_vertex( int i, int j )
{
    int color;
    int   J = j + 1;
    row_t I = 0x80000000 >> ( i + 1 );  // Bit 31 set to 1 is 0x80000000

    if ( board_black[J] & I ) {
        color = BLACK;
    }
    else if ( board_white[J] & I ) {
        color = WHITE;
    }
    else {
        color = EMPTY;
    }

    return color;
}

void get_board_as_string( char board_output[] )
{

    return;
}

