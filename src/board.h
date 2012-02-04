#ifndef BOARD_H
#define BOARD_H

void init_board( int wanted_board_size );
void free_board(void);
void get_board_as_string( char board_output[] );
int  get_board_size(void);
void set_vertex( int color, int i, int j );
int  get_vertex( int i, int j );
void create_groups(void);
void count_liberties(void);
void remove_stones( int color );

void set_group( int i, int j );
int  has_neighbour( int i, int j, int neighbour[][2] );
int  get_free_group_nr( int color );
int  get_last_group_nr( int color );

//DEBUG:
void print_groups(void);

#endif
