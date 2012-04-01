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
void print_worm_boards(void);
void print_worm_lists(void);

int get_white_captured(void);
int get_black_captured(void);
void set_white_captured( int captured );
void set_black_captured( int captured );

int remove_stones( int color );

bool is_hoshi( int i, int j );

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
void print_groups(void);
int  get_stone_count( int color );
void get_bouzy_as_string( char bouzy_str[] );
int get_captured_now( int captured[][2] );
void do_influence(void);
int get_count_kosumis( int color );
void count_kosumi(void);
int get_count_influence( int color );

/* Hashing */
void init_hash_table(void);
void init_hash_id(void);

#endif

