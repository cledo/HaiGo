#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "global_const.h"
#include "board.h"


/* State variables */
// As describes in GTP v2.0 chapter 5.1
int  **board;
int  **group;
bool **hoshi;
int board_size     = 0;
int black_cpatured = 0;
int white_captured = 0;

static void get_label_x( int i, char x[] );
static void get_label_y_left( int i, char x[] );
static void get_label_y_right( int j, char y[] );
static bool is_hoshi( int i, int j );   // This will be needed as extern maybe ..

int has_neighbour( int i, int j, int neighbour[][2] );
int get_free_group( int color );

/**
 *  @brief Allocates memory for all board data structures.
 *
 *  Allocates memory for the data structures board, group, and hoshi. Its sets the
 *  board and group fields to EMPTY, and sets the correct hoshi points depending on board
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

    black_cpatured = 0;
    white_captured = 0;

    board_size = wanted_board_size;

    board = malloc( board_size * sizeof(int *) );
    group = malloc( board_size * sizeof(int *) );
    hoshi = malloc( board_size * sizeof(bool *) );

    if ( board == NULL || hoshi == NULL || group == NULL ) {
        fprintf( stderr, "Failed to malloc memory");
        exit(EXIT_FAILURE);
    }

    for ( i = 0; i < board_size; i++ ) {
        board[i] = malloc( board_size * sizeof(int) );
        group[i] = malloc( board_size * sizeof(int) );
        hoshi[i] = malloc( board_size * sizeof(bool) );
        if ( board[i] == NULL || hoshi[i] == NULL || group[i] == NULL ) {
            fprintf( stderr, "Failed to malloc memory");
            exit(EXIT_FAILURE);
        }

        for ( j = 0; j < board_size; j++ ) {
            board[i][j] = EMPTY;
            group[i][j] = EMPTY;
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

/**
 * @brief Frees the memory allocated for all board data structures.
 *
 * Frees the memory which has been allocated for the board, group, and hoshi data
 * structures.
 *
 * @return  nothing
 * @sa      init_board()
 * @note    The pointers to board, group, and hoshi are also set to NULL, so a check
 *          whether the pointers are still valid or not is possible.
 */
void free_board(void)
{
    int i;

    for ( i = 0; i < board_size; i++ ) {
        free( board[i] );
        free( group[i] );
        free( hoshi[i] );
    }
    free(board);
    free(group);
    free(hoshi);

    board = NULL;
    group = NULL;
    hoshi = NULL;

    return;
}

/**
 * @brief   Constructs ASCII board
 *
 * Constructs a complete ASCII board with coordinates, depending on the current boardsize, ready for printing.
 *
 * @param[out]  board_output    String representation of board
 * @return      nothing
 * @note        This function only constructs the board string. It does not
 *              print it.
 * @sa          The output should look exactly like the one from GnuGo. See
 *              'gnugo --mode gtp' and then the command 'showboard'.
 */
void get_board_as_string( char board_output[] )
{
    int i;      // Index for x-axis
    int j;      // Index for y-axis
    char x[3];  // Label for x-axis
    char y[2];  // Label for y-axis

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

/**
 * @brief Creates horizontal board coordinate.
 *
 * Creates the horizontal board coordinate for a given number.
 *
 * @param[in]   i   The number of the horizontal coordinate
 * @param[out]  x   The string representing the horizontal coordinate
 * @return      nothing
 * @sa          get_label_y_left(), get_label_y_right()
 * @note        The letter coordinate is returned as string, not as char.
 *              The character 'I' is not used in Go boards.
 */
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

/**
 * @brief   Creates left vertical coordinate.
 *
 * Creates the vertical coordinate on the left side of the board. The
 * coordinate on the left hand side are aligned to the right hand side.
 *
 * @param[in]   j   The number of the vertical coordinate.
 * @param[out]  y   The string representing the vertical coordinate
 * @return      nothing
 * @note        The coordinates on the left side are right aligned.
 * @sa          get_label_y_right(), get_label_x()
 */
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

/**
 * @brief   Creates right vertical coordinate.
 *
 * Creates the vertical coordinate on the right side of the board. The
 * coordinate on the right hand side are aligned to the left hand side.
 *
 * @param[in]   j   The number of the vertical coordinate.
 * @param[out]  y   The string representing the vertical coordinate
 * @return      nothing
 * @note        The coordinates on the right side are left aligned.
 * @sa          get_label_y_left(), get_label_x()
 */
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

/**
 * @brief   Checks if a given vertex is a star point.
 *
 * Checks if a given vertex (with its separate coordinates) is a star point
 * (hoshi). The hoshi points depend on the board size.
 *
 * @param[in]   i   horizontal coordinate
 * @param[in]   j   vertical coordinate
 * @return      true | false
 * @sa          init_board()
 * @note        Currently only for the default board sizes (9x9, 13x13, 19x19)
 *              hoshi points are defined in init_board().
 * @todo        This function may be needed as extern later.
 */
bool is_hoshi( int i, int j )
{
    return hoshi[i][j];
}

/**
 * @brief   Returns current board size.
 *
 * Returns the current board size.
 *
 * @return      nothing
 * @sa          init_board() which sets the current board size.
 * @note        The valid board size is defined by BOARD_SIZE_MIN and
 *              BOARD_SIZE_MAX. The default board size is defined by
 *              BOARD_SIZE_DEFAULT.
 */
int get_board_size(void)
{
    return board_size;
}

/**
 * @brief   Sets or unsets a stone on a given vertex.
 *
 * A given color, which may be BLACK, WHITE or EMPTY, is stored for a given
 * vertex in the board data structure. So this function may set a stone or
 * delete it.
 *
 * @param[in]   color   Color of the stone (BLACK, WHITE) or EMPTY
 * @param[in]   i       horizontal coordinate
 * @param[in]   j       vertical coordinate
 * @return      nothing
 * @note        Remember that this function may set a stone or delete a stone.
 */
void set_vertex( int color, int i, int j )
{
    board[i][j] = color;

    return;
}

void create_groups(void)
{
    int i, j;
    int k, l;
    int color;
    int count = 0;
    int neighbour[4][2];

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {

            // Reset data structure for neighbours:
            for ( k = 0; k < 4; k++ ) {
                for ( l = 0; l < 2; l++ ) {
                    neighbour[k][l] = -1;
                }
            }

            color = board[i][j];

            if ( color == EMPTY ) {
                continue;
            }

            switch(color) {
                case BLACK:
                    printf( "# BLACK on %d,%d\n", i, j );
                    count = has_neighbour( i, j, neighbour );
                    break;
                case WHITE:
                    printf( "# WHITE on %d,%d\n", i, j );
                    count = has_neighbour( i, j, neighbour );
                    break;
                case EMPTY:
                    break;
                default:
                    fprintf( stderr, "Invalid value on %d,%d: %d\n", i, j, color );
                    exit(EXIT_FAILURE);
            }

            // TEST: show neighbours
            /*
            for ( k = 0; k < 4; k++ ) {
                if ( neighbour[k][0] == -1 ) {
                    continue;
                }
                printf( "S: %d,%d N: %d,%d\n", i, j, neighbour[k][0], neighbour[k][1] );
            }
            */

            // TODO:
            // If stone has no neighbours -> new group must be created
            // If stone has only one neighbour -> stone must be added to
            // existing group.
            // If stone has more than one neighbours -> groups must be merged.
            switch (count) {
                case 0:
                    if ( group[i][j] == 0 ) {
                        group[i][j] = get_free_group(color);
                    }
                    break;
                case 1:
                    group[i][j] = group[ neighbour[0][0] ][ neighbour[0][1] ];
                    break;
                default:
                    break;
            }
        }

    }

    return;
}

int has_neighbour( int i, int j, int neighbour[][2] )
{
    int k     = 0;
    int color = board[i][j];

    if ( color == EMPTY ) {
        fprintf( stderr, "invalid color found in group\n" );
        exit(EXIT_FAILURE);
    }

    if ( j + 1 < board_size && board[i][j+1] == color ) {
        neighbour[k][0] = i;
        neighbour[k][1] = j+1;
        k++;
    }
    if ( i + 1 < board_size && board[i+1][j] == color ) {
        neighbour[k][0] = i+1;
        neighbour[k][1] = j;
        k++;
    }
    if ( j - 1 >= 0 && board[i][j-1] == color ) {
        neighbour[k][0] = i;
        neighbour[k][1] = j-1;
        k++;
    }
    if ( i - 1 >= 0 && board[i-1][j] == color ) {
        neighbour[k][0] = i-1;
        neighbour[k][1] = j;
        k++;
    }

    return k;
}

int get_free_group( int color )
{
    int i, j;
    int field          = 0;
    int group_nr       = 0;
    int group_nr_black = 0;
    int group_nr_white = 0;

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            field = group[i][j];
            if ( field > 0 && field > group_nr_black ) {
                group_nr_black = field;
            }
            else if ( field < 0 && field < group_nr_white ) {
                group_nr_white = field;
            }
        }
    }

    switch (color) {
        case BLACK:
            group_nr = group_nr_black + 1;
            break;
        case WHITE:
            group_nr = group_nr_white - 1;
            break;
        default:
            // Error message should go here ...
            break;
    }

    return group_nr;
}

void print_groups(void)
{
    int i, j;

    for ( j = board_size - 1; j >= 0; j-- ) {
        for ( i = 0; i < board_size; i++ ) {
            printf( "%4d", group[i][j] );
        }
        printf("\n");
    }
    printf("\n");

    return;
}

