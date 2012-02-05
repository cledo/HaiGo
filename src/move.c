#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "global_const.h"
#include "move.h"


int move_number = 0;

/**
 * @brief   Structure that represents a move.
 *
 **/
struct move {
    int  number;    //!< The number of the move in the move history.
    bool pass;      //!< Indicates whether this move is a pass os not.
    int  i;         //!< The horizontal coordinate of the vertex.
    int  j;         //!< The vertical coordinate of the vertex.
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];    //!< List of captured stones; In move zero: List of handicap stones.
} next_move;


int create_next_move(void)
{
    int k;
    
    next_move.number = ++move_number;
    next_move.pass   = true;
    next_move.i      = -1;
    next_move.j      = -1;
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        next_move.stones[k][0] = -1;
        next_move.stones[k][0] = -1;
    }

    return next_move.number;
}

void set_move_vertex( int i, int j )
{
    next_move.i = i;
    next_move.j = j;
    next_move.pass = false;

    return;
}

void set_move_captured_stones( int captured_stones[][2] )
{
    int k = 0;

    while ( captured_stones[k][0] != -1 ) {
        next_move.stones[k][0] = captured_stones[k][0];
        next_move.stones[k][1] = captured_stones[k][1];
        k++;
    }
    next_move.stones[k][0] = -1;
    next_move.stones[k][1] = -1;

    return;
}

void print_move(void)
{
    int k = 0;

    printf( "# move nr.: %d\n", next_move.number );
    printf( "#    pass: %s\n", ( next_move.pass == true ) ? "true" : "false");
    printf( "#    %d,%d\n", next_move.i, next_move.j );
    while ( next_move.stones[k][0] != -1 ) {
        printf( "#    captrd %d: %d,%d\n", k+1, next_move.stones[k][0], next_move.stones[k][1] );
        k++;
    }

    return;
}



