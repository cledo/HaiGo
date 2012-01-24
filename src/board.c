#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "global_const.h"
#include "board.h"


int  **board;
bool **hoshi;

int board_size = 0;

void get_label_x( int i, char x[] );
void get_label_y_left( int i, char x[] );
void get_label_y_right( int j, char y[] );
bool is_hoshi( int i, int j );
//void set_vertex( int color, int i, int j );

/**
 *  @brief Allocates memory for all board data structures.
 *
 *  Allocates memory for the data structures board and hoshi. Its sets the
 *  board fields to EMPTY and sets the correct hoshi points depending on board
 *  size.
 *
 *  @param[in]  wanted_board_size  Integer of intended board size
 *  @return     nothing
 *  @sa         [n/a]
 *
 *  @todo   A size check has to be implemented probably.
 */
void init_board( int wanted_board_size )
{
    int i, j;

    board_size = wanted_board_size;

    board = malloc( board_size * sizeof(int *) );
    hoshi = malloc( board_size * sizeof(bool *) );
    if ( board == NULL || hoshi == NULL ) {
        fprintf( stderr, "Failed to malloc memory");
        exit(EXIT_FAILURE);
    }

    for ( i = 0; i < board_size; i++ ) {
        board[i] = malloc( board_size * sizeof(int) );
        hoshi[i] = malloc( board_size * sizeof(bool) );
        if ( board[i] == NULL || hoshi[i] == NULL ) {
            fprintf( stderr, "Failed to malloc memory");
            exit(EXIT_FAILURE);
        }

        for ( j = 0; j < board_size; j++ ) {
            board[i][j] = EMPTY;
            hoshi[i][j] = false;
        }
    }

    switch (board_size) {
        case 19:
            hoshi[3][3]   = true;
            hoshi[3][9]   = true;
            hoshi[3][15]  = true;
            hoshi[9][3]   = true;
            hoshi[9][9]   = true;
            hoshi[9][15]  = true;
            hoshi[15][3]  = true;
            hoshi[15][9]  = true;
            hoshi[15][15] = true;
            break;
        case 13:
            hoshi[3][3] = true;
            hoshi[3][9] = true;
            hoshi[9][3] = true;
            hoshi[9][9] = true;
            hoshi[6][6] = true;
            break;
        case 9:
            hoshi[2][2] = true;
            hoshi[2][6] = true;
            hoshi[6][2] = true;
            hoshi[6][6] = true;
            hoshi[4][4] = true;
            break;
    }

    return;
}

/*******************************************************************/
/*  Usage     : free_board()                                       */
/*  Purpose   : Frees the memory allocated for board and hoshi     */
/*  Parameter : none                                               */
/*  Returns   : nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : none                                               */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void free_board(void) {
    int i;

    for ( i = 0; i < board_size; i++ ) {
        free(board[i]);
        free(hoshi[i]);
    }
    free(board);
    free(hoshi);

    board = NULL;
    hoshi = NULL;

    return;
}

/*******************************************************************/
/*  Usage     : get_board_as_string()                              */
/*  Purpose   : Returns a printable represantation of the board    */
/*  Parameter : none                                               */
/*  Returns   : String of complete board                           */
/*  Throws    : Nothing                                            */
/*  Comment   : This should look exactly what is shown by GnuGo    */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void get_board_as_string( char board_output[] ) {
    int i; // Index for x-axis
    int j; // Index for y-axis
    char x[3];      // Label for x-axis
    char y[2];      // Label for y-axis

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
            switch ( board[i][j] ) {
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

void get_label_x( int i, char x[] ) {

    if ( i >= 8 ) {
        i++;
    }
    i += 65;
    x[0] = (char) i;
    x[1] = '\0';

    return;
}

void get_label_y_left( int j, char y[] ) {

    j++;

    y[0] = (char)(int)( j / 10 + 48 );
    y[1] = (char)( j % 10 + 48 );
    y[2] = '\0';
    if ( y[0] == '0' ) {
        y[0] = ' ';
    }

    return;
}

void get_label_y_right( int j, char y[] ) {

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

bool is_hoshi( int i, int j ) {

    return hoshi[i][j];
}

int get_board_size(void) {

    return board_size;
}

void set_vertex( int color, int i, int j ) {

    board[i][j] = color;

    return;
}

