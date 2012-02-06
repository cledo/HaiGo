#ifndef MOVE_H
#define MOVE_H

void init_move_history(void);
int  get_next_move_nr(void);
void create_next_move(void);
void set_move_vertex( int color, int i, int j );
void set_move_captured_stones( int captured_stones[][2] );
void push_move(void);

#endif
