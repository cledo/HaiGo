#include <stdlib.h>
#include <stdio.h>  // remove when printf is not needed anymore
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "global_const.h"
#include "sgf.h"
#include "global_tools.h"

/**
 * @file    sgf.c
 *
 * @brief   Parses SGF files
 *
 * Functions and data structures for parsing SGF files.
 * The SGF tree is build as completely as possible, even if the loadsgf GTP
 * command only needs the main line.
 *
 */

static void add_node( struct node_st *sgf_tree_start, int node_nr, int game_tree_nr, int game_tree_level, bool is_main_line );
static void add_property( struct node_st *sgf_tree, char property_name[] );
static void add_value( struct property_st *property_st, char *property_value );

/**
 * @brief       Parses an SGF string
 *
 * Parses a complete SGF string as it is read from an SGF file.
 *
 * @param[in]   file_content    SGF string as read from file
 * @return      Pointer to SGF tree
 * @todo        Should this function write into the move_history or build an
 *              SGF tree?
 */
struct node_st * parse_sgf( char *file_content )
{
    int  k = 0;
    int  l = 0;
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
    struct node_st *sgf_tree;
    struct node_st *sgf_tree_start;
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

    sgf_tree = malloc( sizeof( struct node_st ) * ( node_count + 1 ) );

    sgf_tree_start = sgf_tree;

    while ( ( current_char = file_content[k++] ) != '\0' ) {
        if ( current_char == '(' && ! in_property_value ) {
            game_tree_nr++;
            game_tree_level++;
            //printf( "# New GameTree %d\n", game_tree_level );
        }
        if ( current_char == ')' && ! in_property_value ) {
            //printf( "# End GameTree %d\n", game_tree_level );
            game_tree_level--;
            is_main_line = false;
        }
        if ( current_char == ';' && ! in_property_value ) {
            node_nr++;
            if ( node_nr > 0 ) {
                sgf_tree++;
            }
            //printf( "#### New Node %d\n", node_nr );
            add_node( sgf_tree_start, node_nr, game_tree_nr, game_tree_level, is_main_line );
            property_count = 0;
        }
        if ( isupper(current_char) && ( isspace(last_char) || (last_char == ']') || (last_char == ';') ) && ! in_property_value ) {
            in_property_name = true;
            //printf( "    ## New Property - " );
            value_count = 0;
        }
        if ( in_property_name && ! isupper(current_char) && !  in_property_value ) {
            in_property_name = false;
            property_name[l] = '\0';
            l = 0;
            //printf( "%s\n", property_name );
            sgf_tree->property_count = ++property_count;
            add_property( sgf_tree, property_name );
        }
        if ( current_char == '[' && ! in_property_value ) {
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
            //printf( "    ValNr.: %d\n", value_count );
            (sgf_tree->property + property_count - 1)->value_count = ++value_count;
            add_value( sgf_tree->property + property_count - 1, property_value );

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
    /*
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
    */

    // Mark and of node list:
    sgf_tree++;
    sgf_tree->number  = -1;

    /*
    sgf_tree = sgf_tree_start;
    free(sgf_tree);
    sgf_tree       = NULL;
    sgf_tree_start = NULL;
    */
    free(property_name);
    free(property_value);

    return sgf_tree_start;
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
void add_node( struct node_st *sgf_tree_start, int node_nr, int game_tree_nr, int game_tree_level, bool is_main_line )
{
    int m;
    struct node_st *new_node;

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
void add_property( struct node_st *sgf_tree, char property_name[] )
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

/**
 * @brief       Adds a value to the current property
 *
 * Adds a value to the list of values of the current property
 *
 * @param[in]   property    Pointer to the current property
 * @param[in]   value       String of value to add
 * @return      Nothing
 */
void add_value( struct property_st *property, char *value )
{
    int value_count = property->value_count;

    if ( value_count == 1 ) {
        property->value = malloc( sizeof(char **) );
    }
    else if ( value_count > 1 ) {
        property->value = realloc( property->value, sizeof(char **) * value_count );
    }
    *(property->value + value_count - 1) = malloc( strlen(value) + 1 );
    my_strcpy( *(property->value + value_count - 1), value, (int) strlen(value) );

    return;
}

/**
 * @brief       Frees all allocated memory in SGF tree.
 *
 * The SGF tree is a list of nodes, which contain parameters and their values.
 * All the allocated pointers should be freed after the data has been moved to the
 * move history.
 *
 * @param[in]   sgf_tree    Pointer to SGF tree
 * @return      Nothing
 */
void free_sgf_tree( struct node_st *sgf_tree )
{
    int k, l, m;
    int count_nodes      = 0;
    int count_properties = 0;
    int count_values     = 0;

    struct node_st *sgf_tree_start = sgf_tree;

    // Count nodes:
    while ( sgf_tree->number != -1 ) {
        sgf_tree++;
        count_nodes++;
    }
    sgf_tree = sgf_tree_start;

    // Go through SGF tree nodes from last to first:
    for ( k = count_nodes - 1; k >= 0; k-- ) {
        //printf( "NodeNr.: %d\n", (sgf_tree + k)->number );
        count_properties = (sgf_tree + k)->property_count;
        for ( l = count_properties - 1; l >= 0; l-- ) {
            //printf( "   PropNr.: %d\n", ((sgf_tree +k)->property + l)->number );
            count_values = ((sgf_tree + k)->property + l)->value_count;
            for ( m = count_values - 1; m >= 0; m-- ) {
                //printf( "       Val.: %s\n", *(((sgf_tree + k)->property + l)->value + m) );
                // Free value string:
                free( *(((sgf_tree + k)->property + l)->value + m) );
            }
            // Free value list:
            free( ((sgf_tree + k)->property + l)->value );
            // Free property name:
            free( ((sgf_tree + k)->property + l)->name  );
        }
        // Free properties:
        free( (sgf_tree + k)->property );
    }
    free(sgf_tree);

    return;
}

