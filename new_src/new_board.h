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

void scan_board(void);
void print_worm_boards(void);
void print_worm_lists(void);

/* Old interface, delete later! */
void create_groups(void);
void create_group_chains(void);
void count_liberties(void);
int get_last_group_nr( int color );
int get_group_nr( int i, int j );
int get_nr_of_liberties( int group_nr );
void set_groups_size(void);
int get_size_of_group( int group_nr );
int get_size_of_empty_group( int group_nr );
int get_group_count_atari( int color );
int get_group_count_liberties( int group_nr );
int get_last_chain_nr( int color );
int get_nr_groups_no_chain( int color );
int get_one_eye_groups( int color );

#endif

