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
struct search_stats_st {
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
};

void search_tree( int color, int *i, int *j );

void set_search_level( int level );
int  get_search_level(void);

bool get_do_log(void);
void set_do_log(void);

int compare_value_black( const void *move1, const void *move2 );
int compare_value_white( const void *move1, const void *move2 );

struct search_stats_st get_search_stats(void);

#endif

