#ifndef NEW_BOARD_H
#define NEW_BOARD_H


/**
 * @file    board.h
 *
 * @brief   External interface for board.c
 *
 * Contains the external interface
 * for board.c.
 *
 */

#include <stdbool.h>

typedef unsigned short bsize_t;     //!< Type of board size value.

void init_board( bsize_t board_size );
void free_board(void);

void    set_board_size( bsize_t size );
bsize_t get_board_size(void);

void get_board_as_string( char board_output[] );

void set_vertex( int color, int i, int j );
int  get_vertex( int i, int j );

void scan_board_1(void);
void scan_board_1_upd( int i, int j );
void scan_board_2(void);

void print_worm_boards(void);
void print_worm_lists(void);

int get_white_captured(void);
int get_black_captured(void);
void set_white_captured( int captured );
void set_black_captured( int captured );

int remove_stones( int color );

int get_worm_liberty_count( int i, int j );

int get_size_of_worm( int group_nr );
int get_worm_nr( int i, int j );
int get_nr_of_liberties( int worm_nr );
int get_captured_now( int captured[][2] );
int get_stone_count( int color );
int get_worm_count_atari( int color );

#endif

