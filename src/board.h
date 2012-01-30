#ifndef BOARD_H
#define BOARD_H

void init_board( int wanted_board_size );
void free_board(void);
void get_board_as_string( char board_output[] );
int  get_board_size(void);
void set_vertex( int color, int i, int j );

#endif
