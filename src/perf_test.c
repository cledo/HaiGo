#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "global_const.h"
#include "board.h"
#include "board_intern.h"


/**
 * @file    perf_test.c
 *
 * A simple main function for testing.
 *
 * A simple main function where performance testing for selected functions can
 * be done.
 *
 */

void perf_scan_1(void);
void perf_scan_1_upd(void);

/**
 * @brief       Dummy main function.
 *
 * A placeholder for the main functio,
 *
 * @return      Return exit code
 * @note        May be used for simple testing.
 */
int main(void)
{

    //perf_scan_1();

    return EXIT_SUCCESS;
}


/**
 * @brief       Performance test for scan_board_1
 *
 * Performance test for scan_board_1.
 *
 * @return      Nothing
 */
inline void perf_scan_1(void)
{
    unsigned long k, l;
    unsigned long max_k = 1000000;    // scan_board_1
    //unsigned long max_k = 10000000;  // scan_board_2
    unsigned long max_l = 100;

    //unsigned long max_k = 1000000;
    //unsigned long max_l = 10;


    // For valgrind test:
    //unsigned long max_k = 10000;
    //unsigned long max_l = 1;

    int i, j;
    int stone_count;
    int color = BLACK;
    bsize_t board_size = 19;
    //bsize_t board_size = 6;
    int count_removed;

    char board_output[MAX_OUTPUT_LENGTH];

    // Variables for measuring time:
    time_t start;
    time_t stop;
    time_t diff_time;
    time_t sum_time = 0;


    srand( time(NULL) );

    for ( l = 1; l <= max_l; l++ ) {
        init_board(board_size);

        stone_count = rand() % ( board_size * board_size );
        for ( k = 1; k <= stone_count; k++ ) {
            i = rand() % board_size;
            j = rand() % board_size;
            if ( get_vertex( i, j ) != EMPTY ) {
                k--;
                continue;
            }
            set_vertex( color, i, j );
            color *= -1;
        }

        get_board_as_string(board_output);
        printf( "\n%s\n", board_output );

        (void) time(&start);
        for ( k = 1; k <= max_k; k++ ) {
            scan_board_1();

            /*
            i = rand() % board_size;
            j = rand() % board_size;
            set_vertex( i, j, color * -1 );
            scan_board_1_upd( i, j );
            */
            //scan_board_2();
        }
        //count_removed = remove_stones(WHITE);
        //print_removed();
        //scan_board_1();

        (void) time(&stop);
        diff_time = stop - start;
        sum_time += diff_time;

        printf( "BoardNr.: %lu\t%lu\t(%d)\n", l, diff_time, count_removed );

        print_worm_boards();
        print_worm_lists();

        free_board();
    }

    printf( "\nTime:   %lu\n", sum_time );
    printf( "Scan/s: %lu\n", (l-1) * (k-1) / sum_time );


    return;
}

/**
 * @brief       Performance test for scan_board_1_upd
 *
 * Performance test for scan_board_1_upd.
 *
 * @return      Nothing
 */
void perf_scan_1_upd(void)
{



    return;
}

