#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include <ctype.h>
#include "global_const.h"
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
    int  game_tree_level = INVALID;
    int  node_nr         = INVALID;

    while ( ( current_char = file_content[k++] ) != '\0' ) {
        if ( current_char == '(' ) {
            game_tree_level++;
            printf( "# New GameTree %d\n", game_tree_level );
        }
        if ( current_char == ')' ) {
            printf( "# End GameTree %d\n", game_tree_level );
            game_tree_level--;
        }
        if ( current_char == ';' ) {
            node_nr++;
            printf( "#### New Node %d\n", node_nr );
        }
        if ( isupper(current_char) && ( isspace(last_char) || last_char == ']' || last_char == ';' ) ) {
            printf( "    ## New Property\n" );
        }
        if ( current_char == '[' ) {
            printf( "      # New PropValue\n" );
        }

        last_char = current_char;
    }

    return;
}

