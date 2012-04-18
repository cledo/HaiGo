#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "../src/global_const.h"
#include "board_intern.h"
#include "board.h"


/**
 * @file    board.c
 *
 * @brief   Board data structures and functions.
 *
 * Represents all board data structures and their
 * necessary functions.
 *
 */


//! Value of the board array that indicates an index which is off the board.
#define BOARD_OFF   INT_MAX

//! Macro that converts two dimensional index into one dimension.
#define INDEX(i,j)  ( ( ( (j)+1 ) * ( board_size+1 ) ) + (i) )

#define BLACK_INDEX 2   //!< Array index for BLACK; must be BLACK + 1
#define WHITE_INDEX 0   //!< Array index for WHITE; must be WHITE + 1
#define EMPTY_INDEX 1   //!< Array index for EMPTY; must be EMPTY + 1

//////////////////////////////
//                          //
//  Board data structures   //
//                          //
//////////////////////////////

bsize_t board_size = BOARD_SIZE_DEFAULT;    //!< Sets the boardsize to the default.
int index_1d_max;   //!< The maximum 1d index without highest off board row.

int *board;         //!< Board data structures wich holds color per field.
int *board_hoshi;   //!< Board that defines star points.

// TEST!
int  remove_worm( int index_1d );
void update_worm_data( int index_1d, int color_index );


//////////////////////////////
//                          //
//  Worm data structures    //
//                          //
//////////////////////////////


worm_nr_t MAX_WORM_COUNT;   //!< Stores the maximum of possible worms for one color.

worm_nr_t *worm_board[3] ;  //!< Three 1D-Boards with worm numbers (for WHITE_INDEX,EMPTY_INDEX,BLACK_INDEX).
worm_nr_t worm_nr_max[3];   //!< List of current highest worm numbers (for WHITE_INDEX,EMPTY_INDEX,BLACK_INDEX).

worm_t worm_list[3][BOARD_SIZE_MAX * BOARD_SIZE_MAX / 2];       //!< List of worm structs for black. Index is worm_nr.


//! Struct with coordinates for different board types and additional data.
typedef struct {
    int   index_1d;         //!< Coordinate for 1D-board.
    worm_nr_t worm_nr;      //!< Worm number.
} vertex_t;


//////////////////////////////
//                          //
//  Evaluation data         //
//                          //
//////////////////////////////

int count_color[3];     //!< Number of WHITE, EMPTY, BLACK on board.

int captured_by_black;  //!< Number of white stones captured by black.
int captured_by_white;  //!< Number of black stones captured by white.

int *removed[3];        //!< List of 1d-indexes where stones have been removed by remove_stones().
int removed_max[3];     //!< Counts the number of elements in *removed[3].


/**
 * @name    Board data structures
 *
 * Setting up initial board data and freeing board data.
 *
 */

//@{

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
    int index_1d;
    int board_index_max = ( board_size + 1 ) * ( board_size + 2 ) - 1;

    set_board_size(board_size);

    board            = malloc( ( (board_size+1) * (board_size+2) * sizeof(int) ) );
    board_hoshi      = malloc( ( (board_size+1) * (board_size+2) * sizeof(int) ) );
    worm_board[BLACK_INDEX] = malloc( ( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    worm_board[WHITE_INDEX] = malloc( ( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    worm_board[EMPTY_INDEX] = malloc( ( (board_size+1) * (board_size+2) * sizeof(worm_nr_t) ) );
    if ( board == NULL || board_hoshi == NULL ) {
        fprintf( stderr, "cannot allocate memory for board\n" );
        exit(EXIT_FAILURE);
    }
    if ( worm_board[BLACK_INDEX] == NULL || worm_board[WHITE_INDEX] == NULL || worm_board[EMPTY_INDEX] == NULL ) {
        fprintf( stderr, "cannot allocate memory for worm_board\n" );
        exit(EXIT_FAILURE);
    }

    // Initialise board data structures:
    for ( index_1d = 0; index_1d <= board_index_max; index_1d++ ) {
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
            board[index_1d]       = EMPTY;
            board_hoshi[index_1d] = EMPTY;
        }
        worm_board[BLACK_INDEX][index_1d] = EMPTY;
        worm_board[WHITE_INDEX][index_1d] = EMPTY;
        worm_board[EMPTY_INDEX][index_1d] = EMPTY;
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

    captured_by_black = 0;
    captured_by_white = 0;

    removed[WHITE_INDEX] = malloc( board_size * board_size * sizeof(int) );
    removed[EMPTY_INDEX] = malloc( board_size * board_size * sizeof(int) );
    removed[BLACK_INDEX] = malloc( board_size * board_size * sizeof(int) );

    removed_max[BLACK_INDEX] = removed_max[WHITE_INDEX] = removed_max[EMPTY_INDEX] = 0;

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

    free(worm_board[BLACK_INDEX]);
    free(worm_board[WHITE_INDEX]);
    free(worm_board[EMPTY_INDEX]);

    worm_board[BLACK_INDEX] = NULL;
    worm_board[WHITE_INDEX] = NULL;
    worm_board[EMPTY_INDEX] = NULL;

    free(removed[BLACK_INDEX]);
    free(removed[EMPTY_INDEX]);
    free(removed[WHITE_INDEX]);

    removed[WHITE_INDEX] = NULL;
    removed[EMPTY_INDEX] = NULL;
    removed[BLACK_INDEX] = NULL;

    return;
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

    board_size   = size;
    index_1d_max = ( board_size + 1 ) * ( board_size + 1 ) - 1;

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


//@}

/**
 * @name    Board scanning functions.
 *
 * Scan the board for static evaluation data.
 *
 */

//@{

/**
 * @brief       Level 1 of board scan.
 *
 * Scans the board to create data necessary for making a valid move.
 * Level 1 scan.
 *
 * @return      Nothing
 * @sa          scan_board_2()
 * @note        This replaces the former create_groups() function.
 */
void scan_board_1(void)
{
    int k;
    int index_1d;
    int color;

    // Only those functions should be called here, which are necessary to make
    // a valid move! Consider this to be kind of a scan level nr. 1.

    // Maybe this should be moved to init_board():
    //memset( worm_board[BLACK_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    //memset( worm_board[WHITE_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    //memset( worm_board[EMPTY_INDEX], 0, (board_size+1) * (board_size+2) * sizeof(worm_nr_t));
    for ( k = 0; k < (board_size+1) * (board_size+2); k++ ) {
        worm_board[BLACK_INDEX][k] = 0;
        worm_board[WHITE_INDEX][k] = 0;
        worm_board[EMPTY_INDEX][k] = 0;
    }
    worm_nr_max[BLACK_INDEX] = 0;
    worm_nr_max[WHITE_INDEX] = 0;
    worm_nr_max[EMPTY_INDEX] = 0;

    /*
    memset( worm_list[BLACK_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );
    memset( worm_list[WHITE_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );
    memset( worm_list[EMPTY_INDEX], 0, MAX_WORM_COUNT * sizeof(worm_t) );
    */

    for ( k = 0; k < MAX_WORM_COUNT; k++ ) {
        worm_list[BLACK_INDEX][k].number = 0;
        worm_list[EMPTY_INDEX][k].number = 0;
        worm_list[WHITE_INDEX][k].number = 0;
    }

    // First scan:
    // Gives a worm_nr to every field.
    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {
        // Current vertex is index_1d.

        if ( board[index_1d] == BOARD_OFF ) {
            continue;
        }

        // Check for worm number:
        if ( ( worm_board[BLACK_INDEX][index_1d] & worm_board[WHITE_INDEX][index_1d] & worm_board[EMPTY_INDEX][index_1d] ) == 0 ) {
            color = board[index_1d];
            create_worm_data( index_1d, color+1 );  // Should be renamed to fill_worm_board() probably.
        }
    }

    // Second scan:
    // Build list of worm structs.
    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {

        if ( board[index_1d] == BOARD_OFF ) {
            continue;
        }
        build_worms(index_1d);  // Should probably renamed to fill_worm_list() ....

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
 * @brief       Updates scan level 1 information
 *
 * Performs incremental update of worm boards, worm lists, and liberty count.
 *
 * @param[in]   i       Horizontal coordinate
 * @param[in]   j       Vertical coordinate
 * @return      Nothing
 * @sa          scan_board_1()
 * @note        This function must work on the same data structures as
 *              scan_board_1().
 * @note        The following steps are performed:
 *              - worm_board EMPTY set to zero for newly added stone.
 *              - worm-board for color is updated, so new stone has either new
 *                or existing worm_nr.
 *              - worm list for opposite color is updated to catch zero
 *                liberty worms of opposing color. This is only done for
 *                neighbouring worms of the opposite color.
 *              - If neighbouring worm of opposite color has its liberties
 *                reduced to zero, that worm is removed.
 *              - If worm has been removed, worm board empty is updated.
 *              - If worms of opposite color have been removed, they are
 *                deleted from worm list.
 *
 * @todo        worm lists must be updated (which? all of them?)
 * @todo        Newly added worm list entry has not been given correct number
 *              of liberties.
 * @todo        Worms of own color have to be merged.
 */
void scan_board_1_upd( int i, int j )
{
    int k;
    int index;
    int count = 0;
    int index_1d            = INDEX(i,j);
    int color               = board[index_1d];
    int color_index         = board[index_1d] + 1;
    worm_nr_t worm_nr;
    //worm_nr_t empty_worm_nr = worm_board[EMPTY_INDEX][index_1d];

    worm_board[EMPTY_INDEX][index_1d] = 0;

    worm_nr = worm_nr_max[color_index];

    // Update worm board for given color:
    //create_worm_data( index_1d, color_index );
    update_worm_data( index_1d, color_index );

    // Update worm list for opposite color:
    index = index_1d + board_size + 1;
    if ( board[index] != BOARD_OFF ) {
        worm_nr = worm_board[ ( color * -1 ) + 1 ][index];
        if ( worm_nr ) {
            // Reduce liberties, remove worm:
            if ( ( worm_list[ ( color * -1 ) + 1 ][worm_nr].liberties -= 12 ) ==  0 ) {
                count += remove_worm(index);
                //printf( "## Removing worm color: %d, Nr.: %hu\n", color * -1, worm_nr );
                // Update worm board EMPTY:
                //create_worm_data( index, EMPTY_INDEX ); // Necessary here??

                // Remove from worm list:
                worm_list[ ( color * -1 ) + 1 ][worm_nr].number = 0;
            }
        }
    }

    index = index_1d + 1;
    if ( board[index] != BOARD_OFF ) {
        worm_nr = worm_board[ ( color * -1 ) + 1 ][index];
        if ( worm_nr ) {
            // Reduce liberties:
            if ( ( worm_list[ ( color * -1 ) + 1 ][worm_nr].liberties -= 12 ) ==  0 ) {
                count += remove_worm(index);
                //printf( "## Removing worm color: %d, Nr.: %hu\n", color * -1, worm_nr );
                // Update worm board EMPTY:
                //create_worm_data( index, EMPTY_INDEX );

                // Remove from worm list:
                worm_list[ ( color * -1 ) + 1 ][worm_nr].number = 0;
            }
        }
    }

    index = index_1d - board_size - 1;
    if ( board[index] != BOARD_OFF ) {
        worm_nr = worm_board[ ( color * -1 ) + 1 ][index];
        if ( worm_nr ) {
            // Reduce liberties:
            if ( ( worm_list[ ( color * -1 ) + 1 ][worm_nr].liberties -= 12 ) ==  0 ) {
                count += remove_worm(index);
                //printf( "## Removing worm color: %d, Nr.: %hu\n", color * -1, worm_nr );
                // Update worm board EMPTY:
                //create_worm_data( index, EMPTY_INDEX );

                // Remove from worm list:
                worm_list[ ( color * -1 ) + 1 ][worm_nr].number = 0;
            }
        }
    }

    index = index_1d - 1;
    if ( board[index] != BOARD_OFF ) {
        worm_nr = worm_board[ ( color * -1 ) + 1 ][index];
        if ( worm_nr ) {
            // Reduce liberties:
            if ( ( worm_list[ ( color * -1 ) + 1 ][worm_nr].liberties -= 12 ) ==  0 ) {
                count += remove_worm(index);
                //printf( "## Removing worm color: %d, Nr.: %hu\n", color * -1, worm_nr );
                // Update worm board EMPTY:
                //create_worm_data( index, EMPTY_INDEX );

                // Remove from worm list:
                worm_list[ ( color * -1 ) + 1 ][worm_nr].number = 0;
            }
        }
    }

    // Check if current worm has liberties, otherwise move is illegal:
    // Get worm number of current stone:
    worm_nr = worm_board[color_index][index_1d];
    // Reset liberties of current worm:
    worm_list[color_index][worm_nr].liberties = 0;
    // Count liberties of current worm:
    for ( k = 0; k < worm_list[color_index][worm_nr].count; k++ ) {
        count_worm_liberties( worm_list[color_index][worm_nr].index[k]  );
    }


    if (count) {
        if ( color == BLACK ) {
            set_black_captured( get_black_captured() + count );
        }
        else {
            set_white_captured( get_white_captured() + count );
        }
    }

    return;
}

int remove_worm( int index_1d )
{
    int index;
    int count         = 0;
    int color         = board[index_1d];
    worm_nr_t worm_nr = worm_board[ color + 1 ][index_1d];

    board[index_1d] = EMPTY;
    worm_board[ color + 1 ][index_1d] = EMPTY;
    //worm_board[EMPTY_INDEX][index_1d] = get_free_worm_nr(EMPTY);
    worm_board[EMPTY_INDEX][index_1d] = get_free_worm_nr(EMPTY);
    // TODO: worm_nr must be increased somewhere ...

    count++;

    index = index_1d + board_size + 1;
    if ( board[index] != BOARD_OFF && worm_board[ color + 1 ][index] == worm_nr ) {
        count += remove_worm(index);
        // If Stones have been removed we have a new empty worm:
    }

    index = index_1d + 1;
    if ( board[index] != BOARD_OFF && worm_board[ color + 1 ][index] == worm_nr ) {
        count += remove_worm(index);
    }

    index = index_1d - board_size - 1;
    if ( board[index] != BOARD_OFF && worm_board[ color + 1 ][index] == worm_nr ) {
        count += remove_worm(index);
    }

    index = index_1d - 1;
    if ( board[index] != BOARD_OFF && worm_board[ color + 1 ][index] == worm_nr ) {
        count += remove_worm(index);
    }

    return count;
}

int get_worm_liberty_count( int i, int j )
{
    int index_1d      = INDEX(i,j);
    int color_index   = board[index_1d] + 1;
    worm_nr_t worm_nr = worm_board[color_index][index_1d];

    return get_nr_of_liberties(worm_nr);
}

/**
 * @brief       Level 2 of board scan.
 *
 * Scan level 2: scans the board for creating data needed for evaluation.
 *
 * @return      Nothing
 * @sa          scan_board_1()
 */
void scan_board_2(void)
{
    int index_1d;
    int color;

    count_color[BLACK_INDEX] = count_color[WHITE_INDEX] = count_color[EMPTY_INDEX] = 0;

    for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {
        color = board[index_1d];

        if ( color == BOARD_OFF ) {
            continue;
        }

        // Count stones on board:
        count_color[color+1]++;
    }

    return;
}

/**
 * @brief       Creates board data for worms.
 *
 * Creates worms by assigning worm numbers to BLACK, WHITE and EMPTY fields.
 *
 * @param[in]   index_1d    Coordinate for 1D-board.
 * @param[in]   color_index Color index of the given field ( which is color +  1 ).
 * @return      Nothing
 * @note        This is a recursive function.
 */
void create_worm_data( int index_1d, int color_index )
{
    int n;
    int i;
    int count = 0;
    worm_nr_t worm_nr_min = USHRT_MAX;
    worm_nr_t *worm_board_color = worm_board[color_index];

    vertex_t neighbours[4];

    neighbours[0].worm_nr = 0;
    neighbours[1].worm_nr = 0;
    neighbours[2].worm_nr = 0;
    neighbours[3].worm_nr = 0;

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour EAST:
    i = index_1d + 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour WEST:
    i = index_1d - 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }

    switch (count) {
        case 0:
            worm_board_color[index_1d] = ++worm_nr_max[color_index];
            break;
        case 1:
            worm_board_color[index_1d] = neighbours[0].worm_nr;
            break;
        case 2:
        case 3:
        case 4:
            // We take the lowest of all neighbouring worm numbers:
            worm_board_color[index_1d] = worm_nr_min;
            for ( n = 0; n < count; n++ ) {
                // Call create_worm() on all neighbours that have not the
                // minimum worm number:
                if ( neighbours[n].worm_nr > worm_nr_min ) {
                    create_worm_data( neighbours[n].index_1d, color_index );
                }
            }
            break;
    }

    return;
}

void update_worm_data( int index_1d, int color_index )
{
    int n;
    int i;
    int count = 0;
    worm_nr_t worm_nr_min       = USHRT_MAX;
    worm_nr_t *worm_board_color = worm_board[color_index];
    worm_t    *worm_list_color  = worm_list[color_index];
    worm_nr_t worm_nr;

    vertex_t neighbours[4];

    neighbours[0].worm_nr = 0;
    neighbours[1].worm_nr = 0;
    neighbours[2].worm_nr = 0;
    neighbours[3].worm_nr = 0;

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour EAST:
    i = index_1d + 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }
    // Check neighbour WEST:
    i = index_1d - 1;
    if ( board[i] != BOARD_OFF && worm_board_color[i] ) {
        neighbours[count].index_1d = i;
        neighbours[count].worm_nr  = worm_board_color[i];

        if ( neighbours[count].worm_nr < worm_nr_min ) {
            worm_nr_min = neighbours[count].worm_nr;
        }
        count++;
    }

    switch (count) {
        case 0:
            // New worm is added:
            worm_nr = ++worm_nr_max[color_index];
            // Updating worm board:
            worm_board_color[index_1d] = worm_nr;
            // Updating worm list:
            worm_list_color[worm_nr].number    = worm_nr;
            worm_list_color[worm_nr].count     = 1;
            worm_list_color[worm_nr].liberties = 0; // Number of liberties not known yet.
            worm_list_color[worm_nr].index[0]  = index_1d;
            break;
        case 1:
            // New stone is merged with existing worm:
            worm_nr = neighbours[0].worm_nr;
            // Updating worm board:
            worm_board_color[index_1d] = worm_nr;
            // Updating worm list:
            worm_list_color[worm_nr].index[ worm_list_color[worm_nr].count ] = index_1d;
            worm_list_color[worm_nr].count += 1;
            break;
        case 2:
        case 3:
        case 4:
            // 2 to 4 worms are merged with the lowest found worm number:
            worm_nr = worm_nr_min;
            // Updating worm board:
            worm_board_color[index_1d] = worm_nr;
            for ( n = 0; n < count; n++ ) {
                // Call update_worm_data() on all neighbours that do not have the
                // minimum worm number:
                if ( neighbours[n].worm_nr > worm_nr ) {
                    update_worm_data( neighbours[n].index_1d, color_index );
                    // Remove merged worm from list:
                    worm_list_color[ neighbours[n].worm_nr].number = 0;
                    // Update merging worm count:
                    worm_list_color[worm_nr].index[ worm_list_color[worm_nr].count ] = index_1d;
                    worm_list_color[worm_nr].count += 1;
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
    int color_index           = board[index_1d] + 1;
    worm_nr_t worm_nr_current = worm_board[color_index][index_1d];
    worm_t *w                 = &worm_list[color_index][worm_nr_current];

    if ( w->number == 0 ) {
        w->number    = worm_nr_current;
        w->count     = 1;
        w->liberties = 0;
        w->index[0]  = index_1d;
    }
    else {
        w->index[ w->count ] = index_1d;
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
    int color_i       = board[index_1d] + 1;
    worm_nr_t worm_nr = worm_board[color_i][index_1d];
    worm_t *w         = &worm_list[color_i][worm_nr];

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr, color_i );
        if (count) {
            w->liberties += ( 12 / count );
        }
    }

    // Check neighbour EAST:
    i = index_1d + 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr, color_i );
        if (count) {
            w->liberties += ( 12 / count );
        }
    }

    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr, color_i );
        if (count) {
            w->liberties += ( 12 / count );
        }
    }

    // Check neighbour WEST:
    i = index_1d - 1;
    if ( board[i] == EMPTY ) {
        count = get_worm_neighbours( i, worm_nr, color_i );
        if (count) {
            w->liberties += ( 12 / count );
        }
    }

    //worm_list[color_i][worm_nr] = w;

    return;
}

/**
 * @brief       Counts neighbours of same worm.
 *
 * Counts the the number of neighbouring stones, which are all of the same
 * given color and worm number. This is needed for counting the liberties of a
 * worm.
 *
 * @param[in]   index_1d    1d index for board
 * @param[in]   worm_nr     Worm number
 * @param[in]   color_i     Color turned to index (color + 1)
 * @return      Number of neighbours of given worm.
 * @sa          count_worm_liberties()
 * @note        This is primarily a helper function for count_worm_liberties()
 */
inline int get_worm_neighbours( int index_1d, worm_nr_t worm_nr, int color_i )
{
    int i;
    int count = 0;
    worm_nr_t *w = worm_board[color_i];

    // Check neighbour NORTH:
    i = index_1d + board_size + 1;
    if ( w[i] == worm_nr) {
        count++;
    }

    // Check neighbour EAST:
    i = index_1d + 1;
    if ( w[i] == worm_nr) {
        count++;
    }

    // Check neighbour SOUTH:
    i = index_1d - board_size - 1;
    if ( w[i] == worm_nr) {
        count++;
    }

    // Check neighbour WEST:
    i = index_1d - 1;
    if ( w[i] == worm_nr) {
        count++;
    }

    return count;
}

//@}



/**
 * @name    Evaluation data
 *
 * Functions that retrieve evaluation data.
 *
 */

//@{

/**
 * @brief       Returns number of stones for color
 *
 * Returns the number of stones for a given color, or number of empty fields
 * if color is EMPTY.
 *
 * @param[in]   color   BLACK|WHITE|EMPTY
 * @return      Number of stones or fields
 */
int get_stone_count( int color )
{
    return count_color[color+1];
}

/**
 * @brief       Returns number of worms in atari.
 *
 * For a gievn color the number of worms with only one liberty is returned.
 *
 * @param[in]   color   BLACK|WHITE
 * @return      Number of worms
 * @note        Color EMPTY as parameter does not make sense here.
 */
int get_worm_count_atari( int color )
{
    int k;
    int count       = 0;
    worm_t *w       = worm_list[color+1];
    worm_nr_t w_max = worm_nr_max[color+1];

    for ( k = 1; k <= w_max; k++ ) {
        if ( w[k].liberties == 12 ) {
            count++;
        }
    }

    return count;
}

//@}




/**
 * @name    Vertex related functions
 *
 * Setting and getting vertex information.
 *
 */

//@{

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
 * @param[in]   index_1d    1d index of board
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
 * @brief       Counts number of neighbours.
 *
 * Counts the number of neighbours, which have the same color as the given
 * field. Writes their indexes into neighbour list.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @param[out]  neighbour   List of neighbour indexes
 * @return      Number of neighbour
 */
int has_neighbour( int i, int j, int neighbour[][2] )
{
    int index;
    int index_1d = INDEX(i,j);
    int color = board[index_1d];
    int count = 0;

    // Check NORTH:
    index = index_1d + board_size + 1;
    if ( board[index] == color ) {
        neighbour[count][0] = i;
        neighbour[count][1] = j + 1;
        count++;
    }

    // Check EAST:
    index = index_1d + 1;
    if ( board[index] == color ) {
        neighbour[count][0] = i + 1;
        neighbour[count][1] = j;
        count++;
    }

    // Check SOUTH:
    index = index_1d - board_size - 1;
    if ( board[index] == color ) {
        neighbour[count][0] = i;
        neighbour[count][1] = j - 1;
        count++;
    }

    // Check WEST:
    index = index_1d - 1;
    if ( board[index] == color ) {
        neighbour[count][0] = i - 1;
        neighbour[count][1] = j;
        count++;
    }

    return count;
}

//@}



/**
 * @name    Worm related functions
 *
 * Functions that generate worm related information.
 *
 */

//@{

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

    return worm_list[color+1][worm_nr];
}


/**
 * @brief       Returns worm number.
 *
 * Returns the worm number for the given vertex.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      Worm number or zero for empty field.
 */
int get_worm_nr( int i, int j )
{
    int index_1d      = INDEX(i,j);
    int color         = board[index_1d];
    worm_nr_t worm_nr = worm_board[color+1][index_1d];

    if ( color == EMPTY ) {
        worm_nr = 0;
    }

    return (int)worm_nr * color;
}

/**
 * @brief       Returns the next free and usable worm number.
 *
 * Returns the next free and usable worm number for the given color.
 *
 * @param[in]   color   BLACK|WHITE|EMPTY
 * @return      Worm number
 */
int get_free_worm_nr( int color )
{
    return ( worm_nr_max[color+1] + 1 );
}

/**
 * @brief       Returns number of liberties for given worm number.
 *
 * Returns number of liberties for the given worm number.
 *
 * @param[in]   worm_nr     Number of worm
 * @return      Number of liberties.
 * @note        The saved number must be divided by 12 to get the real number
 * of liberties.
 */
int get_nr_of_liberties( int worm_nr )
{
    int count;
    int color_i;

    if ( worm_nr > 0 ) {
        color_i = BLACK_INDEX;
    }
    else {
        color_i = WHITE_INDEX;
        worm_nr *= -1;
    }
    count = (int)( worm_list[color_i][worm_nr].liberties );

    return count / 12;
}

/**
 * @brief       Removes worms with zero liberties from board.
 *
 * Removes worms of the given color that have no liberties from the board.
 *
 * @param[in]   color   BLACK|WHITE
 * @return      Number of stones removed
 */
int remove_stones( int color )
{
    int count_removed = 0;
    worm_nr_t worm_nr;
    worm_t    *wl      = worm_list[color+1];
    worm_nr_t *wb      = worm_board[color+1];
    worm_nr_t worm_max = worm_nr_max[color+1];
    worm_nr_t zero_worm[worm_max];  // Lists worms with zero liberties.
    int k = 0;
    int l = 0;
    int index_1d;

    removed_max[BLACK_INDEX] = removed_max[WHITE_INDEX] = 0;

    // Go through worm list:
    for ( worm_nr = 1; worm_nr <= worm_max; worm_nr++ ) {
        if ( wl[worm_nr].number != 0 && wl[worm_nr].liberties == 0 ) {
            zero_worm[k++] = worm_nr;
            //wl[worm_nr].number = 0;  // ??
        }
    }
    //printf("Zeros: %d\n", k );

    // Go through boards:
    if ( k > 0 ) {
        // At least one worm has to be removed:
        for ( index_1d = board_size + 1; index_1d < index_1d_max; index_1d++ ) {
            if ( board[index_1d] != color ) {
                continue;
            }
            for ( l = 0; l < k; l++ ) {
                if ( wb[index_1d] == zero_worm[l] ) {
                    //wb[index_1d]    = EMPTY;
                    board[index_1d] = EMPTY;
                    count_removed++;

                    removed[color+1][removed_max[color+1]++] = index_1d;
                }
            }
        }
    }

    if ( color == WHITE ) {
        captured_by_black += count_removed;
    }
    else {
        captured_by_white += count_removed;
    }

    return count_removed;
}

/**
 * @brief       Returns size of given worm.
 *
 * Returns the number of stones that a part of a given worm number.
 *
 * @param[in]   worm_nr     Number of worm
 * @return      Size of worm
 */
int get_size_of_worm( int worm_nr )
{
    int size;
    int color = BLACK;
    if ( worm_nr < 0 ) {
        color = WHITE;
    }

    size = worm_list[color+1][worm_nr*color].count;

    return size;
}

//@}



/**
 * @name    Capture related functions
 *
 * Functions that get and set capture information.
 *
 */

//@{

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
    return captured_by_black;
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
    return captured_by_white;
}

/**
 * @brief       Sets the number of captured stones for black.
 *
 * Sets the number of white stones black has captured.
 *
 * @param[in]   captured    Number of captured stones.
 * @return      Nothing
 * @todo        Maybe this should be replaced by set_captured(color, captured)
 */
void set_black_captured( int captured )
{
    captured_by_black = captured;

    return;
}

/**
 * @brief       Sets the number of captured stones for white.
 *
 * Sets the number of black stones white has captured.
 *
 * @param[in]   captured    Number of captured stones.
 * @return      Nothing
 * @todo        Maybe this should be replaced by set_captured(color, captured)
 */
void set_white_captured( int captured )
{
    captured_by_white = captured;

    return;
}

/**
 * @brief       Returns currently captured stones.
 *
 * Returns number of currently captured stones and writes their indexes into
 * the parameter captured[][2].
 *
 * @param[out]  captured    List of indexes of captured stones.
 * @return      Number of captured stones.
 * @sa          remove_stones()
 * @note        The captured stones are defined by remove_stones().
 */
int get_captured_now( int captured[][2] )
{
    int k;
    int l = 0;
    int i, j;
    int index_1d;

    for ( k = 0; k < removed_max[BLACK_INDEX]; k++ ) {
        index_1d = removed[BLACK_INDEX][k];
        i = index_1d % ( board_size + 1 );
        j = ( index_1d / ( board_size + 1 ) ) - 1;
        captured[l][0] = i;
        captured[l][1] = j;
        l++;
    }
    for ( k = 0; k < removed_max[WHITE_INDEX]; k++ ) {
        index_1d = removed[WHITE_INDEX][k];
        i = index_1d % ( board_size + 1 );
        j = ( index_1d / ( board_size + 1 ) ) - 1;
        captured[l][0] = i;
        captured[l][1] = j;
        l++;
    }

    captured[l][0] = INVALID;
    captured[l][1] = INVALID;

    return l;
}

//@}


/**
 * @name    Output functions
 *
 * Functions that generate strings designed for printing.
 *
 */

//@{

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

//@}

/**
 * @name    Test functions
 *
 * Test functions mostly for unit tests.
 *
 */

//@{

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

//@}

/**
 * @name    Debug functions
 *
 * Functions that show debugging information. These functions may print
 * directly to stdout.
 *
 */

//@{

/**
 * @brief       Prints worm boards
 *
 * Prints worm boards
 *
 * @note        May be removed later!
 */
void print_worm_boards(void)
{
    int i, j;

    printf("\n");
    for ( j = ( board_size + 1 ) * board_size; j > board_size; j -= board_size + 1 ) {
        for ( i = 0; i < board_size; i++ ) {
            printf( " %2hu ", worm_board[BLACK_INDEX][j+i] );
        }
        printf("\n");
    }
    printf("\n");

    printf("\n");
    for ( j = ( board_size + 1 ) * board_size; j > board_size; j -= board_size + 1 ) {
        for ( i = 0; i < board_size; i++ ) {
            printf( " %2hu ", worm_board[WHITE_INDEX][j+i] );
        }
        printf("\n");
    }
    printf("\n");

    printf("\n");
    for ( j = ( board_size + 1 ) * board_size; j > board_size; j -= board_size + 1 ) {
        for ( i = 0; i < board_size; i++ ) {
            printf( " %2hu ", worm_board[EMPTY_INDEX][j+i] );
        }
        printf("\n");
    }
    printf("\n");

    return;
}

/**
 * @brief       Prints worm structs list
 *
 * Prints worm structs list.
 *
 * @note        May be removed later.
 */
void print_worm_lists(void)
{
    int i, j;
    worm_t *w;

    printf( "Black worm list:\n" );
    w = worm_list[BLACK_INDEX];
    for ( i = 0; i < MAX_WORM_COUNT; i++ ) {
        if ( w[i].number == 0 ) {
            continue;
        }

        printf( "Nr.: %hu\tCount: %hu Lib: %hu "
            , w[i].number, w[i].count, w[i].liberties / 12 );

        printf("(");
        for ( j = 0; j < w[i].count; j++ ) {
            printf( "%d ", w[i].index[j] );
        }
        printf(")\n");
    }


    printf( "White worm list:\n" );
    w = worm_list[WHITE_INDEX];
    for ( i = 0; i < MAX_WORM_COUNT; i++ ) {
        if ( w[i].number == 0 ) {
            continue;
        }

        printf( "Nr.: %hu\tCount: %hu Lib: %hu "
            , w[i].number, w[i].count, w[i].liberties / 12 );

        printf("(");
        for ( j = 0; j < w[i].count; j++ ) {
            printf( "%d ", w[i].index[j] );
        }
        printf(")\n");
    }

    printf( "Empty worm list:\n" );
    w = worm_list[EMPTY_INDEX];
    for ( i = 0; i < MAX_WORM_COUNT; i++ ) {
        if ( w[i].number == 0 ) {
            continue;
        }

        printf( "Nr.: %hu\tCount: %hu Lib: %hu "
            , w[i].number, w[i].count, w[i].liberties / 12 );

        printf("(");
        for ( j = 0; j < w[i].count; j++ ) {
            printf( "%d ", w[i].index[j] );
        }
        printf(")\n");
    }

    return;
}

/**
 * @brief       Prints arrays of removed stones.
 *
 * Prints arrays for BLACK and WHITE with the 1d indexes of removed stones.
 * This function is for testing only.
 *
 * @return      Nothing
 */
void print_removed(void)
{
    int k;

    printf( "## removed Black: " );
    for ( k = 0; k < removed_max[BLACK_INDEX]; k++ ) {
        printf( "%d, ", removed[BLACK_INDEX][k] );
    }
    printf("\n");

    printf( "## removed White: " );
    for ( k = 0; k < removed_max[WHITE_INDEX]; k++ ) {
        printf( "%d, ", removed[WHITE_INDEX][k] );
    }
    printf("\n");

    return;
}

//@}

