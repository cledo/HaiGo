#include <stdio.h>
#include <stdlib.h>
#include "new_board.h"
#include "../src/global_const.h"


/**
 * @file    new_main.c
 *
 * Placeholder for main.
 *
 * A simple temporary placeholder
 * for main.c until the new_board stuff may be integrated into the main
 * program.
 *
 */


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
    bsize_t board_size = 19;

    char board_output[MAX_OUTPUT_LENGTH];

    init_board(board_size);

    set_vertex( BLACK, 0, 0 );
    set_vertex( WHITE, 18, 18 );


    get_board_as_string(board_output);
    printf( "%s\n", board_output );

    free_board();

    board_size = 13;
    board_output[0] = '\0';

    init_board(board_size);
    set_vertex( BLACK, 0, 0 );
    set_vertex( BLACK, 0, 1 );
    set_vertex( BLACK, 1, 1 );
    set_vertex( WHITE, 12, 12 );
    set_vertex( WHITE, 12, 11 );
    get_board_as_string(board_output);
    printf( "%s\n", board_output );

    scan_board();

    free_board();

    return EXIT_SUCCESS;
}

