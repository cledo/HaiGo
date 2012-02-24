#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

/**
 * @file    board.h
 *
 * @brief   Interface definition for board.c
 *
 */

/**
 * @name    Board data structures
 *
 * Functions to allocate and free all necessary board data structures.
 *
 */

//@{
void init_board( int wanted_board_size );
void free_board(void);
//@}

/**
 * @name Group related functions
 *
 * Functions that create and retrieve data according to groups.
 *
 */

//@{
void create_groups(void);
void count_liberties(void);
int  has_neighbour( int i, int j, int neighbour[][2] );
int  get_free_group_nr( int color );
int  get_last_group_nr( int color );
int  get_group_nr( int i, int j );
int  get_nr_of_liberties( int group_nr );
void set_groups_size(void);
int  get_size_of_group( int group_nr );
//@}

/**
 * @name Capturing related functions
 *
 * Functions about captured stones and capturing stones.
 *
 */

//@{
int  remove_stones( int color );
int  get_captured_now( int captured[][2] );
int  get_black_captured(void);
int  get_white_captured(void);
void set_black_captured( int captured );
void set_white_captured( int captured );
//@}


/**
 * @name Test functions
 *
 * Functions that are (mainly) needed for testing.
 *
 */

//@{
bool is_board_null(void);
bool is_group_board_null(void);
bool is_hoshi_board_null(void);
void print_groups(void);
//@}

/**
 * @name Other functions
 *
 */

//@{
void get_board_as_string( char board_output[] );
int  get_board_size(void);
void set_vertex( int color, int i, int j );
int  get_vertex( int i, int j );
bool is_hoshi( int i, int j );
int  get_stone_count( int color );
//@}

#endif

