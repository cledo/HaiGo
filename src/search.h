#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>

/**
 *  @file   search.h
 *
 *  @brief  Interface definition for search.c
 *
 */

void search_tree( int color, int *i, int *j );

void set_search_level( int level );
int  get_search_level(void);

bool get_do_log(void);
void set_do_log(void);

#endif
