#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "global_const.h"
#include "move.h"
#include "board.h"

/**
 * @file    move.c
 *
 * @brief   Defines move structure, move history
 *
 * Defines all data structures, such as move and move_history, and its
 * corresponding functions.
 *
 */

//! The number of the latest move.
static int move_number = 0;

//! Move history: contains all moves performed.
static struct move_st move_history[MOVE_HISTORY_MAX];

static int compare_by_value( const void *move1, const void *move2 );


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
        move_history[k].count_stones = INVALID;
        move_history[k].value        = 0;
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

    next_move.number = move_number + 1;
    next_move.color  = EMPTY;
    next_move.pass   = true;
    next_move.i      = INVALID;
    next_move.j      = INVALID;
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        next_move.stones[k][0] = INVALID;
        next_move.stones[k][1] = INVALID;
    }
    next_move.ko[0]        = INVALID;
    next_move.ko[1]        = INVALID;
    next_move.count_stones = 0;
    next_move.value        = 0;

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
 * @param[in]   captured_stones      Array of vertexes
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

    next_move.count_stones = k;

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

/**
 * @brief       Returns i of last move's ko.
 *
 * Returns the horizontal coordinate of the last move's ko field.
 *
 * @return      Horizontal coordinate of ko field
 * @sa          get_last_move_j()
 */
int get_move_last_ko_i(void)
{

    return move_history[move_number].ko[0];
}

/**
 * @brief       Returns j of last move's ko.
 *
 * Returns the vertical coordinate of the last move's ko field.
 *
 * @return      Horizontal coordinate of ko field
 * @sa          get_last_move_i()
 */
int get_move_last_ko_j(void)
{

    return move_history[move_number].ko[1];
}

/**
 * @brief       Returns the number of the last move in the move history.
 *
 * Returns the number of the last move in the move history. As the first move
 * data in the move_history starts with an invalid move, the returned number
 * should be INVALID if no move has been made yet.
 *
 * @return      Move number or INVALID
 */
int get_last_move_number(void)
{

    return move_history[move_number].number;
}

/**
 * @brief       Returns color of last move.
 *
 * Returns the color of the last move in the move history.
 *
 * @return      BLACK|WHITE
 * @note        Remember that the Go Text Protocol does not say that colors
 *              must change every move!
 */
int get_move_last_color(void)
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
 * @sa          pop_move()
 */
void push_move(void)
{
    move_number++;

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
 * @brief       Pops last move from move history.
 *
 * The last move of the move history is deleted. It is not really a pop,
 * because nothing is returned here.
 *
 * @return      Nothing
 * @sa          push_move()
 */
void pop_move(void)
{
    int k;

    move_history[move_number].number = INVALID;
    move_history[move_number].color  = EMPTY;
    move_history[move_number].pass   = true;
    move_history[move_number].ko[0]  = INVALID;
    move_history[move_number].ko[1]  = INVALID;
    move_history[move_number].i      = INVALID;
    move_history[move_number].j      = INVALID;
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        move_history[move_number].stones[k][0] = INVALID;
        move_history[move_number].stones[k][1] = INVALID;
    }
    move_history[move_number].count_stones = INVALID;

    move_number--;

    return;
}

/**
 * @brief       Returns last move number.
 *
 * Returns the move number of the last move.
 *
 * @return      Move number
 */
int get_move_number(void)
{
    return move_number;
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
    ko_color = get_move_last_color();

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
int get_pseudo_valid_move_list( int color, int valid_moves[][3] )
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
 * @param[in]   color               Color of moving side (BLACK|WHITE)
 * @param[in]   valid_moves_count   Number of moves in valid_moves array
 * @param[out]  valid_moves         List of valid moves (zero liberty moves excluded)
 * @return      Number of valid moves
 * @sa          get_pseudo_valid_move_list()
 * @warning     The function get_pseudo_valid_move_list() must be called
 *              before get_valid_move_list().
 */
int get_valid_move_list( int color, int valid_moves_count, int valid_moves[][3] )
{
    int  count = 0;
    int  i, j;
    int  k, l;
    int  nr_of_removed_stones;
    int  captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int  group_nr;
    int  nr_of_liberties;
    bool is_valid;
    int  temp_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][3];
    int  value;

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

        // TEST:
        value = get_black_captured() - get_white_captured();


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
            temp_moves[count][0] = i;
            temp_moves[count][1] = j;
            temp_moves[count][2] = value;
            count++;
        }
    }

    // Copy only valid moves into valid_moves list:
    for ( k = 0; k < count; k++ ) {
        valid_moves[k][0] = temp_moves[k][0];
        valid_moves[k][1] = temp_moves[k][1];
        valid_moves[k][2] = temp_moves[k][2];
    }
    valid_moves[count][0] = INVALID;
    valid_moves[count][1] = INVALID;

    // Sort valid moves list by value
    qsort( valid_moves, count, sizeof(valid_moves[0]), compare_by_value );

    return count;
}

/**
 * @brief       Return color of last move.
 *
 * Returns the color of the stone of the last move.
 *
 * @return      BLACK|WHITE
 */
int get_last_move_color(void)
{
    int color = move_history[move_number].color;


    return color;
}

/**
 * @brief       Returns i of last move.
 *
 * Returns the horizontal coordinate of the last move.
 *
 * @return      Horizontal coordinate
 * @sa          get_last_move_j()
 */
int get_last_move_i(void)
{
    int i = move_history[move_number].i;

    return i;
}

/**
 * @brief       Returns j of last move.
 *
 * Returns the vertical coordinate of the last move.
 *
 * @return      Vertical coordinate
 * @sa          get_last_move_i()
 */
int get_last_move_j(void)
{
    int j = move_history[move_number].j;

    return j;
}

/**
 * @brief       Shows if last move was a PASS.
 *
 * Returns true if the last move in the move history has been a PASS, false
 * otherwise.
 *
 * @return      true|false
 */
bool get_last_move_pass(void)
{
    bool pass = move_history[move_number].pass;

    return pass;
}

/**
 * @brief       Returns number of captured stones of last move.
 *
 * Returns the number of stones which have been captured by the last move.
 *
 * @return      Number of captured stones.
 */
int get_last_move_count_stones(void)
{
    int count_stones = move_history[move_number].count_stones;

    return count_stones;
}

/**
 * @brief       Returns verteces of captured stones.
 *
 * Returns a list of verteces which shows where the last move has captured
 * stones.
 *
 * @param[out]  stones  List of verteces
 * @return      Nothing
 */
void get_last_move_stones( int stones[][2] )
{
    int k;
    int count_stones = move_history[move_number].count_stones;

    for ( k = 0; k < count_stones; k++ ) {
        stones[k][0] = move_history[move_number].stones[k][0];
        stones[k][1] = move_history[move_number].stones[k][1];
    }

    return;
}

/**
 * @brief       Returns value of move.
 *
 * Returns the value of the last move in the move_history.
 *
 * @return      value   Value of move
 */
int get_last_move_value(void)
{

    return move_history[move_number].value;
}

/**
 * @brief       Helper function for qsort().
 *
 * This is a helper function for qsort(), that makes it possible to sort the
 * move list by move value.
 *
 * @param[in]   move1   Pointer to first move
 * @param[in]   move2   Pointer to second move
 * @return      1|0|-1
 * @sa          man 3 qsort
 */
int compare_by_value( const void *move1, const void *move2 )
{
    if ( ((int *)move1)[2] > ( (int *)move2)[2] ) {
        return -1;
    }
    else if ( ( (int *)move1)[2] < ( (int *)move2)[2] ) {
        return 1;
    }

    return 0;
}

