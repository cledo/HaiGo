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

//! Data structure representing a worm.
typedef struct worm_st {
    unsigned short number;  //!< Worm number
    unsigned short count;   //!< Number of stones (or fields)
} worm_t;

bool is_board_null(void);
bool is_on_board( int i, int j );
void init_hoshi(void);
void set_hoshi( int i, int j );
bool is_hoshi( int i, int j );
int  get_vertex_intern( int index_1d );
void create_worm_data( int index_1d, int color );
void build_worms( int index_1d );
//int  get_vertex_intern( row_t I, int J );
//void create_worm( row_t I, int J, int index_1d, int color );

void get_label_x( int i, char x[] );
void get_label_y_left( int j, char y[] );
void get_label_y_right( int j, char y[] );

int get_white_captured(void);
int get_black_captured(void);
worm_t get_worm( int color, unsigned short worm_nr );


#endif

