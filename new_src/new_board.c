#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
    int i;      // Index for x-axis
    int j;      // Index for y-axis
    char x[2];  // Label for x-axis
    char y[3];  // Label for y-axis

    char buffer[128];

    // Line number where captured stones are shown:
    int line_show_white = 1;
    int line_show_black = 0;
    if ( board_size > 10 ) {
        line_show_white = board_size - 9;
        line_show_black = board_size - 10;
    }


    board_output[0] = '\0';
    strcat( board_output, "\n" );

    /* Print uppercase letters above the board */
    strcat( board_output, "   " );
    for ( i = 0; i < board_size; i++ ) {
        get_label_x( i, x );
        strcat( board_output, " " );
        strcat( board_output, x );
    }
    strcat( board_output, "\n" );

    for ( j = board_size - 1; j >= 0; j-- ) {

        /* Print numbers left of board */
        get_label_y_left( j, y );
        strcat( board_output, " " );
        strcat( board_output, y );

        /* Print board fields */
        for ( i = 0; i < board_size; i++ ) {
            strcat( board_output, " " );
            switch ( get_vertex( i, j ) ) {
                case WHITE:
                    strcat( board_output, WHITE_STONE );
                    break;
                case BLACK:
                    strcat( board_output, BLACK_STONE );
                    break;
                case EMPTY:
                    switch ( is_hoshi( i, j ) ) {
                        case true:
                            strcat( board_output, FIELD_HOSHI );
                            break;
                        case false:
                            strcat( board_output, FIELD_EMPTY );
                            break;
                    }
                    break;
            }
        }

        /* Print numbers right of board */
        get_label_y_right( j, y );
        strcat( board_output,  " " );
        strcat( board_output,    y );

        /* Show number of captured stones */
        if ( j == line_show_white ) {
            snprintf( buffer, 128, "\t    WHITE (%s) has captured %d stones", WHITE_STONE, get_white_captured() );
            strcat( board_output, buffer );
        }
        if ( j == line_show_black ) {
            snprintf( buffer, 128, "\t    BLACK (%s) has captured %d stones", BLACK_STONE, get_black_captured() );
            strcat( board_output, buffer );
        }

        strcat( board_output, "\n" );
    }

    /* Print uppercase letters below board */
    strcat( board_output, "   " );
    for ( i = 0; i < board_size; i++ ) {
        get_label_x( i, x );
        strcat( board_output, " " );
        strcat( board_output, x );
    }

    return;
}
void get_label_x( int i, char x[] )

{
    if ( i >= 8 ) {
        i++;
    }
    i += 65;
    x[0] = (char) i;
    x[1] = '\0';

    return;
}

void get_label_y_left( int j, char y[] )
{
    j++;

    y[0] = (char)(int)( j / 10 + 48 );
    y[1] = (char)( j % 10 + 48 );
    y[2] = '\0';
    if ( y[0] == '0' ) {
        y[0] = ' ';
    }

    return;
}

void get_label_y_right( int j, char y[] )
{
    j++;

    y[0] = (char)(int)( j / 10 + 48 );
    y[1] = (char)( j % 10 + 48 );
    y[2] = '\0';
    if ( y[0] == '0' ) {
        y[0] = y[1];
        y[1] = '\0';
    }

    return;
}

int get_white_captured(void)
{
    return 0;
}

int get_black_captured(void)
{
    return 0;
}

bool is_hoshi( int i, int j )
{
    return false;
}

