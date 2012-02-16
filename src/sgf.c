#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include <ctype.h>
#include <stdbool.h>
#include "global_const.h"
#include "sgf.h"

struct node {
    int  number;
    bool is_main;
    int  tree_nr;
    int  tree_level;
    struct node *parent;
};

void add_node( struct node *sgf_tree_start, int node_nr, int game_tree_nr, int game_tree_level, bool is_main_line );

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
    int  game_tree_nr    = INVALID;
    int  game_tree_level = INVALID;
    int  node_nr         = INVALID;
    bool in_PropValue    = false;
    char PropValue[128];
    bool in_PropIdent    = false;
    char PropIdent[128];

    int    node_count = 0;
    struct node *sgf_tree;
    struct node *sgf_tree_start;
    bool   is_main_line = true;

    // Count number of ';' characters to get maximum number of possible nodes:
    while ( ( current_char = file_content[k++] ) != '\0' ) {
        if ( current_char == ';' ) {
            node_count++;
        }
    }
    k = 0;

    sgf_tree       = malloc( sizeof( struct node ) * ( node_count ) );
    sgf_tree_start = sgf_tree;

    while ( ( current_char = file_content[k++] ) != '\0' ) {
        if ( current_char == '(' ) {
            game_tree_nr++;
            game_tree_level++;
            //printf( "# New GameTree %d\n", game_tree_level );
        }
        if ( current_char == ')' ) {
            //printf( "# End GameTree %d\n", game_tree_level );
            game_tree_level--;
            is_main_line = false;
        }
        if ( current_char == ';' ) {
            node_nr++;
            if ( node_nr > 0 ) {
                sgf_tree++;
            }
            //printf( "#### New Node %d\n", node_nr );
            add_node( sgf_tree_start, node_nr, game_tree_nr, game_tree_level, is_main_line );
        }
        if ( isupper(current_char) && ( isspace(last_char) || (last_char == ']') || (last_char == ';') ) ) {
            in_PropIdent = true;
            //printf( "    ## New Property - " );
        }
        if ( in_PropIdent && ! isupper(current_char) ) {
            in_PropIdent = false;
            PropIdent[l] = '\0';
            l = 0;
            //printf( "%s\n", PropIdent );
        }
        if ( current_char == '[' ) {
            //printf( "      # New PropValue start - " );
            in_PropValue = true;

            last_char = current_char;
            continue;
        }
        if ( current_char == ']' && last_char != '\\' ) {
            in_PropValue = false;
            PropValue[l] = '\0';
            l = 0;
            //printf( "%s\n", PropValue );

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

    // DEBUG:
    /*
    for ( k = 0; k <= node_nr; k++ ) {
        printf( "# Node Nr.: %d\n", (sgf_tree_start + k)->number );
        printf( "#        Main: %d\n", (sgf_tree_start + k)->is_main );
        printf( "#        Level: %d\n", (sgf_tree_start + k)->tree_level );
        printf( "#        Tree : %d\n", (sgf_tree_start + k)->tree_nr );
        if ( (sgf_tree_start + k)->parent != NULL ) {
            printf( "#        Par  : %d\n", (sgf_tree_start + k)->parent->number );
        }
        else {
            printf( "#        Par  : ??\n" );
        }
    }
    */

    sgf_tree = sgf_tree_start;
    free(sgf_tree);
    sgf_tree       = NULL;
    sgf_tree_start = NULL;

    return;
}

/**
 * @brief       Adds new node to SGF tree
 *
 * Adds a new node to the SGF tree. Sets the node's number, tree level, tree
 * number, and parent node.
 *
 * @param[in]   *sgf_tree_start     Starting pointer to SGF tree
 * @param[in]   node_nr             Number of node to add
 * @param[in]   game_tree_nr        Number of game tree this node belongs to
 * @param[in]   game_tree_level     Level of game tree this node belongs to
 * @param[in]   is_main_line        Flag that shows if this move is part of the the main line
 * @return      Nothing
 * @note        game_tree_nr and game_tree_level are important to determine
 *              the node's parent.
 */
void add_node( struct node *sgf_tree_start, int node_nr, int game_tree_nr, int game_tree_level, bool is_main_line )
{
    int m;
    struct node *new_node;

    new_node = sgf_tree_start + node_nr;

    new_node->number     = node_nr;
    new_node->tree_nr    = game_tree_nr;
    new_node->tree_level = game_tree_level;
    new_node->is_main    = is_main_line;

    // Set parent node:
    if ( node_nr == 0 ) {
        new_node->parent = new_node;
    }
    else if ( game_tree_level == (new_node - 1)->tree_level && game_tree_nr == (new_node - 1)->tree_nr ) {
        new_node->parent = (new_node - 1);
    }
    else if ( game_tree_level - 1 == (new_node - 1)->tree_level && game_tree_nr - 1 == (new_node - 1)->tree_nr ) {
        new_node->parent = (new_node - 1);
    }
    else {
        for ( m = node_nr - 1; m >= 0; m-- ) {
            if ( (sgf_tree_start + m)->tree_level == game_tree_level - 1 ) {
                new_node->parent = (sgf_tree_start + m);
                break;
            }
        }
    }

    return;
}

