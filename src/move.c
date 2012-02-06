#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "global_const.h"
#include "move.h"


//! The number of the latest move.
int move_number = 0;

/**
 * @brief   Structure that represents a move.
 *
 **/
struct move {
    int  number;    //!< The number of the move in the move history.
    int  color;     //!< Color of the current stone.
    bool pass;      //!< Indicates whether this move is a pass os not.
    int  i;         //!< The horizontal coordinate of the vertex.
    int  j;         //!< The vertical coordinate of the vertex.
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];    //!< List of captured stones; In move zero: List of handicap stones.
    int  ko[2];     //!< Saves the vertex of the last ko field.
} next_move;

//! Move history: contains all moves performed.
struct move move_history[MOVE_HISTORY_MAX];


/**
 * @brief       Initialises the move_history data structure.
 *
 * This function initialises the move_history data structure by setting all
 * move numbers to -1.
 *
 * @return      Nothing
 */
void init_move_history(void)
{
    int k;

    for ( k = 0; k < MOVE_HISTORY_MAX; k++ ) {
        move_history[k].number = -1;
        move_history[k].ko[0]  = -1;
        move_history[k].ko[1]  = -1;
    }

    return;
};

/**
 * @brief       Returns next move number.
 *
 * Returns the number of the next move to be made.
 *
 * @return      Number of next move.
 */
int get_next_move_nr(void)
{

    return move_number + 1;
}

/**
 * @brief       Creates a move structure with default values.
 *
 * The variable next_move is filled with default values; the old values are
 * overwritten. This structure represents the next move. Other functions may
 * change the default values.
 *
 * @return      Nothing
 */
void create_next_move(void)
{
    int k;

    next_move.number = ++move_number;
    next_move.color  = EMPTY;
    next_move.pass   = true;
    next_move.i      = -1;
    next_move.j      = -1;
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        next_move.stones[k][0] = -1;
        next_move.stones[k][1] = -1;
    }
    next_move.ko[0] = -1;
    next_move.ko[1] = -1;

    return;
}

/**
 * @brief       Sets the color and the vertex coordinates of the next move.
 *
 * The color of the current stone is set to BLACK or WHITE. The vertex
 * coordinated are set. The pass variable of the next move is set to false
 * then. The function create_next_move() must be called before
 * set_move_vertex().
 *
 * @param[in]   color   Color of the current stone
 * @param[in]   i       Horizontal coordinate
 * @param[in]   j       Vertical coordinate
 * @return      Nothing
 * @note        Remember that the function create_next_move() must be called
 *              first.
 */
void set_move_vertex( int color, int i, int j )
{
    next_move.color = color;
    next_move.i     = i;
    next_move.j     = j;
    next_move.pass  = false;

    return;
}

/**
 * @brief       Sets the coordinates of all captured stones of this move.
 *
 * The next_move structure must know all stones that have been captured. This
 * may be needed for undoing a move, for example.
 *
 * @param[out]  captured_stones      Array of vertexes
 * @return      Nothing
 */
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

/**
 * @brief       Sets the ko field.
 *
 * If a ko happens, the field where the stone has been captured is remembered.
 * The opponent must not play on this field in the next move.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @return      Nothing
 */
void set_move_ko( int i, int j )
{
    next_move.ko[0] = i;
    next_move.ko[1] = j;

    return;
}

int get_move_last_ko_i(void)
{

    return move_history[move_number].ko[0];
}
int get_move_last_ko_j(void)
{

    return move_history[move_number].ko[1];
}

/**
 * @brief       Pushes next move onto move history.
 *
 * The current next move data structure is added to the move history list.
 *
 * @return      Nothing
 * @note        When the move number reaches MOVE_HISTORY_MAX, the program
 *              exits with a failure message.
 */
void push_move(void)
{
    // Check move number:
    if ( move_number >= MOVE_HISTORY_MAX ) {
        fprintf( stderr, "Number of possible moves has exceeded MOVE_HISTORY_MAX" );
        exit(EXIT_FAILURE);
    }

    move_history[move_number] = next_move;

    return;
}

