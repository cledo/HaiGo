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

//! Data structure representing a worm.
typedef struct worm_st {
    unsigned short number;  //!< Worm number
} worm_t;

unsigned short MAX_WORM_COUNT;  //!< Stores the maximum of possible worms for one color.

unsigned short *worm_nr[3] ;    //!< Three 1D-Boards with worm numbers (for WHITE+1,EMPTY+1,BLACK+1)
unsigned short worm_nr_max[3];  //!< List of current highest worm numbers (for WHITE+1,EMPTY+1,BLACK+1).

worm_t *black_worms;    //!< List of worm structs for black.
worm_t *white_worms;    //!< List of worm structs for white.

int *liberty[3];        //!< List of empty fields (index_1d) that are a liberty to worm.
int liberty_nr_max[3];  //!< Current highest index of *liberty[color+1]

//! Struct with coordinates for different board types and additional data.
typedef struct {
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
    //int i = 0;
    //int j = 0;
    int index_1d;
    int index_1d_max;

    set_board_size(board_size);
    index_1d_max = (board_size+1) * (board_size+2) - 1; // Make macro of this ...

    board            = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    board_hoshi      = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    worm_nr[BLACK+1] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    worm_nr[WHITE+1] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    worm_nr[EMPTY+1] = malloc( (size_t)( (board_size+1) * (board_size+2) * sizeof(int) ) );
    if ( board == NULL || board_hoshi == NULL ) {
        fprintf( stderr, "cannot allocate memory for board\n" );
        exit(EXIT_FAILURE);
    }
    if ( worm_nr[BLACK+1] == NULL || worm_nr[WHITE+1] == NULL || worm_nr[EMPTY+1] == NULL ) {
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
        worm_nr[BLACK+1][index_1d] = EMPTY;
        worm_nr[WHITE+1][index_1d] = EMPTY;
        worm_nr[EMPTY+1][index_1d] = EMPTY;
    }

    worm_nr_max[BLACK+1] = 0;
    worm_nr_max[WHITE+1] = 0;
    worm_nr_max[EMPTY+1] = 0;

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
        exit(EXIT_FAILURE);
    }

    liberty[BLACK+1] = malloc( board_size * board_size * sizeof(int) );
    liberty[WHITE+1] = malloc( board_size * board_size * sizeof(int) );

    liberty_nr_max[BLACK+1] = 0;
    liberty_nr_max[WHITE+1] = 0;
    
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

    free(black_worms);
    free(white_worms);
    free(worm_nr[BLACK+1]);
    free(worm_nr[WHITE+1]);
    free(worm_nr[EMPTY+1]);

    free(liberty[BLACK+1]);
    free(liberty[WHITE+1]);

    black_worms      = NULL;
    white_worms      = NULL;
    worm_nr[BLACK+1] = NULL;
    worm_nr[WHITE+1] = NULL;
    worm_nr[EMPTY+1] = NULL;

    liberty[BLACK+1] = NULL;
    liberty[WHITE+1] = NULL;

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
    bool is_on    = false;
    int  index_1d = INDEX(i,j);

    if ( index_1d < 0 ) {
        return false;
    }

    if ( board[index_1d] != BOARD_OFF ) {
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
    bool is_hoshi_point = false;

    if ( board_hoshi[ INDEX(i,j) ] ) {
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
    int index_1d;
    int color;

    // Only those functions should be called here, which are necessary to make
    // a valid move! Consider this to be kind of a scan level nr. 1.

    // Maybe this should be moved to init_board():
    memset( worm_nr[BLACK+1], 0, (board_size+1) * (board_size+2) * sizeof(int));
    memset( worm_nr[WHITE+1], 0, (board_size+1) * (board_size+2) * sizeof(int));
    memset( worm_nr[EMPTY+1], 0, (board_size+1) * (board_size+2) * sizeof(int));
    worm_nr_max[BLACK+1] = 0;
    worm_nr_max[WHITE+1] = 0;
    worm_nr_max[EMPTY+1] = 0;

    memset( liberty[BLACK+1], 0, board_size * board_size * sizeof(int) );
    memset( liberty[WHITE+1], 0, board_size * board_size * sizeof(int) );
    liberty_nr_max[BLACK+1] = 0;
    liberty_nr_max[WHITE+1] = 0;

    for ( index_1d = board_size + 1; index_1d < (board_size+1) * (board_size+1) - 1; index_1d++ ) {

        // Current vertex is index_1d.

        if ( board[index_1d] == BOARD_OFF ) {
            continue;
        }

        // Check for worm number:
        if ( ( worm_nr[BLACK+1][index_1d] & worm_nr[WHITE+1][index_1d] & worm_nr[EMPTY+1][index_1d] ) == 0 ) {
            color = board[index_1d];
            create_worm( index_1d, color );
        }
    }

    printf( "# LibBlack: %d\n", liberty_nr_max[BLACK+1] );
    printf( "# LibWhite: %d\n", liberty_nr_max[WHITE+1] );

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
 * @param[in]   color       Color of the given field.
 * @return      Nothing
 * @note        This is a recursive function.
 */
void create_worm( int index_1d, int color )
{
    int n;
    int i;
    int count = 0;
    unsigned short worm_nr_min = USHRT_MAX;
    int color_index = color + 1;
    int neighbour_color;

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

        // Check for BLACK/WHITE neighbour if field is EMPTY:
        if ( board[index_1d] == EMPTY && board[i] != EMPTY ) {
            neighbour_color = board[i] + 1;
            liberty[neighbour_color][ liberty_nr_max[neighbour_color]++ ] = index_1d;
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

        // Check for BLACK/WHITE neighbour if field is EMPTY:
        if ( board[index_1d] == EMPTY && board[i] != EMPTY ) {
            neighbour_color = board[i] + 1;
            liberty[neighbour_color][ liberty_nr_max[neighbour_color]++ ] = index_1d;
        }
    }
    // Check neighbour SOUTH:
    i = index_1d - (board_size + 1);
    if ( board[i] != BOARD_OFF ) {
        if ( worm_nr[color_index][i] ) {
            neighbours[count].index_1d = i;
            neighbours[count].worm_nr  = worm_nr[color_index][i];

            if ( neighbours[count].worm_nr < worm_nr_min ) {
                worm_nr_min = neighbours[count].worm_nr;
            }
            count++;
        }

        // Check for BLACK/WHITE neighbour if field is EMPTY:
        if ( board[index_1d] == EMPTY && board[i] != EMPTY ) {
            neighbour_color = board[i] + 1;
            liberty[neighbour_color][ liberty_nr_max[neighbour_color]++ ] = index_1d;
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

        // Check for BLACK/WHITE neighbour if field is EMPTY:
        if ( board[index_1d] == EMPTY && board[i] != EMPTY ) {
            neighbour_color = board[i] + 1;
            liberty[neighbour_color][ liberty_nr_max[neighbour_color]++ ] = index_1d;
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
                    create_worm( neighbours[n].index_1d, color );
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

    printf("\n");
    for ( i = board_size + 1; i < (board_size+1) * (board_size+2) - 1; i++ ) {
        if ( board[i] == BOARD_OFF ) {
            if ( ( ( i + 1 ) % ( board_size + 1 ) ) == 0 ) {
                printf("\n");
            }
            continue;
        }
        printf( " %2d ", worm_nr[BLACK+1][i] );
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
        printf( " %2d ", worm_nr[WHITE+1][i] );
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
        printf( " %2d ", worm_nr[EMPTY+1][i] );
    }
    printf("\n");

    return;
}

