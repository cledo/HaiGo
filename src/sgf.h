#ifndef SGF_H
#define SGF_H

/**
 * @file    sgf.h
 *
 * @brief   Interface definitions for sgf.c
 *
 */

//! @brief Defines a node in the SGF tree
typedef struct node_st {
    int  number;                    //!< Node number starting with zero
    bool is_main;                   //!< Flag that shows if node is part of the main line
    int  tree_nr;                   //!< Game tree number
    int  tree_level;                //!< Game tree level as defined by opening and closing braces
    struct node_st *parent;         //!< Pointer to the parent node
    struct property_st *property;   //!< List of properties of this node
    int  property_count;            //!< Number of properties of this node
} node_t;

//! @brief Defines a node's property
typedef struct property_st {
    int  number;        //!< Number of this property in current node
    char *name;         //!< Stores the property's name
    char **value;       //!< List of values of this property
    int  value_count;   //!< Number of values of this property
} property_t;

node_t * parse_sgf( char *file_content );
void     free_sgf_tree( node_t *sgf_tree );

#endif
