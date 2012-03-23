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

bool is_board_null(void);
bool is_on_board( int i, int j );
void init_hoshi(void);
void set_hoshi( int i, int j );
bool is_hoshi( int i, int j );

void get_label_x( int i, char x[] );
void get_label_y_left( int j, char y[] );
void get_label_y_right( int j, char y[] );

int get_white_captured(void);
int get_black_captured(void);


#endif

