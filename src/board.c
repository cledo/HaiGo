#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
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
int black_liberties[BOARD_SIZE_MAX * BOARD_SIZE_MAX];
int white_liberties[BOARD_SIZE_MAX * BOARD_SIZE_MAX];


static void get_label_x( int i, char x[] );
static void get_label_y_left( int i, char x[] );
static void get_label_y_right( int j, char y[] );
static bool is_hoshi( int i, int j );   // This will be needed as extern maybe ..


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

    // Initialise board, group, and hoshi:
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

    // Set hoshi depending on board size:
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

    // Initialise liberty lists:
    for ( i = 0; i < BOARD_SIZE_MAX * BOARD_SIZE_MAX; i++ ) {
        black_liberties[i] = -1;
        white_liberties[i] = -1;
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
 * @todo        Maybe a coordinate check should be done here ...
 */
void set_vertex( int color, int i, int j )
{
    board[i][j] = color;

    return;
}

/**
 * @brief       Returns the color of the given vertex.
 *
 * Returns the color of the stone on a given vertex or EMPTY.
 *
 * @param[in]   i   horizontal coordinate
 * @param[in]   j   vertex coordinate
 * @return      BLACK|WHITE|EMPTY
 * @sa          set_vertex()
 * @todo        Maybe a coordinate check should be done here ...
 */
int get_vertex( int i, int j )
{
    int color = board[i][j];

    return color;
}

/**
 * @brief       Creates groups of stones.
 *
 * This function creates groups of stone and assigns numbers to them. Thess
 * numbers are stored in the group data structure. Black groups have positive
 * numbers, white groups have negative numbers. Empty fields are marked with
 * zero.
 *
 * @return      nothing
 * @note        When this function is called, the whole group board is rebuild
 *              from scratch.
 * @todo        Maybe a function is needed that updates the group board,
 *              instead of rebuilding it each time again.
 */
void create_groups(void)
{
    int i, j;
    int color;

    // Reset group board:
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            group[i][j] = 0;
        }
    }

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {

            color = board[i][j];

            // Skip field if it is empty:
            if ( color == EMPTY ) {
                continue;
            }

            // Skip if group number is set already:
            if ( group[i][j] ) {
                continue;
            }

            set_group( i, j );

        }

    }

    return;
}

/**
 * @brief       Sets the group number for a given stone.
 *
 * Sets the group number for a given vertex and if necessary for all
 * neighbouring stones. This function works recursively. It writes its data
 * into the group board data structure.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      nothing
 * @note        This is where the group board gets its data from.
 * @note        This is a recursive function
 */
void set_group( int i, int j )
{
    int k, l;
    int color;
    int count        = 0;
    int group_nr     = 0;
    int group_nr_min = INT_MAX;
    int group_nr_max = INT_MIN;
    int neighbour[4][2];
    int neighbour_group;

    color = board[i][j];

    // Reset data structure for neighbours:
    for ( k = 0; k < 4; k++ ) {
        for ( l = 0; l < 2; l++ ) {
            neighbour[k][l] = -1;
        }
    }

    // How many neighbours does the stone have, and where are they:
    count = has_neighbour( i, j, neighbour );

    // Check which group number to give to current stone:
    switch (count) {
        case 0:
            // Single stone (no neighbours), that has no group number,
            // gets next free group number.
            if ( group[i][j] == 0 ) {
                group[i][j] = get_free_group_nr(color);
            }
            break;
        default:
            // Get lowest positive (for BLACK) or highest negative
            // (for WHITE) group number of all neighbours. This is
            // then the group number for the current stone. If this
            // number is still zero, get the next free group number
            // (like case above).
            for ( k = 0; k < count; k++ ) {
                group_nr = group[ neighbour[k][0] ][ neighbour[k][1] ];
                if ( group_nr_min > group_nr && group_nr > 0 ) {
                    group_nr_min = group_nr;
                }
                if ( group_nr_max < group_nr && group_nr < 0 ) {
                    group_nr_max = group_nr;
                }
            }
            if ( color == BLACK ) {
                if ( group_nr_min == 0 || group_nr_min == INT_MAX ) {
                    group_nr_min = get_free_group_nr(BLACK);
                }
                group[i][j] = group_nr_min;
            }
            if ( color == WHITE ) {
                if ( group_nr_max == 0 || group_nr_max == INT_MIN ) {
                    group_nr_max = get_free_group_nr(WHITE);
                }
                group[i][j] = group_nr_max;
            }

            // Call function recursively for all other neighbours which have a
            // different group number.
            for ( k = 0; k < count; k++ ) {
                neighbour_group = group[ neighbour[k][0] ][ neighbour[k][1] ];
                if ( neighbour_group != group[i][j] ) {
                    set_group( neighbour[k][0], neighbour[k][1] );
                }
            }

            break;
    }


    return;
}

/**
 * @brief       Checks number of neighbours for given stone.
 *
 * For a given vertex, the number of neighbours is returned, and there
 * coordinates are written into the neighbour data structure.
 *
 * @param[in]   i           Horizontal coordinate
 * @param[in]   j           Vertical coordinate
 * @param[out]  neighbour   List of vertexes
 * @return      Number of neighbours
 */
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

/**
 * @brief       Get the next group number that may be used for new group.
 *
 * This function returns the next group number for the given color, that may
 * be used for a new group. For black this is the highest current group number
 * plus one. For white this is the lowest current group number minus one.
 *
 * @param[in]   color   BLACK|WHITE
 * @return      int     next free group number
 */
int get_free_group_nr( int color )
{
    int group_nr = 0;

    // Check if color is valid:
    if ( color != BLACK && color != WHITE ) {
        fprintf( stderr, "invalid color given to get_free_group_nr()" );
        exit(EXIT_FAILURE);
    }

    group_nr = get_last_group_nr(color);

    switch (color) {
        case BLACK:
            group_nr++;
            break;
        case WHITE:
            group_nr--;
            break;
        default:
            break;
    }

    return group_nr;
}

/**
 * @brief       Returns the last group number for given color.
 *
 * If the given color is black, the highest group number currently in use is
 * returned. For white the lowest number is returned.
 *
 * @param[in]   color   BLACK|WHITE
 * @return      int     group number
 */
int get_last_group_nr( int color )
{
    int i, j;
    int field          = 0;
    int group_nr       = 0;
    int group_nr_black = 0;
    int group_nr_white = 0;

    // Check if color is valid:
    if ( color != BLACK && color != WHITE ) {
        fprintf( stderr, "invalid color given to get_last_group_nr()" );
        exit(EXIT_FAILURE);
    }

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            field = group[i][j];
            // Get highest number of black group:
            if ( field > 0 && field > group_nr_black ) {
                group_nr_black = field;
            }
            // Get lowest number of white group:
            else if ( field < 0 && field < group_nr_white ) {
                group_nr_white = field;
            }
        }
    }

    switch (color) {
        case BLACK:
            group_nr = group_nr_black;
            break;
        case WHITE:
            group_nr = group_nr_white;
            break;
        default:
            break;
    }

    return group_nr;
}

/**
 * @brief       Shows group numbers.
 *
 * This is a debug function which shows a simple board where empty fields are
 * marked with zero, black groups are shown with positive integer numbers, and
 * white groups are shown with negative integer number.
 *
 * @return      nothing
 * @note        This function prints directly to STDOUT.
 * @todo        Maybe this function can be removed later on.
 */
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

/**
 * @brief       Counts liberties of all groups.
 *
 * This function counts the liberties of all black and white groups. The
 * results are written into the two arrays black_liberties[] and
 * white_liberties[].
 *
 */
void count_liberties(void)
{
    int i, j;
    int group_nr;
    int count;
    int black_group_nr_max = 0;
    int white_group_nr_min = 0;

    // Get highest black group number, get lowest white group number.
    black_group_nr_max = get_last_group_nr(BLACK);
    white_group_nr_min = get_last_group_nr(WHITE);

    // Initialise liberty lists:
    for ( i = 0; i < BOARD_SIZE_MAX * BOARD_SIZE_MAX; i++ ) {
        black_liberties[i] = -1;
        white_liberties[i] = -1;
    }

    // Get next black group:
    for ( group_nr = 1; group_nr <= black_group_nr_max; group_nr++ ) {
        count = 0;

        // Search group board:
        for ( i = 0; i < board_size; i++ ) {
            for ( j = 0; j < board_size; j++ ) {
                if ( group_nr == group[i][j] ) {
                    // Stone of current group found,
                    // set liberties to INT_MAX:
                    if ( j + 1 < board_size && group[i][j+1] == EMPTY ) {
                        group[i][j+1] = INT_MAX;
                    }
                    if ( i + 1 < board_size && group[i+1][j] == EMPTY ) {
                        group[i+1][j] = INT_MAX;
                    }
                    if ( i - 1 >= 0 && group[i-1][j] == EMPTY ) {
                        group[i-1][j] = INT_MAX;
                    }
                    if ( j - 1 >= 0 && group[i][j-1] == EMPTY ) {
                        group[i][j-1] = INT_MAX;
                    }
                }
            }
        }

        // Count number of INT_MAX on board, which is the number of liberties
        // for current group:
        for ( i = 0; i < board_size; i++ ) {
            for ( j = 0; j < board_size; j++ ) {
                if ( group[i][j] == INT_MAX ) {
                    count++;
                    group[i][j] = 0;
                }
            }
        }

        // Save number of liberties for current group in liberties list:
        black_liberties[group_nr] = count;
    }

    // Get next white group:
    for ( group_nr = -1; group_nr >= white_group_nr_min; group_nr-- ) {
        count = 0;

        // Search group board:
        for ( i = 0; i < board_size; i++ ) {
            for ( j = 0; j < board_size; j++ ) {
                if ( group_nr == group[i][j] ) {
                    // Stone of current group found,
                    // set liberties to INT_MIN:
                    if ( j + 1 < board_size && group[i][j+1] == EMPTY ) {
                        group[i][j+1] = INT_MIN;
                    }
                    if ( i + 1 < board_size && group[i+1][j] == EMPTY ) {
                        group[i+1][j] = INT_MIN;
                    }
                    if ( i - 1 >= 0 && group[i-1][j] == EMPTY ) {
                        group[i-1][j] = INT_MIN;
                    }
                    if ( j - 1 >= 0 && group[i][j-1] == EMPTY ) {
                        group[i][j-1] = INT_MIN;
                    }
                }
            }
        }

        // Count number of INT_MIN on boad, which is the number of liberties
        // for current group:
        for ( i = 0; i < board_size; i++ ) {
            for ( j = 0; j < board_size; j++ ) {
                if ( group[i][j] == INT_MIN ) {
                    count++;
                    group[i][j] = 0;
                }
            }
        }

        // Save number of liberties for current group in liberties list:
        white_liberties[group_nr * -1] = count;
    }

    // DEBUG:
    /*
    printf("Black groups:\n");
    for ( i = 1; i <= black_group_nr_max; i++ ) {
        printf( "\tGroup %d: %d\n", i, black_liberties[i] );
    }
    printf("White groups:\n");
    for ( i = 1; i <= white_group_nr_min * -1; i++ ) {
        printf( "\tGroup %d: %d\n", i, white_liberties[i] );
    }
    */

    return;
}

