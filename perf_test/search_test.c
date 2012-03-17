#include <stdlib.h>
#include <stdio.h>
#include "../src/global_const.h"
#include "../src/board.h"
#include "../src/move.h"
#include "../src/evaluate.h"
#include "../src/search.h"


int main( int argc, char *argv[] )
{
    int board_size;
    int i, j;
    int color;
    int level;
    int b;
    search_stats_t s;

    if ( argc >= 3 ) {
        board_size = atoi(argv[1]);
        level      = atoi(argv[2]);
    }
    else {
        board_size = 3;
        level      = 0;
    }

    color = BLACK;

    init_board(board_size);
    init_move_history();
    init_brains();
    init_search_stats();
    set_search_depth(level);

    if ( argc >= 4 && argv[3][0] == '1' ) {
        for ( b = 0; b < COUNT_BRAINS; b++ ) {
            set_factor( b, 0 );
        }
    }

    //set_do_log();

    search_tree( color, &i, &j );

    s = get_search_stats();

    printf( "Board:   %d\n", board_size            );
    printf( "Color:   %s\n", s.color               );
    printf( "Move:    %s\n", s.move                );
    printf( "Level:   %d\n", s.level               );
    printf( "Time:    %d\n", (int) s.duration      );
    printf( "Nodes:   %llu\n", s.node_count        );
    printf( "Nodes/s: %d\n", (int) s.nodes_per_sec );
    printf( "QSearch: %d\n", s.qsearch_count       );
    printf( "A-Cut:   %d\n", s.alpha_cut           );
    printf( "B-Cut:   %d\n", s.beta_cut            );
    printf( "Value:   %d\n", s.value               );

    free_board();

    return EXIT_SUCCESS;
}


