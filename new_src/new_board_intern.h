#ifndef NEW_BOARD_INTERN_H
#define NEW_BOARD_INTERN_H

#include <stdbool.h>

void get_label_x( int i, char x[] );
void get_label_y_left( int j, char y[] );
void get_label_y_right( int j, char y[] );

int get_white_captured(void);
int get_black_captured(void);
bool is_hoshi( int i, int j );


#endif

