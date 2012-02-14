#ifndef MOVE_H
#define MOVE_H

void init_move_history(void);
void create_next_move(void);
void set_move_vertex( int color, int i, int j );
void set_move_captured_stones( int captured_stones[][2] );
void push_move(void);
void set_move_ko( int i, int j );
void set_move_pass( int color );
int  get_move_last_ko_i(void);
int  get_move_last_ko_j(void);
int  get_move_last_ko_color(void);

bool is_move_ko( int color, int i, int j );
int  get_pseudo_valid_move_list( int color, int valid_moves[][2] );
int  get_valid_move_list( int color, int valid_moves_count, int valid_moves[][2] );

#endif
