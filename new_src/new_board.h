#ifndef NEW_BOARD_H
#define NEW_BOARD_H


/**
 * @file    new_board.h
 *
 * @brief   External interface for new_board.c
 *
 * Contains the external interface
 * for new_board.c.
 *
 */

typedef unsigned short bsize_t;     //!< Type of board size value.

void init_board( bsize_t board_size );
void free_board(void);

void    set_board_size( bsize_t size );
bsize_t get_board_size(void);

void get_board_as_string( char board_output[] );

void set_vertex( int color, int i, int j );
int  get_vertex( int i, int j );

void create_worms(void);

#endif

