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
 * @name Vertex related functions
 *
 */
//@{
void set_vertex( int color, int i, int j );
int  get_vertex( int i, int j );
bool is_hoshi( int i, int j );
//@}

/**
 * @name Group related functions
 *
 * Functions that create and retrieve data according to groups.
 *
 */

//@{
void create_groups(void);
void create_group_chains(void);
void count_liberties(void);
int  has_neighbour( int i, int j, int neighbour[][2] );
int  get_free_group_nr( int color );
int  get_last_group_nr( int color );
int  get_group_nr( int i, int j );
int  get_nr_of_liberties( int group_nr );
void set_groups_size(void);
int  get_size_of_group( int group_nr );
int  get_group_count_atari( int color );
int  get_group_count_liberties( int color );
int  get_last_chain_nr( int color );
int  get_nr_groups_no_chain( int color );
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
 * @name Hash table functions.
 *
 * Functions that deal with the transposition hash table.
 *
 */

//@{
void     init_hash_id(void);
unsigned get_hash_id(void);
void     set_hash_id( unsigned id );
void     init_hash_table(void);
void     insert_hash_table( unsigned id, int value );
int      select_hash_table_value( unsigned id );
bool     exists_hash_id( unsigned id );
//@}

/**
 * @name Other functions
 *
 */

//@{
void get_board_as_string( char board_output[] );
int  get_board_size(void);
int  get_stone_count( int color );
//@}

#endif

