#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "global_const.h"
#include "board.h"
#include "move.h"


/**
 * @file    board.c
 *
 * @brief   Represents the board
 *
 * Represents the board and all its data structures.
 * It provides all necessary functions to work with board data.
 *
 */

#define BLACK_HASH 0    //!< Index for black stone in hash board.
#define WHITE_HASH 1    //!< Index for white stone in hash board.
#define KO_HASH    2    //!< Index for ko field in hash board.
//#define EMPTY_HASH 3

/* State variables */
// As described in GTP v2.0 chapter 5.1
static int  **board;   //!< The main board data structure.
static int  **group;   //!< A helper board which contains group numbers.
static bool **hoshi;   //!< A helper board which defines the star points.

static unsigned ***hash_board;  //!< Initial hash board according to Zobrist hashing.
static unsigned hash_id;        //!< Represents the hash ID of a position on the board.

//! Element of hash table:
struct hash_pos_st {
    bool valid;     //!< Indicates if element is valid.
    int  value;     //!< Value of position.
};
//! Hash table for transpositions:
struct hash_pos_st hash_table[HASH_TABLE_SIZE];

static int board_size     = 0;     //!< The size of the board.
static int black_captured = 0;     //!< Number of white stones captured by black.
static int white_captured = 0;     //!< Number of black stones captured by white.
static int black_liberties[BOARD_SIZE_MAX * BOARD_SIZE_MAX];   //!< List of number of liberties per black group.
static int white_liberties[BOARD_SIZE_MAX * BOARD_SIZE_MAX];   //!< List of number of liberties per white group.
static int black_group_size[BOARD_SIZE_MAX * BOARD_SIZE_MAX];  //!< List of group size per black group.
static int white_group_size[BOARD_SIZE_MAX * BOARD_SIZE_MAX];  //!< List of group size per white group.
static int captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];   //!< List of verteces of captured stones by current move.

static int black_last_group_nr;     //!< Stores the last used group number for black.
static int white_last_group_nr;     //!< Stores the last used group number for white.

static void init_hash_board(void);
static void set_group( int i, int j );
static void get_label_x( int i, char x[] );
static void get_label_y_left( int i, char x[] );
static void get_label_y_right( int j, char y[] );



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

    black_captured = 0;
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

    // Initialise liberty lists, group size lists, and captured_now list:
    for ( i = 0; i < BOARD_SIZE_MAX * BOARD_SIZE_MAX; i++ ) {
        black_liberties[i]  = INVALID;
        white_liberties[i]  = INVALID;
        black_group_size[i] = 0;
        white_group_size[i] = 0;
        captured_now[i][0]  = INVALID;
        captured_now[i][1]  = INVALID;
    }

    init_hash_board();

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
    int i, j;

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

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            free( hash_board[i][j] );
        }
        free( hash_board[i] );
    }
    free(hash_board);

    hash_board = NULL;

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
    int old_color = get_vertex( i, j );

    if ( old_color == BLACK ) {
        hash_id ^= hash_board[i][j][BLACK_HASH];
    }
    else if ( old_color == WHITE ) {
        hash_id ^= hash_board[i][j][WHITE_HASH];
    }

    if ( color == BLACK ) {
        hash_id ^= hash_board[i][j][BLACK_HASH];
    }
    else if ( color == WHITE ) {
        hash_id ^= hash_board[i][j][WHITE_HASH];
    }

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
 * This function creates groups of stone and assigns numbers to them. These
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

    black_last_group_nr = 0;
    white_last_group_nr = 0;

    // Reset group board:
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            group[i][j] = 0;
        }
    }

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {

            color = get_vertex( i, j );

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

    color = get_vertex( i, j );

    // Reset data structure for neighbours:
    for ( k = 0; k < 4; k++ ) {
        for ( l = 0; l < 2; l++ ) {
            neighbour[k][l] = INVALID;
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
                //group[i][j] = get_free_group_nr(color);
                if ( color == BLACK ) {
                    group[i][j] = ++black_last_group_nr;
                }
                else {
                    group[i][j] = --white_last_group_nr;
                }
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
                    //group_nr_min = get_free_group_nr(BLACK);
                    group_nr_min = ++black_last_group_nr;
                }
                group[i][j] = group_nr_min;
            }
            if ( color == WHITE ) {
                if ( group_nr_max == 0 || group_nr_max == INT_MIN ) {
                    //group_nr_max = get_free_group_nr(WHITE);
                    group_nr_max = --white_last_group_nr;
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
    int color = get_vertex( i, j );

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
    int group_nr;

    if ( color == BLACK ) {
        group_nr = black_last_group_nr;
    }
    else {
        group_nr = white_last_group_nr;
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
    int k;
    int group_nr;
    bool is_liberty_black[BOARD_SIZE_MAX * BOARD_SIZE_MAX];
    bool is_liberty_white[BOARD_SIZE_MAX * BOARD_SIZE_MAX];

    int black_group_nr_max = get_last_group_nr(BLACK);
    int white_group_nr_min = get_last_group_nr(WHITE);

    // Initialise liberty lists:
    for ( i = 0; i < BOARD_SIZE_MAX * BOARD_SIZE_MAX; i++ ) {
        black_liberties[i] = INVALID;
        white_liberties[i] = INVALID;
    }

    for ( i = 0; i <= black_group_nr_max; i++ ) {
        is_liberty_black[i] = false;
        black_liberties[i]  = 0;
    }
    for ( i = 0; i <= white_group_nr_min * -1; i++ ) {
        is_liberty_white[i] = false;
        white_liberties[i]  = 0;
    }

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( get_vertex( i, j ) == EMPTY ) {
                // North:
                if ( j + 1 < board_size && ( group_nr = get_group_nr( i, j+1 ) ) ) {
                    if ( group_nr > 0 ) {
                        is_liberty_black[group_nr] = true;
                    }
                    else {
                        is_liberty_white[group_nr * -1] = true;
                    }
                }
                // South:
                if ( j - 1 >= 0 && ( group_nr = get_group_nr( i, j-1 ) ) ) {
                    if ( group_nr > 0 ) {
                        is_liberty_black[group_nr] = true;
                    }
                    else {
                        is_liberty_white[group_nr * -1] = true;
                    }
                }
                // East:
                if ( i + 1 < board_size && ( group_nr = get_group_nr( i+1, j ) ) ) {
                    if ( group_nr > 0 ) {
                        is_liberty_black[group_nr] = true;
                    }
                    else {
                        is_liberty_white[group_nr * -1] = true;
                    }
                }
                // West:
                if ( i - 1 >= 0 && ( group_nr = get_group_nr( i-1, j ) ) ) {
                    if ( group_nr > 0 ) {
                        is_liberty_black[group_nr] = true;
                    }
                    else {
                        is_liberty_white[group_nr * -1] = true;
                    }
                }

                // Count liberties:
                for ( k = 1; k <= black_group_nr_max; k ++ ) {
                    if ( is_liberty_black[k] ) {
                        black_liberties[k]++;
                        is_liberty_black[k] = false;
                    }
                }
                for ( k = 1; k <= white_group_nr_min * -1; k ++ ) {
                    if ( is_liberty_white[k] ) {
                        white_liberties[k]++;
                        is_liberty_white[k] = false;
                    }
                }
            }
        }
    }

    return;
}

/**
 * @brief       Remove all groups without liberties for given color.
 *
 * This function removes all groups with zero liberties of a given color. The
 * number of removed stones is returned.
 *
 * @param[in]   color   BLACK|WHITE
 * @return      Number of removed stones
 */
int remove_stones( int color )
{
    int i, j;
    int k = 0;
    int group_nr;
    int real_group_nr;
    int group_nr_max;
    int *group_liberties;
    int stones_removed;
    
    group_nr_max   = get_last_group_nr(color);
    stones_removed = 0;

    if ( color == WHITE ) {
        group_nr_max *= -1;
        group_liberties = white_liberties;
    }
    else {
        group_liberties = black_liberties;
    }

    // Check all groups for current color:
    for ( group_nr = 1; group_nr <= group_nr_max; group_nr++ ) {
        if ( group_liberties[group_nr] == 0 ) {
            // Found group with zero liberties:
            for ( i = 0; i < board_size; i++ ) {
                for ( j = 0; j < board_size; j++ ) {
                    real_group_nr = ( color == WHITE ) ? group_nr * -1 : group_nr;
                    // Delete this group:
                    if ( group[i][j] == real_group_nr ) {
                        //board[i][j] = EMPTY;
                        set_vertex( EMPTY, i, j );
                        group[i][j] = EMPTY;
                        stones_removed++;
                        captured_now[k][0] = i;
                        captured_now[k][1] = j;
                        k++;
                    }
                }
            }
        }
    }
    captured_now[k][0] = INVALID;
    captured_now[k][1] = INVALID;

    if ( color == BLACK ) {
        white_captured += stones_removed;
    }
    else if ( color == WHITE ) {
        black_captured += stones_removed;
    }


    return stones_removed;
}

/**
 * @brief       Returns group number for given vertex.
 *
 * Returns the group number for a given vertex. For a black stone this is a
 * positive integer, for a white stone this is a negative integer, and for an
 * empty field zero is returned.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      group number
 */
int get_group_nr( int i, int j )
{

    return group[i][j];
}

/**
 * @brief       Returns number of liberties for a given group.
 *
 * Returns the number of liberties for a given group.
 *
 * @param[in]   group_nr     group number
 * @return      Number of liberties
 */
int get_nr_of_liberties( int group_nr )
{
    int nr_of_liberties;

    if ( group_nr > 0 ) {
        nr_of_liberties = black_liberties[group_nr];
    }
    else {
        nr_of_liberties = white_liberties[ group_nr * -1 ];
    }

    return nr_of_liberties;
}

/**
 * @brief       Returns number and vertexes of currently captured stones.
 *
 * The number of currently captured stones is returned. The list of vertexes
 * where stones have been captured now is written into the captured[][2]
 * parameter.
 *
 * @param[out]  captured    List of vertexes
 * @return      Number of captured stones
 * @todo        Maybe a pointer to captured_now[][2] would be enough instead
 *              of performing a copy.
 */
int get_captured_now( int captured[][2] )
{
    int k = 0;
    int nr_of_captured_now = 0;

    while ( captured_now[k][0] != INVALID ) {
        captured[k][0] = captured_now[k][0];
        captured[k][1] = captured_now[k][1];
        nr_of_captured_now++;
        k++;
    }
    captured[k][0] = INVALID;
    captured[k][1] = INVALID;

    return nr_of_captured_now;
}

/**
 * @brief       Determines the size of all groups.
 *
 * Determines the size of all black and white groups and stores the results in
 * black_group_size[] and white_group_size[].
 *
 * @sa  get_size_of_group()
 *
 * @return      Nothing
 */
void set_groups_size(void)
{
    int i, j;
    int group_nr;

    for ( i = 0; i < BOARD_SIZE_MAX * BOARD_SIZE_MAX; i++ ) {
        black_group_size[i] = 0;
        white_group_size[i] = 0;
    }

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            group_nr = group[i][j];
            if ( group_nr == 0 ) {
                continue;
            }
            else if ( group_nr > 0 ) {
                black_group_size[group_nr]++;
            }
            else if ( group_nr < 0 ) {
                white_group_size[group_nr * -1]++;
            }
        }
    }

    return;
}

/**
 * @brief       Returns the size of a given group.
 *
 * Returns the size, that is the number of stones, for a given group,
 * determined by group number.
 *
 * @param[in]   group_nr    Number of group.
 * @return      group_size  Size of group.
 */
int get_size_of_group( int group_nr )
{
    int group_size = 0;

    if ( group_nr > 0 ) {
        group_size = black_group_size[group_nr];
    }
    else if ( group_nr < 0 ) {
        group_size = white_group_size[group_nr * -1];
    }

    return group_size;
}

/**
 * @brief       Returns the number of white stones black has captured.
 *
 * Returns the number of white stones black has captured in total.
 *
 * @return      Number of captured stones by black
 * @sa          get_white_captured()
 * @warning     This does not return the number of captured black stones!
 */
int get_black_captured(void)
{

    return black_captured;
}

/**
 * @brief       Returns the number of black stones captured by white.
 *
 * Returns the number of black stones that white has captured in total.
 *
 * @return      Number of black stones white has captured.
 * @sa          get_black_captured()
 * @warning     This does not return the number of captured white stones!
 */
int get_white_captured(void)
{

    return white_captured;
}

/**
 * @brief       Sets the number of stones captured by black.
 *
 * Sets the number of total stones captured by black.
 *
 * @param[in]   captured    Number of captured stones.
 * @return      Nothing
 * @sa          set_white_captured()
 */
void set_black_captured( int captured )
{
    black_captured = captured;

    return;
}

/**
 * @brief       Sets the number of stones captured by white.
 *
 * Sets the number of total stones captured by white.
 *
 * @param[in]   captured    Number of captured stones.
 * @return      Nothing
 * @sa          set_black_captured()
 */
void set_white_captured( int captured )
{
    white_captured = captured;

    return;
}

/**
 * @brief       Count stones on board.
 *
 * Counts the number of stones of a given color on the board. Because EMPTY is
 * treated as color also, empty fields may be counted, too.
 *
 * @param[in]   color   Color of stones or EMPTY
 * @return      Number of stones (or empty fields)
 */
int get_stone_count( int color )
{
    int i, j;
    int count = 0;

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( get_vertex( i, j ) == color ) {
                count++;
            }
        }
    }

    return count;
}

/**
 * @brief       Checks if board pointer is NULL.
 *
 * Returns true if board points to NULL, false otherwise.
 *
 * @return      true|false
 * @sa          is_hoshi_board_null(), is_group_board_null()
 * @note        This function is needed for testing only.
 */
bool is_board_null(void)
{
    bool is_null;

    if ( board == NULL ) {
        is_null = true;
    }
    else {
        is_null = false;
    }

    return is_null;
}

/**
 * @brief       Checks if group board points to NULL.
 *
 * Returns true if the group board points to NULL, false otherwise.
 *
 * @return      true|false
 * @sa          is_board_null(), is_hoshi_board_null()
 * @note        This function is needed for testing only.
 */
bool is_group_board_null(void)
{
    bool is_null;

    if ( group == NULL ) {
        is_null = true;
    }
    else {
        is_null = false;
    }

    return is_null;
}

/**
 * @brief       Checks if hoshi board points to NULL.
 *
 * Returns true if the hoshi board points to NULL, false otherwise.
 *
 * @return      true|false
 * @sa          is_board_null(), is_group_board_null()
 * @note        This function is needed for testing only.
 */
bool is_hoshi_board_null(void)
{
    bool is_null;

    if ( hoshi == NULL ) {
        is_null = true;
    }
    else {
        is_null = false;
    }

    return is_null;
}

/**
 * @brief       Initialises the hash board at start up.
 *
 * Initialises the hash board for Zobrist hashing.
 *
 * @return      Nothing
 */
void init_hash_board(void)
{
    int i, j, k;
    int board_size = get_board_size();

    // Initialise hash_id for empty board:
    hash_id = 0;

    // Allocate memory for hash_board:
    hash_board = malloc( sizeof( unsigned *** ) * board_size );
    for ( i = 0; i < board_size; i ++ ) {
        hash_board[i] = malloc( sizeof( unsigned ** ) * board_size );
        for ( j = 0; j < board_size; j++ ) {
            hash_board[i][j] = malloc( sizeof( unsigned * ) * 3 );
        }
    }

    // Set random values to hash_board:
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            for ( k = 0; k < 3; k++ ) {
                hash_board[i][j][k] = (unsigned)( rand() + rand() );
            }
        }
    }


    return;
}

/**
 * @brief       Initialises the hash_id for the starting position.
 *
 * The initial hash_id for the starting position is calculated. This
 * must be called explicitely when an SGF file is loaded.
 *
 * @return      Nothing.
 * @note        This function does not return the hash_id.
 * @sa          get_hash_id()
 */
void init_hash_id(void)
{
    int i, j;

    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            switch ( get_vertex( i, j ) ) {
                case BLACK:
                    hash_id ^= hash_board[i][j][BLACK_HASH];
                    break;
                case WHITE:
                    hash_id ^= hash_board[i][j][WHITE_HASH];
                    break;
                case EMPTY:
                    break;
            }
        }
    }

    return;
}

/**
 * @brief       Returns the current hash_id.
 *
 * Returns the hash_id of the current board position.
 *
 * @return      hash_id
 */
unsigned get_hash_id(void)
{

    return hash_id;
}

/**
 * @brief       Sets the new value of hash_id.
 *
 * Sets the hash_id to the given value.
 *
 * @param[in]   id   New hash_id of current position.
 * @return      Nothing
 */
void set_hash_id( unsigned id )
{

    hash_id = id;

    return;
}

/**
 * @brief       Initialises hash table.
 *
 * Initialises hash table by setting all entries to invalid.
 *
 * @return      Nothing
 */
void init_hash_table(void)
{
    int k;

    for ( k = 0; k < HASH_TABLE_SIZE; k++ ) {
        hash_table[k].valid = false;
        hash_table[k].value = 0;
    }

    return;
}

/**
 * @brief       Inserts new element into hash table.
 *
 * Inserts a new hash_pos_st element into the hash_table list.
 *
 * @param[in]   id      The current position's hash_id.
 * @param[in]   value   The current position's value.
 * @return      Nothing
 */
void insert_hash_table( unsigned id, int value )
{
    
    unsigned index = id % HASH_TABLE_SIZE;

    hash_table[index].valid = true;
    hash_table[index].value = value;

    return;
}

/**
 * @brief       Select position value from hash table.
 *
 * For a given position's hash_id the value from the hash_table is selected.
 *
 * @param[in]   id      hash_id of current position.
 * @return      value   Value of position.
 */
int select_hash_table_value( unsigned id )
{
    int      value;
    unsigned index = id % HASH_TABLE_SIZE;

    value = hash_table[index].value;

    return value;
}

/**
 * @brief       Checks if current positon has hash_id entry.
 *
 * Returns true if the current positon with its hash_id has already a valid
 * entry in the hash table.
 *
 * @param[in]   id      hash_id of current position.
 * @return      true|false
 */
bool exists_hash_id( unsigned id )
{
    bool is_valid  = false;
    unsigned index = id % HASH_TABLE_SIZE;

    if ( hash_table[index].valid ) {
        is_valid = true;
    }

    return is_valid;
}

