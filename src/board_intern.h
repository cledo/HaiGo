#ifndef NEW_BOARD_INTERN_H
#define NEW_BOARD_INTERN_H


/**
 * @file    board_intern.h
 *
 * @brief   Internal Interface for board.c
 *
 * Contains the internal interface for new_board.c,
 * collected in a separate header file for unit testing.
 *
 */

#include <stdbool.h>
#include "global_const.h"

//! Defines data type of worm number.
typedef unsigned short worm_nr_t;

//! Data structure representing a worm.
typedef struct worm_st {
    worm_nr_t number;           //!< Worm number
    unsigned short count;       //!< Number of stones (or fields)
    unsigned short liberties;   //!< Number of liberties
    int index[BOARD_SIZE_MAX * BOARD_SIZE_MAX];  //!< List of indexes of worm
} worm_t;

bool is_board_null(void);
bool is_on_board( int i, int j );
bool is_hoshi( int i, int j );
void init_hoshi(void);
void set_hoshi( int i, int j );
int  get_vertex_intern( int index_1d );
void create_worm_data( int index_1d, int color );
void build_worms( int index_1d );
void count_worm_liberties( int index_1d );
int  get_worm_neighbours( int index_1d, worm_nr_t worm_nr_current, int color_i );

void get_label_x( int i, char x[] );
void get_label_y_left( int j, char y[] );
void get_label_y_right( int j, char y[] );

worm_t get_worm( int color, worm_nr_t worm_nr );

int get_free_worm_nr( int color );
int has_neighbour( int i, int j, int neighbour[][2] );


// TEST:
void print_removed(void);

#endif

