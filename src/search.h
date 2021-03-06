#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include <time.h>

/**
 *  @file   search.h
 *
 *  @brief  Interface definition for search.c
 *
 */

/**
 * @brief   Structure that stores information about last generated move.
 *
 **/
typedef struct {
    char   color[6];                        //!< String representation of color of last move.
    char   move[4];                         //!< String representation of last generated move.
    int    level;                           //!< Search level of last generated move.
    time_t duration;                        //!< Duration of last search in seconds.
    unsigned long long int node_count;      //!< Number of created nodes in search tree.
    unsigned long long int nodes_per_sec;   //!< Number of created nodes per second.
    int qsearch_count;                      //!< Number of quiet search nodes created.
    unsigned int hash_hit;                  //!< Number of hash hits.
    int alpha_cut;                          //!< Number of alpha cut-offs;
    int beta_cut;                           //!< Number of beta cut-offs;
    int value;                              //!< Value of move;
} search_stats_t;

void search_tree( int color, int *i, int *j );

void set_search_depth( int depth );
int  get_search_depth(void);

bool get_do_log(void);
void set_do_log(void);

int compare_value_black( const void *move1, const void *move2 );
int compare_value_white( const void *move1, const void *move2 );

void init_search_stats(void);
search_stats_t get_search_stats(void);

#endif

