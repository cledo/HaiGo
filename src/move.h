#ifndef MOVE_H
#define MOVE_H

/**
 * @file    move.h
 *
 * @brief   Interface definition for move.h
 *
 */

#include <stdbool.h>
#include "global_const.h"

/**
 * @brief   Structure that represents a move.
 *
 **/
struct move_st {
    int  number;    //!< The number of the move in the move history.
    int  color;     //!< Color of the current stone.
    bool pass;      //!< Indicates whether this move is a pass or not.
    int  ko[2];     //!< Saves the vertex of the last ko field.
    int  i;         //!< The horizontal coordinate of the vertex.
    int  j;         //!< The vertical coordinate of the vertex.
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];    //!< List of captured stones; In move zero: List of handicap stones.
    int  count_stones;  //!< Number of captured stones
} next_move;


void init_move_history(void);
void create_next_move(void);
void set_move_vertex( int color, int i, int j );
void set_move_captured_stones( int captured_stones[][2] );
void push_move(void);
void pop_move(void);
void set_move_ko( int i, int j );
void set_move_pass( int color );
int  get_move_last_ko_i(void);
int  get_move_last_ko_j(void);
int  get_move_last_color(void);

bool is_move_ko( int color, int i, int j );
int  get_pseudo_valid_move_list( int color, int valid_moves[][2] );
int  get_valid_move_list( int color, int valid_moves_count, int valid_moves[][2] );

int  get_move_number(void);
int  get_last_move_number(void);
int  get_last_move_color(void);
int  get_last_move_i(void);
int  get_last_move_j(void);
bool get_last_move_pass(void);
int  get_last_move_count_stones(void);
void get_last_move_stones( int stones[][2] );

#endif
