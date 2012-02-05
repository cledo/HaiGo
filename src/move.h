#ifndef MOVE_H
#define MOVE_H

int  create_next_move(void);
void set_move_vertex( int i, int j );
void set_move_captured_stones( int captured_stones[][2] );
void print_move(void);

#endif
