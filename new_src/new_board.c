#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "../src/global_const.h"
#include "new_board.h"
#include "new_board_intern.h"


/**
 * @file    new_board.c
 *
 * @brief   Board data structures and functions.
 *
 * Represents all board data structures and their
 * necessary functions.
 *
 */


#define BOARD_OFF   INT_MAX
#define INDEX(i,j)  ( ( ( (j)+1 ) * ( board_size+1 ) ) + (i) )

#define BLACK_INDEX 2
#define WHITE_INDEX 0
#define EMPTY_INDEX 1

//////////////////////////////
//                          //
//  Board data structures   //
//                          //
//////////////////////////////

bsize_t board_size = BOARD_SIZE_DEFAULT;    //!< Sets the boardsize to the default.

int *board;
int *board_hoshi;


//////////////////////////////
//                          //
//  Worm data structures    //
//                          //
//////////////////////////////


worm_nr_t MAX_WORM_COUNT;  //!< Stores the maximum of possible worms for one color.

worm_nr_t *worm_nr[3] ;    //!< Three 1D-Boards with worm numbers (for WHITE_INDEX,EMPTY_INDEX,BLACK+1)
worm_nr_t worm_nr_max[3];  //!< List of current highest worm numbers (for WHITE_INDEX,EMPTY_INDEX,BLACK+1).

worm_t *worms[3];    //!< List of worm structs for black. Index is worm_nr.


//! Struct with coordinates for different board types and additional data.
typedef struct {
    int   index_1d;         //!< Coordinate for 1D-board.
    worm_nr_t worm_nr;      //!< Worm number.
} vertex_t;


/**
 *  @brief Allocates memory for all board data structures.
 *
 *  Allocates memory for the data structures board_black, board_white and
 *  board_on. Its sets the board rows to empty.
 *
 *  @param[in]  board_size  Integer of intended board size
 *  @return     nothing
 *
 */
void init_board( bsize_t board_size )
{
    //int i = 0;
    //int j = 0;
    int index_1d;
    int index_1d_max;

    set_board_size(board_size);
    index_1d_max = (board_size+1) * (board_size+2) - 1; // Make macro of this ...

    board            = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    board_hoshi      = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    worm_nr[BLACK_INDEX] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    worm_nr[WHITE_INDEX] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    worm_nr[EMPTY_INDEX] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    if ( board == NULL || board_hoshi == NULL ) {
        fprintf( stderr, "cannot allocate memory for board\n" );
        exit(EXIT_FAILURE);
    }
    if ( worm_nr[BLACK_INDEX] == NULL || worm_nr[WHITE_INDEX] == NULL || worm_nr[EMPTY_INDEX] == NULL ) {
        fprintf( stderr, "cannot allocate memory for worm_nr\n" );
        exit(EXIT_FAILURE);
    }

    // Initialise board data structures:
    for ( index_1d = 0; index_1d <= index_1d_max; index_1d++ ) {
        if ( index_1d <= board_size ) {
            // Setting lower boundary
            board[index_1d] = BOARD_OFF;
        }
        else if ( index_1d > (board_size+1) * (board_size+1) - 1) {
            // Setting upper boundary
            board[index_1d] = BOARD_OFF;
        }
        else if ( (index_1d+1) % (board_size+1) == 0 ) {
            // Setting right boundary
            board[index_1d] = BOARD_OFF;
        }
        else {
            board[index_1d] = EMPTY;
        }
        worm_nr[BLACK_INDEX][index_1d] = EMPTY;
        worm_nr[WHITE_INDEX][index_1d] = EMPTY;
        worm_nr[EMPTY_INDEX][index_1d] = EMPTY;
    }

    worm_nr_max[BLACK_INDEX] = 0;
    worm_nr_max[WHITE_INDEX] = 0;
    worm_nr_max[EMPTY_INDEX] = 0;

    // Define star points:
    init_hoshi();

    // Initialise worms array:
    if ( board_size & (bsize_t) 1 ) {
        // board_size is odd
        MAX_WORM_COUNT = ( board_size * board_size + 1 ) / 2;
    }
    else {
        // board_size is even
        MAX_WORM_COUNT = board_size * board_size / 2;
    }
    worms[BLACK_INDEX] = malloc( MAX_WORM_COUNT * sizeof(worm_t) );
    worms[WHITE_INDEX] = malloc( MAX_WORM_COUNT * sizeof(worm_t) );
    worms[EMPTY_INDEX] = malloc( MAX_WORM_COUNT * sizeof(worm_t) );
    if ( worms[BLACK_INDEX] == NULL || worms[WHITE_INDEX] == NULL || worms[EMPTY_INDEX] == NULL ) {
        fprintf( stderr, "cannot allocate memory for worms" );
        exit(EXIT_FAILURE);
    }

    /*
    index_1d = 0;
    for ( i = 0; i <= board_size+1; i++ ) {
        for ( j = 0; j <= board_size; j++ ) {
            if ( board[index_1d] == BOARD_OFF ) {
                printf( " X " );
            }
            else {
                printf( " %d ", board[index_1d] );
            }
            index_1d++;
        }
        printf("\n");
    }
    */

    return;
}

/**
 * @brief       Defines hoshi points.
 *
 * Defines the star points on the current board, depending on board size.
 *
 * @return      Nothing
 */
void init_hoshi(void)
{
    switch ( get_board_size() ) {
        case 19:
            set_hoshi( 3, 3   );
            set_hoshi( 3, 9   );
            set_hoshi( 3, 15  );
            set_hoshi( 9, 3   );
            set_hoshi( 9, 9   );
            set_hoshi( 9, 15  );
            set_hoshi( 15, 3  );
            set_hoshi( 15, 9  );
            set_hoshi( 15, 15 );
            break;
        case 13:
            set_hoshi( 3, 3   );
            set_hoshi( 3, 9   );
            set_hoshi( 9, 3   );
            set_hoshi( 9, 9   );
            set_hoshi( 6, 6   );
            break;
        case 9:
            set_hoshi( 2, 2   );
            set_hoshi( 2, 6   );
            set_hoshi( 6, 2   );
            set_hoshi( 6, 6   );
            set_hoshi( 4, 4   );
            break;
    }

    return;
}

/**
 * @brief Frees the memory allocated for all board data structures.
 *
 * Frees the memory which has been allocated for the board_black, board_white
 * and board_on structures.
 *
 * @return  nothing
 * @sa      init_board()
 * @note    The pointers to board_black, board_white and board_on are also set to NULL, so a check
 *          whether the pointers are still valid or not is possible.
 */
void free_board(void)
{
    free(board);
    free(board_hoshi);

    board       = NULL;
    board_hoshi = NULL;

    free(worms[BLACK_INDEX]);
    free(worms[WHITE_INDEX]);
    free(worms[EMPTY_INDEX]);
    free(worm_nr[BLACK_INDEX]);
    free(worm_nr[WHITE_INDEX]);
    free(worm_nr[EMPTY_INDEX]);

    worms[BLACK_INDEX]   = NULL;
    worms[WHITE_INDEX]   = NULL;
    worms[EMPTY_INDEX]   = NULL;
    worm_nr[BLACK_INDEX] = NULL;
    worm_nr[WHITE_INDEX] = NULL;
    worm_nr[EMPTY_INDEX] = NULL;

    return;
}

/**
 * @brief       Defines vertex as hoshi
 *
 * Defines the the given vertex as star point.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      Nothing
 */
void set_hoshi( int i, int j )
{
    board_hoshi[ INDEX(i,j) ] = 1;

    return;
}

/**
 * @brief       Checks if all board pointers are NULL.
 *
 * Returns true if all pointers to board_* are NULL.
 *
 * @return      true|false
 * @note        This function is needed for testing only.
 */
bool is_board_null(void)
{
    bool is_null = false;

    if ( board == NULL && board_hoshi == NULL ) {
        is_null = true;
    }

    return is_null;
}

/**
 * @brief       Checks if vertex is valid board vertex.
 *
 * Checks if the given vertex is still within board range.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      true | false
 */
bool is_on_board( int i, int j )
{
    int  index_1d = INDEX(i,j);

    if ( index_1d < 0 ) {
        return false;
    }

    if ( board[index_1d] != BOARD_OFF ) {
        return true;
    }

    return false;
}

/**
 * @brief       Sets board size.
 *
 * Sets the size of the board to the given value.
 *
 * @param[in]   size    New size of the board.
 * @note        The board size must be a value from BOARD_SIZE_MIN to BOARD_SIZE_MAN.
 * @return      Nothing
 */
void set_board_size( bsize_t size )
{
    if ( size < BOARD_SIZE_MIN || size > BOARD_SIZE_MAX ) {
        fprintf( stderr, "set_board_size() has been given invalid size: %u\n", size );
        exit(EXIT_FAILURE);
    }

    board_size = size;

    return;
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
bsize_t get_board_size(void)
{
    return board_size;
}

/**
 * @brief   Sets or unsets a stone on a given vertex.
 *
 * A given color, which may be BLACK, WHITE or EMPTY, is stored for a given
 * vertex in the board data structures. So this function may set a stone or
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
    board[ INDEX(i,j) ] = color;

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
 * @sa          get_vertex_intern()
 */
int get_vertex( int i, int j )
{

    return board[ INDEX(i,j) ];
}

/**
 * @brief       Returns the color of the given vertex.
 *
 * Returns the color of the stone on a given vertex or EMPTY. The coordinates
 * must have been converted to internal data!
 *
 * @param[in]   I   horizontal coordinate of row_t data type
 * @param[in]   J   vertex coordinate, which is j + 1
 * @return      BLACK|WHITE|EMPTY
 * @sa          get_vertex()
 * @note        Use get_vertex() if you have external coordinated i, j.
 * @todo        Check if inline makes sense here.
 */
inline int get_vertex_intern( int index_1d )
{
    return board[index_1d];
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
 * @brief       Returns the number of white stones black has captured.
 *
 * Returns the number of white stones black has captured in total.
 *
 * @return      Number of captured stones by black
 * @sa          get_white_captured()
 * @warning     This does not return the number of captured black stones!
 * @todo        Not implemented yet!
 */
int get_black_captured(void)
{
    return 0;
}

/**
 * @brief       Returns the number of black stones captured by white.
 *
 * Returns the number of black stones that white has captured in total.
 *
 * @return      Number of black stones white has captured.
 * @sa          get_black_captured()
 * @warning     This does not return the number of captured white stones!
 * @todo        Not implemented yet!
 */
int get_white_captured(void)
{
    return 0;
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
 * @note        Currently only for the default board sizes (9x9, 13x13, 19x19)
 *              hoshi points are defined in init_board().
 */
bool is_hoshi( int i, int j )
{

    if ( board_hoshi[ INDEX(i,j) ] ) {
        return true;
    }

    return false;
}

/**
 * @brief       Scans the board.
 *
 * Scans the board to create various data.
 *
 * @return      Nothing
 * @note        This replaces the former create_groups() function.
 */
void scan_board(void)
{
    int index_1d;
    int index_1d_max;
    int color;

    // Only those functions should be called here, which are necessary to make
    // a valid move! Consider this to be kind of a scan level nr. 1.

    // Maybe this should be moved to init_board():
    memset( worm_nr[BLACK_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    memset( worm_nr[WHITE_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    memset( worm_nr[EMPTY_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    worm_nr_max[BLACK_INDEX] = 0;
    worm_nr_max[WHITE_INDEX] = 0;
    worm_nr_max[EMPTY_INDEX] = 0;

    memset( worms[BLACK_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );
    memset( worms[WHITE_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );
    memset( worms[EMPTY_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );

    index_1d_max = ( board_size + 1 ) * ( board_size + 1 ) - 1;

    // First scan:
    // Gives a worm_nr to every field.
    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {
        // Current vertex is index_1d.

        if ( board[index_1d] == BOARD_OFF ) {
            continue;
        }

        // Check for worm number:
        if ( ( worm_nr[BLACK_INDEX][index_1d] & worm_nr[WHITE_INDEX][index_1d] & worm_nr[EMPTY_INDEX][index_1d] ) == 0 ) {
            color = board[index_1d];
            create_worm_data( index_1d, color );
        }
    }

    // Second scan:
    // Build list of worm structs.
    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {

        if ( board[index_1d] == BOARD_OFF ) {
            continue;
        }

        build_worms(index_1d);

        // Number of worms is still unknown here, because a first level scan
        // does not need this information!

    }

    // Third scan:
    // Count liberties of every worm.
    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {

        if ( board[index_1d] == BLACK || board[index_1d] == WHITE ) {
            count_worm_liberties(index_1d);
        }

    }

    return;
}

/**
 * @brief       Creates board data for worms.
 *
 * Creates worms by assigning worm numbers to BLACK, WHITE and EMPTY fields.
 *
 * @param[in]   index_1d    Coordinate for 1D-board.
 * @param[in]   color       Color of the given field.
 * @return      Nothing
 * @note        This is a recursive function.
 */
void create_worm_data( int index_1d, int color )
{
    int n;
    int i;
    int count = 0;
    worm_nr_t worm_nr_min = USHRT_MAX;
    int color_index = color + 1;

    vertex_t neighbours[4];

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( board[i] != BOARD_OFF ) {
        // Check for neighbour of same color:
        if ( worm_nr[color_index][i] ) {
            neighbours[count].index_1d = i;
            neighbours[count].worm_nr  = worm_nr[color_index][i];

            if ( neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    // Check neighbour EAST:
    i = index_1d + 1;
    if ( board[i] != BOARD_OFF ) {
        if ( worm_nr[color_index][i] ) {
            neighbours[count].index_1d = i;
            neighbours[count].worm_nr  = worm_nr[color_index][i];

            if ( neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( board[i] != BOARD_OFF ) {
        if ( worm_nr[color_index][i] ) {
            neighbours[count].index_1d = i;
            neighbours[count].worm_nr  = worm_nr[color_index][i];

            if ( neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    // Check neighbour WEST:
    i = index_1d - 1;
    if ( board[i] != BOARD_OFF ) {
        if ( worm_nr[color_index][i] ) {
            neighbours[count].index_1d = i;
            neighbours[count].worm_nr  = worm_nr[color_index][i];

            if ( neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }

    switch (count) {
        case 0:
            worm_nr[color_index][index_1d] = ++worm_nr_max[color_index];
            break;
        case 1:
            worm_nr[color_index][index_1d] = neighbours[0].worm_nr;
            break;
        case 2:
        case 3:
        case 4:
            // We take the lowest of all neighbouring worm numbers:
            worm_nr[color_index][index_1d] = worm_nr_min;
            for ( n = 0; n < count; n++ ) {
                // Call create_worm() on all neighbours that have not the
                // minimum worm number:
                if ( neighbours[n].worm_nr > worm_nr_min ) {
                    create_worm_data( neighbours[n].index_1d, color );
                }
            }
            break;
    }

    return;
}

/**
 * @brief       Builds list of worm structs.
 *
 * From the information on the worm boards, builds a list of worm structs.
 *
 * @param[in]   index_1d    Coordinate for 1D-board.
 * @return      Nothing
 * @note        The function create_worm_data() must have been called already.
 */
inline void build_worms( int index_1d )
{
    int color                      = board[index_1d];
    int color_index                = color + 1;
    worm_nr_t worm_nr_current = worm_nr[color_index][index_1d];
    worm_t *w                      = &worms[color_index][worm_nr_current];

    if ( w->number == 0 ) {
        w->number    = worm_nr_current;
        w->count     = 1;
        w->liberties = 0;
    }
    else {
        w->count++;
    }

    return;
}

/**
 * @brief       Counts liberties.
 *
 * Counts the liberties of black and white worms and writes them into the worm
 * struct list.
 *
 * @param[in]   index_1d    Coordinate for 1D-board.
 * @return      Nothing
 */
void count_worm_liberties( int index_1d )
{
    int i;
    int count;
    int color = board[index_1d];
    worm_nr_t worm_nr_current = worm_nr[color+1][index_1d];
    worm_t *w = worms[color+1];

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr_current, color );
        if (count) {
            w[worm_nr_current].liberties += ( 12 / count );
        }
    }

    // Check neighbour EAST:
    i = index_1d + 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr_current, color );
        if (count) {
            w[worm_nr_current].liberties += ( 12 / count );
        }
    }

    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr_current, color );
        if (count) {
            w[worm_nr_current].liberties += ( 12 / count );
        }
    }

    // Check neighbour WEST:
    i = index_1d - 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr_current, color );
        if (count) {
            w[worm_nr_current].liberties += ( 12 / count );
        }
    }

    return;
}

inline int get_worm_neighbours( int index_1d, worm_nr_t worm_nr_current, int color )
{
    int i;
    int count = 0;
    worm_nr_t *w = worm_nr[color+1];

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( w[i] == worm_nr_current ) {
        count++;
    }

    // Check neighbour EAST:
    i = index_1d + 1;
    if ( w[i] == worm_nr_current ) {
        count++;
    }

    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( w[i] == worm_nr_current ) {
        count++;
    }

    // Check neighbour WEST:
    i = index_1d - 1;
    if ( w[i] == worm_nr_current ) {
        count++;
    }

    return count;
}

/**
 * @brief       TEST: Prints worm boards
 *
 * TEST: Prints worm boards
 *
 * @note        May be removed later!
 */
void print_worm_boards(void)
{
    int i;

    printf("\n");
    for ( i = board_size + 1; i < (board_size+1) * (board_size+2) - 1; i++ ) {
        if ( board[i] == BOARD_OFF ) {
            if ( ( ( i + 1 ) % ( board_size + 1 ) ) == 0 ) {
                printf("\n");
            }
            continue;
        }
        printf( " %2hu ", worm_nr[BLACK_INDEX][i] );
    }
    printf("\n");

    printf("\n");
    for ( i = board_size + 1; i < (board_size+1) * (board_size+2) - 1; i++ ) {
        if ( board[i] == BOARD_OFF ) {
            if ( ( ( i + 1 ) % ( board_size + 1 ) ) == 0 ) {
                printf("\n");
            }
            continue;
        }
        printf( " %2hu ", worm_nr[WHITE_INDEX][i] );
    }
    printf("\n");

    printf("\n");
    for ( i = board_size + 1; i < (board_size+1) * (board_size+2) - 1; i++ ) {
        if ( board[i] == BOARD_OFF ) {
            if ( ( ( i + 1 ) % ( board_size + 1 ) ) == 0 ) {
                printf("\n");
            }
            continue;
        }
        printf( " %2hu ", worm_nr[EMPTY_INDEX][i] );
    }
    printf("\n");

    return;
}

/**
 * @brief       TEST: Prints worm structs list
 *
 * TEST: Prints worm structs list.
 *
 * @note        May be removed later.
 */
void print_worm_lists(void)
{
    int i;
    worm_t *w;

    //w = worms[BLACK_INDEX];
    w = worms[WHITE_INDEX];
    //w = worms[EMPTY_INDEX];

    for ( i = 0; i < MAX_WORM_COUNT; i++ ) {
        if ( w[i].number == 0 ) {
            continue;
        }

        printf( "Nr.: %hu\tCount: %hu Lib: %hu\n"
            , w[i].number, w[i].count, w[i].liberties );
    }

    return;
}

/**
 * @brief       TEST: Returns struct for given worm number.
 *
 * For the given color and worm number the worm struct is returned.
 *
 * @param[in]   color   BLACK|WHITE|EMPTY
 * @param[in]   worm_nr Worm number.
 * @return      Struct of given worm.
 * @note        [any note about the function you might have]
 */
worm_t get_worm( int color, worm_nr_t worm_nr )
{

    return worms[color+1][worm_nr];
}

