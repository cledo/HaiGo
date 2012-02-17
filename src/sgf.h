#ifndef SGF_H
#define SGF_H

//! @brief Defines a node in the SGF tree
struct node_st {
    int  number;        //!< Node number starting with zero
    bool is_main;       //!< Flag that shows if node is part of the main line
    int  tree_nr;       //!< Game tree number
    int  tree_level;    //!< Game tree level as defined by opening and closing braces
    struct node_st     *parent;     //!< Pointer to the parent node
    struct property_st *property;   //!< List of properties of this node
    int  property_count;            //!< Number of properties of this node
};

//! @brief Defines a node's property
struct property_st {
    int  number;        //!< Number of this property in current node
    char *name;         //!< Stores the property's name
    char **value;       //!< List of values of this property
    int  value_count;   //!< Number of values of this property
};

struct node_st * parse_sgf( char *file_content );

#endif
