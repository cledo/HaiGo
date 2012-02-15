#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include "sgf.h"


/**
 * @brief       Parses an SGF string
 *
 * Parses a complete SGF string as it is read from an SGF file.
 *
 * @param[in]   file_content    SGF string as read from file
 * @return      Nothing
 * @todo        Should this function write into the move_history or build an
 *              SGF tree?
 */
void parse_sgf( char *file_content )
{
    int  k = 0;
    char current_char;
    char last_char;

    while ( ( current_char = file_content[k++] ) != '\0' ) {
        last_char = current_char;
    }

    return;
}

