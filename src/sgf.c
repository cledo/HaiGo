#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include <ctype.h>
#include <stdbool.h>
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
    int  l = 0;
    char current_char;
    char last_char = '\0';
    int  game_tree_level = INVALID;
    int  node_nr         = INVALID;
    bool in_PropValue    = false;
    char PropValue[128];
    bool in_PropIdent    = false;
    char PropIdent[128];

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
        if ( isupper(current_char) && ( isspace(last_char) || (last_char == ']') || (last_char == ';') ) ) {
            in_PropIdent = true;
            printf( "    ## New Property - " );
        }
        if ( in_PropIdent && ! isupper(current_char) ) {
            in_PropIdent = false;
            PropIdent[l] = '\0';
            l = 0;
            printf( "%s\n", PropIdent );
        }
        if ( current_char == '[' ) {
            printf( "      # New PropValue start - " );
            in_PropValue = true;

            last_char = current_char;
            continue;
        }
        if ( current_char == ']' ) {
            in_PropValue = false;
            PropValue[l] = '\0';
            l = 0;
            printf( "%s\n", PropValue );

            last_char = current_char;
            continue;
        }


        if ( in_PropValue ) {
            PropValue[l++] = current_char;
        }
        if ( in_PropIdent ) {
            PropIdent[l++] = current_char;
        }

        last_char = current_char;
    }

    return;
}

