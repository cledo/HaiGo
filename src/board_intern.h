#ifndef NEW_BOARD_INTERN_H
#define NEW_BOARD_INTERN_H


/**
 * @file    new_board_intern.h
 *
 * @brief   Internal Interface for new_board.c
 *
 * Contains the internal interface for new_board.c,
 * collected in a separate header file for unit testing.
 *
 */

#include <stdbool.h>

//typedef unsigned long row_t;    //!< Defines a row of the board.

typedef unsigned short worm_nr_t;

//! Data structure representing a worm.
typedef struct worm_st {
    worm_nr_t number;           //!< Worm number
    unsigned short count;       //!< Number of stones (or fields)
    unsigned short liberties;   //!< Number of liberties
} worm_t;

bool is_board_null(void);
bool is_on_board( int i, int j );
void init_hoshi(void);
void set_hoshi( int i, int j );
int  get_vertex_intern( int index_1d );
void scan_board_1(void);
void scan_board_2(void);
void create_worm_data( int index_1d, int color );
void build_worms( int index_1d );
//int  get_vertex_intern( row_t I, int J );
//void create_worm( row_t I, int J, int index_1d, int color );
void count_worm_liberties( int index_1d );
int  get_worm_neighbours( int index_1d, worm_nr_t worm_nr_current, int color_i );

void get_label_x( int i, char x[] );
void get_label_y_left( int j, char y[] );
void get_label_y_right( int j, char y[] );

worm_t get_worm( int color, worm_nr_t worm_nr );

bool is_group_board_null(void);
bool is_hoshi_board_null(void);

int get_free_group_nr( int color );
int has_neighbour( int i, int j, int neighbour[][2] );
void set_groups_size(void);

#endif
