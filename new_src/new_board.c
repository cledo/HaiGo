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

#define NORTH   0   //!< Sets index for neighbours list.
#define EAST    1   //!< Sets index for neighbours list.
#define SOUTH   2   //!< Sets index for neighbours list.
#define WEST    3   //!< Sets index for neighbours list.

//////////////////////////////
//                          //
//  Board data structures   //
//                          //
//////////////////////////////

bsize_t board_size = BOARD_SIZE_DEFAULT;    //!< Sets the boardsize to the default.

row_t *board_black;     //!< Defines board for black stones.
row_t *board_white;     //!< Defines board for white stones.
row_t *board_on;        //!< Defines board for determining inside and ouside of board.
row_t *board_hoshi;     //!< Defines the star points on the board.


//! Data structure representing a worm.
typedef struct worm_st {
    unsigned short number;  //!< Worm number
} worm_t;

unsigned short MAX_WORM_COUNT;  //!< Stores the maximum of possible worms for one color.

unsigned short *worm_nr[3] ;    //!< Three 1D-Boards with worm numbers (for WHITE+1,EMPTY+1,BLACK+1)
unsigned short worm_nr_max[3];      //!< List of current highest worm numbers (for WHITE+1,EMPTY+1,BLACK+1).

worm_t *black_worms;    //!< List of worm structs for black.
worm_t *white_worms;    //!< List of worm structs for white.


//! Struct with coordinates for different board types and additional data.
typedef struct {
    row_t I;                //!< Horizontal coordinate for binary board.
    int   J;                //!< Vertical coordinate for binary board.
    int   index_1d;         //!< Coordinate for 1D-board.
    unsigned short worm_nr; //!< Worm number.
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
    row_t i;

    set_board_size(board_size);

    // Add a lower and an upper boundary row; therefore we take board_size + 2
    board_black = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    board_white = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    board_on    = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    board_hoshi = malloc( (size_t)( ( board_size + 2 ) * sizeof(row_t) ) );
    if ( board_black == NULL || board_white == NULL || board_on == NULL || board_hoshi == NULL ) {
        fprintf( stderr, "cannot allocate memory for board\n" );
        exit(1);
    }

    for ( i = 0; i < board_size + 2; i++ ) {
        board_black[i] = 0;
        board_white[i] = 0;
        board_hoshi[i] = 0;
        board_on[i]    = 0xFFFFFFFF;
        board_on[i]    = ~( ( board_on[i] >> ( board_size + 1 ) ) | 0x80000000 );
    }
    board_on[0]                = 0x00000000;
    board_on[ board_size + 1 ] = 0x00000000;

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
    black_worms = malloc( (size_t)( MAX_WORM_COUNT * sizeof(worm_t) ) );
    white_worms = malloc( (size_t)( MAX_WORM_COUNT * sizeof(worm_t) ) );
    if ( black_worms == NULL || white_worms == NULL ) {
        fprintf( stderr, "cannot allocate memory for worms" );
        exit(1);
    }

    // Initialise worm boards:
    worm_nr[BLACK+1] = malloc( (size_t)( board_size * board_size * sizeof( unsigned short ) ) );
    worm_nr[WHITE+1] = malloc( (size_t)( board_size * board_size * sizeof( unsigned short ) ) );
    worm_nr[EMPTY+1] = malloc( (size_t)( board_size * board_size * sizeof( unsigned short ) ) );
    worm_nr_max[BLACK+1] = 0;
    worm_nr_max[WHITE+1] = 0;
    worm_nr_max[EMPTY+1] = 0;

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
    free(board_black);
    free(board_white);
    free(board_on);
    free(board_hoshi);

    board_black = NULL;
    board_white = NULL;
    board_on    = NULL;
    board_hoshi = NULL;

    free(black_worms);
    free(white_worms);
    free(worm_nr[BLACK+1]);
    free(worm_nr[WHITE+1]);
    free(worm_nr[EMPTY+1]);

    black_worms   = NULL;
    white_worms   = NULL;
    worm_nr[BLACK+1] = NULL;
    worm_nr[WHITE+1] = NULL;
    worm_nr[EMPTY+1] = NULL;

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
    int   J = j + 1;
    row_t I = 0x80000000 >> ( i + 1 );  // Bit 31 set to 1 is 0x80000000

    board_hoshi[J] |= I;

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

    if ( board_black == NULL && board_white == NULL && board_on == NULL && board_hoshi == NULL ) {
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
    bool  is_on = false;
    int   J     = j + 1;
    row_t I     = 0x80000000 >> ( i + 1 );  // Bit 31 set to 1 is 0x80000000

    if ( board_on[J] & I ) {
        is_on = true;
    }

    return is_on;
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
        exit(1);
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
inline int get_vertex_intern( row_t I, int J )
{
    int color;

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
    bool is_hoshi_point = false;

    int   J = j + 1;
    row_t I = 0x80000000 >> ( i + 1 );  // Bit 31 set to 1 is 0x80000000

    if ( board_hoshi[J] & I ) {
        is_hoshi_point = true;
    }

    return is_hoshi_point;
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
    int   J;
    row_t I;
    int   index_1d;
    int   i, j;

    // Maybe this should be moved to init_board():
    memset( worm_nr[BLACK+1], 0, board_size * board_size * sizeof( unsigned short ) );
    memset( worm_nr[WHITE+1], 0, board_size * board_size * sizeof( unsigned short ) );
    memset( worm_nr[EMPTY+1], 0, board_size * board_size * sizeof( unsigned short ) );
    worm_nr_max[BLACK+1] = 0;
    worm_nr_max[WHITE+1] = 0;
    worm_nr_max[EMPTY+1] = 0;

    for ( J = 1; J <= board_size; J++ ) {
        I = 0x80000000 >> 1;
        j = J - 1;
        i = -1;
        while ( board_on[1] & I ) {
            i++;
            index_1d = j * board_size + i;

            // Check for worm number:
            if ( ( worm_nr[BLACK+1][index_1d] & worm_nr[WHITE+1][index_1d] & worm_nr[EMPTY+1][index_1d] ) == 0 ) {
                create_worm( I, J, index_1d );
            }

            // Current vertex:
            if ( board_black[J] & I ) {
            }
            else if ( board_white[J] & I ) {
            }
            else {
            }

            // North vertex:
            J++;
            if ( board_on[J] & I ) {
                if ( board_black[J] & I ) {
                }
                else if ( board_white[J] & I ) {
                }
                else {
                }
            }
            J--;
            // South vertex:
            J--;
            if ( board_on[J] & I ) {
                if ( board_black[J] & I ) {
                }
                else if ( board_white[J] & I ) {
                }
                else {
                }
            }
            J++;
            // East vertex:
            I >>= 1;
            if ( board_on[J] & I ) {
                if ( board_black[J] & I ) {
                }
                else if ( board_white[J] & I ) {
                }
                else {
                }
            }
            I <<= 1;
            // West vertex:
            I <<= 1;
            if ( board_on[J] & I ) {
                if ( board_black[J] & I ) {
                }
                else if ( board_white[J] & I ) {
                }
                else {
                }
            }
            I >>= 2;

            //I >>= 1;
        }
    }

    return;
}

/**
 * @brief       Creates worms.
 *
 * Creates worms by assigning worm numbers to BLACK, WHITE and EMPTY fields.
 *
 * @param[in]   I           Horizontal coordinate for binary board.
 * @param[in]   J           Vertical coordinate for binary board.
 * @param[in]   index_1d    Coordinate for 1D-board.
 * @return      Nothing
 * @note        This is a recursive function.
 */
void create_worm( row_t I, int J, int index_1d )
{
    int n;
    unsigned short worm_nr_min = USHRT_MAX;

    int color    = get_vertex_intern( I, J );
    int count    = 0;

    vertex_t neighbours[4];
    memset( neighbours, 0, sizeof(vertex_t) * 4 );

    // Check neighbour NORTH:
    J++;
    if ( board_on[J] & I ) {
        if ( get_vertex_intern( I, J ) == color ) {
            neighbours[count].I = I;
            neighbours[count].J = J;
            neighbours[count].index_1d = index_1d + board_size;
            neighbours[count].worm_nr  = worm_nr[color+1][index_1d + board_size];

            if ( neighbours[count].worm_nr > 0 && neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    J--;
    // Check neighbour EAST:
    I >>= 1;
    if ( board_on[J] & I ) {
        if ( get_vertex_intern( I, J ) == color ) {
            neighbours[count].I = I;
            neighbours[count].J = J;
            neighbours[count].index_1d = index_1d + 1;
            neighbours[count].worm_nr  = worm_nr[color+1][index_1d + 1];

            if ( neighbours[count].worm_nr > 0 && neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    I <<= 1;
    // Check neighbour SOUTH:
    J--;
    if ( board_on[J] & I ) {
        if ( get_vertex_intern( I, J ) == color ) {
            neighbours[count].I = I;
            neighbours[count].J = J;
            neighbours[count].index_1d = index_1d - board_size;
            neighbours[count].worm_nr  = worm_nr[color+1][index_1d - board_size];

            if ( neighbours[count].worm_nr > 0 && neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    J++;
    // Check neighbour WEST:
    I <<= 1;
    if ( board_on[J] & I ) {
        if ( get_vertex_intern( I, J ) == color ) {
            neighbours[count].I = I;
            neighbours[count].J = J;
            neighbours[count].index_1d = index_1d - 1;
            neighbours[count].worm_nr  = worm_nr[color+1][index_1d - 1];

            if ( neighbours[count].worm_nr > 0 && neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }
    }
    I >>= 1;

    switch (count) {
        case 0:
            // No neighbours of same color; field gets the next worm number:
            worm_nr[color+1][index_1d] = ++worm_nr_max[color+1];
            break;
        case 1:
            // One neighbour of same color:
            if ( neighbours[0].worm_nr ) {
                // Neighbour has worm number:
                worm_nr[color+1][index_1d] = neighbours[0].worm_nr;
            }
            else {
                // Neighbour has no worm number:
                worm_nr[color+1][index_1d] = ++worm_nr_max[color+1];
                create_worm( neighbours[0].I, neighbours[0].J, neighbours[0].index_1d );
            }
            break;
        case 2:
        case 3:
        case 4:
            if ( worm_nr_min == USHRT_MAX ) {
                // All neighbours are without worm_nr:
                worm_nr[color+1][index_1d] = ++worm_nr_max[color+1];
                for ( n = 0; n < count; n++ ) {
                    create_worm( neighbours[n].I, neighbours[n].J, neighbours[n].index_1d );
                }
            }
            else {
                // At least one neighbour has a worm number; we take the
                // lowest one:
                worm_nr[color+1][index_1d] = worm_nr_min;
                for ( n = 0; n < count; n++ ) {
                    if ( neighbours[n].worm_nr != worm_nr_min ) {
                        create_worm( neighbours[n].I, neighbours[n].J, neighbours[n].index_1d );
                    }
                }
            }
            break;
    }

    return;
}


/**
 * @brief       TEST: Prints worms
 *
 * TEST: Prints worms
 *
 * @note        May be removed later!
 */
void print_worms(void)
{
    int i;

    for ( i = 0; i < board_size * board_size; i++ ) {
        if ( ( i % (board_size) ) == 0 ) {
            printf("\n");
        }
        printf(" %d ", worm_nr[BLACK+1][i] );
    }
    printf("\n");
    printf("\n");
    for ( i = 0; i < board_size * board_size; i++ ) {
        if ( ( i % (board_size) ) == 0 ) {
            printf("\n");
        }
        printf(" %d ", worm_nr[WHITE+1][i] );
    }
    printf("\n");
    printf("\n");
    for ( i = 0; i < board_size * board_size; i++ ) {
        if ( ( i % (board_size) ) == 0 ) {
            printf("\n");
        }
        printf(" %d ", worm_nr[EMPTY+1][i] );
    }
    printf("\n");

    return;
}


