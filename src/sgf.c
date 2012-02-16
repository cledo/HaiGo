#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "global_const.h"
#include "sgf.h"
#include "global_tools.h"

struct node {
    int  number;
    bool is_main;
    int  tree_nr;
    int  tree_level;
    struct node        *parent;
    struct property_st *property;
    int  property_count;
};

struct property_st {
    int  number;
    char *name;
    char **value;
    int  value_count;
};

void add_node( struct node *sgf_tree_start, int node_nr, int game_tree_nr, int game_tree_level, bool is_main_line );
void add_property( struct node *sgf_tree, char property_name[] );

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
    int  m = 0;
    int  content_size = 0;
    char current_char;
    char last_char = '\0';
    int  game_tree_nr    = INVALID;
    int  game_tree_level = INVALID;
    int  node_nr         = INVALID;
    char *property_name;
    char *property_value;
    bool in_property_name  = false;
    bool in_property_value = false;
    int  property_count = 0;
    int  value_count    = 0;

    int    node_count = 0;
    struct node *sgf_tree;
    struct node *sgf_tree_start;
    bool   is_main_line = true;


    // Count number of ';' characters to get maximum number of possible nodes.
    // Count number of characters also.
    while ( ( current_char = file_content[k++] ) != '\0' ) {
        if ( current_char == ';' ) {
            node_count++;
        }
        content_size++;
    }
    k = 0;

    property_name  = malloc( sizeof(char) * content_size );
    property_value = malloc( sizeof(char) * content_size );

    sgf_tree = malloc( sizeof( struct node ) * ( node_count ) );

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
            property_count = 0;
        }
        if ( isupper(current_char) && ( isspace(last_char) || (last_char == ']') || (last_char == ';') ) ) {
            in_property_name = true;
            //printf( "    ## New Property - " );
            value_count = 0;
        }
        if ( in_property_name && ! isupper(current_char) ) {
            in_property_name = false;
            property_name[l] = '\0';
            l = 0;
            //printf( "%s\n", property_name );
            sgf_tree->property_count = ++property_count;
            add_property( sgf_tree, property_name );
        }
        if ( current_char == '[' ) {
            //printf( "      # New property_value start - " );
            in_property_value = true;

            last_char = current_char;
            continue;
        }
        if ( current_char == ']' && last_char != '\\' ) {
            in_property_value = false;
            property_value[l] = '\0';
            l = 0;
            //printf( "%s\n", property_value );
            value_count++;
            //printf( "    ValNr.: %d\n", value_count );
            (sgf_tree->property + property_count - 1)->value_count = value_count;
            if ( value_count == 1 ) {
                (sgf_tree->property + property_count - 1)->value = malloc( sizeof(char **) );
            }
            else if ( value_count > 1 ) {
                (sgf_tree->property + property_count - 1)->value = realloc( (sgf_tree->property + property_count - 1)->value, sizeof(char **) * value_count );
            }
            *((sgf_tree->property + property_count - 1)->value + value_count - 1) = malloc( strlen(property_value) );
            strcpy( *((sgf_tree->property + property_count - 1)->value + value_count - 1), property_value );

            last_char = current_char;
            continue;
        }


        if ( in_property_value ) {
            property_value[l++] = current_char;
        }
        if ( in_property_name ) {
            property_name[l++] = current_char;
        }

        last_char = current_char;
    }

    // DEBUG:
    // /*
    for ( k = 0; k <= node_nr; k++ ) {
        printf( "# Node Nr.: %d\n", (sgf_tree_start + k)->number );
        printf( "#        Main : %d\n", (sgf_tree_start + k)->is_main );
        printf( "#        Level: %d\n", (sgf_tree_start + k)->tree_level );
        printf( "#        Tree : %d\n", (sgf_tree_start + k)->tree_nr );
        if ( (sgf_tree_start + k)->parent != NULL ) {
            printf( "#        Par  : %d\n", (sgf_tree_start + k)->parent->number );
        }
        else {
            printf( "#        Par  : ??\n" );
        }
        printf( "#        Prop : %d\n", (sgf_tree_start + k)->property_count );

        for ( l = 0; l < (sgf_tree_start + k)->property_count; l++ ) {
            printf( "##           PName: %s\n", ((sgf_tree_start + k)->property + l)->name );
            printf( "##           PNr. : %d\n", ((sgf_tree_start + k)->property + l)->number );
            printf( "##           ValNr: %d\n", ((sgf_tree_start + k)->property + l)->value_count );
            for ( m = 0; m < ((sgf_tree_start + k)->property + l)->value_count; m++ ) {
                printf( "####            Val: %s\n", *(((sgf_tree_start + k)->property + l)->value + m) );
            }
        }
    }
    // */

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

/**
 * @brief       Adds property to node
 *
 * Adds a property to the current node of the SGF tree
 *
 * @param[in]   sgf_tree        Pointer to SGF tree
 * @param[out]  property_name   Name of property to add
 * @return      Nothing
 * @todo        malloc needs free ...
 */
void add_property( struct node *sgf_tree, char property_name[] )
{
    struct property_st *property;

    int property_count = sgf_tree->property_count;

    // ... where to free() this stuff ... ??
    property = malloc( sizeof( struct property_st ) );
    if ( property == NULL ) {
        fprintf( stderr, "malloc for property failed\n" );
        exit(1);
    }
    //printf("Prop: %p\n", property );

    property->name = malloc( strlen(property_name) + 1 );
    if ( property->name == NULL ) {
        fprintf( stderr, "malloc for property->name failed\n" );
        exit(1);
    }
    //printf("PropName: %p\n", property->name );

    my_strcpy( property->name, property_name, (int) strlen(property_name) + 1 );

    property->number = property_count;

    if ( property_count == 1 ) {
        sgf_tree->property = malloc( sizeof( struct property_st ) );
    }
    else if ( property_count > 1 ) {
        sgf_tree->property = realloc( sgf_tree->property, sizeof( struct property_st ) * property_count );
    }
    *(sgf_tree->property + property_count - 1 ) = *property;

    //free(property->name);
    free(property);

    return;
}

