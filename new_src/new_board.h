#ifndef NEW_BOARD_H
#define NEW_BOARD_H

typedef unsigned short bsize_t;

void init_board( bsize_t board_size );
void free_board(void);

void    set_board_size( bsize_t size );
bsize_t get_board_size(void);

#endif

