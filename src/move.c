#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "global_const.h"
#include "move.h"
#include "board.h"


//! The number of the latest move.
int move_number = 0;

/**
 * @brief   Structure that represents a move.
 *
 **/
struct move {
    int  number;    //!< The number of the move in the move history.
    int  color;     //!< Color of the current stone.
    bool pass;      //!< Indicates whether this move is a pass or not.
    int  ko[2];     //!< Saves the vertex of the last ko field.
    int  i;         //!< The horizontal coordinate of the vertex.
    int  j;         //!< The vertical coordinate of the vertex.
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];    //!< List of captured stones; In move zero: List of handicap stones.
} next_move;

//! Move history: contains all moves performed.
struct move move_history[MOVE_HISTORY_MAX];


/**
 * @brief       Initialises the move_history data structure.
 *
 * This function initialises the move_history data structure by setting all
 * move numbers to INVALID.
 *
 * @return      Nothing
 */
void init_move_history(void)
{
    int k, l;

    move_number = 0;

    for ( k = 0; k < MOVE_HISTORY_MAX; k++ ) {
        move_history[k].number = INVALID;
        move_history[k].color  = EMPTY;
        move_history[k].pass   = true;
        move_history[k].ko[0]  = INVALID;
        move_history[k].ko[1]  = INVALID;
        move_history[k].i      = INVALID;
        move_history[k].j      = INVALID;
        for ( l = 0; l < BOARD_SIZE_MAX * BOARD_SIZE_MAX; l++ ) {
            move_history[k].stones[l][0] = INVALID;
            move_history[k].stones[l][1] = INVALID;
        }
    }

    return;
};

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
    next_move.i      = INVALID;
    next_move.j      = INVALID;
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        next_move.stones[k][0] = INVALID;
        next_move.stones[k][1] = INVALID;
    }
    next_move.ko[0] = INVALID;
    next_move.ko[1] = INVALID;

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

    while ( captured_stones[k][0] != INVALID ) {
        next_move.stones[k][0] = captured_stones[k][0];
        next_move.stones[k][1] = captured_stones[k][1];
        k++;
    }
    next_move.stones[k][0] = INVALID;
    next_move.stones[k][1] = INVALID;

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

/**
 * @brief       Turns the current move into a PASS.
 *
 * The current move data structure is turned into a PASS move.
 *
 * @param[in]   color   Color that performs the PASS.
 * @return      Nothing
 */
void set_move_pass( int color )
{
    next_move.color = color;
    next_move.pass  = true;
    next_move.i     = INVALID;
    next_move.j     = INVALID;

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
int get_move_last_ko_color(void)
{

    return move_history[move_number].color;
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
        fprintf( stderr
            , "Number of possible moves has exceeded MOVE_HISTORY_MAX\n" );
        exit(EXIT_FAILURE);
    }

    move_history[move_number] = next_move;

    return;
}

/**
 * @brief       Checks if a given vertex is a ko.
 *
 * Checks if a given vertex for a given color is a ko.
 *
 * @param[in]   color   Color of the moving opponent
 * @param[in]   i       Horizontal coordinate
 * @param[in]   j       Vertical coordinate
 * @return      true|false
 */
bool is_move_ko( int color, int i, int j )
{
    bool is_ko = false;
    int  ko_i, ko_j;
    int  ko_color;

    ko_i     = get_move_last_ko_i();
    ko_j     = get_move_last_ko_j();
    ko_color = get_move_last_ko_color();

    if ( ko_i != INVALID && ko_j != INVALID ) {
        if ( ko_i == i && ko_j == j && ko_color == color * -1 ) {
            is_ko = true;
        }
    }

    return is_ko;
}

/**
 * @brief       Creates a list of pseudo valid moves for a given color.
 *
 * Returns a list of pseudo valid moves for a given color. Pseudo valid moves
 * include those moves which leave the setting stone without liberties. Ko
 * moves are not included in this list.
 *
 * @param[in]   color       Current color to move
 * @param[out]  valid_moves List of pseudo valid moves (Ko moves excluded)
 * @return      Number of pseudo valid moves
 * @note        This list does not contain invalid ko moves, those are dropped
 *              from this list. But moves that leave the stone or group
 *              without a liberty are still contained in this list. Therefore
 *              the term "pseudo valid".
 */
int get_pseudo_valid_move_list( int color, int valid_moves[][2] )
{
    int count;
    int i, j;
    int board_size = get_board_size();

    count = 0;
    for ( i = 0; i < board_size; i++ ) {
        for ( j = 0; j < board_size; j++ ) {
            if ( get_vertex( i, j ) == EMPTY && ! is_move_ko( color, i, j ) ) {
                valid_moves[count][0] = i;
                valid_moves[count][1] = j;
                count++;
            }
        }
    }
    valid_moves[count][0] = INVALID;
    valid_moves[count][1] = INVALID;

    return count;
}

/**
 * @brief       Returnes list of valid moves for given color.
 *
 * This function takes a list of pseudo valid moves (as created by
 * get_pseudo_valid_move_list()) and droppes the zero liberty moves. The
 * number of valid moves is returned.
 *
 * @param[in]   color   Color of moving side (BLACK|WHITE)
 * @param[in]   valid_moves_count   Number of moves in valid_moves array
 * @param[out]  valid_moves List of valid moves (zero liberty moves excluded)
 * @return      Number of valid moves
 * @sa          get_pseudo_valid_move_list()
 * @warning     The function get_pseudo_valid_move_list() must be called
 *              before get_valid_move_list().
 */
int get_valid_move_list( int color, int valid_moves_count, int valid_moves[][2] )
{
    int  count = 0;
    int  i, j;
    int  k, l;
    int  nr_of_removed_stones;
    int  captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int  group_nr;
    int  nr_of_liberties;
    bool is_valid;
    int  temp_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        temp_moves[k][0] = INVALID;
        temp_moves[k][1] = INVALID;
    }

    for ( k = 0; k < valid_moves_count; k++ ) {
        is_valid = false;
        i = valid_moves[k][0];
        j = valid_moves[k][1];

        // Make move
        set_vertex( color, i, j );
        create_groups();
        count_liberties();
        //set_groups_size();
        nr_of_removed_stones = remove_stones( color * -1 );

        if ( nr_of_removed_stones > 0 ) {
            is_valid = true;
        }
        else {
            group_nr        = get_group_nr( i, j );
            nr_of_liberties = get_nr_of_liberties(group_nr);
            if ( nr_of_liberties > 0 ) {
                is_valid = true;
            }
        }

        // Undo move:
        nr_of_removed_stones = get_captured_now(captured_now);
        set_vertex( EMPTY, i, j );
        for ( l = 0; l < nr_of_removed_stones; l++ ) {
            set_vertex( color * -1, captured_now[l][0], captured_now[l][1] );
        }
        if ( color == BLACK ) {
            set_black_captured( get_black_captured() - nr_of_removed_stones );
        }
        else {
            set_white_captured( get_white_captured() - nr_of_removed_stones );
        }

        // Save only valid moves in temporary list:
        if ( is_valid ) {
            temp_moves[count][0] = valid_moves[k][0];
            temp_moves[count][1] = valid_moves[k][1];
            count++;
        }
    }

    // Copy only valid moves into valid_moves list:
    for ( k = 0; k < count; k++ ) {
        valid_moves[k][0] = temp_moves[k][0];
        valid_moves[k][1] = temp_moves[k][1];
    }
    valid_moves[count][0] = INVALID;
    valid_moves[count][1] = INVALID;

    return count;
}

