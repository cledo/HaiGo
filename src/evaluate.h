#ifndef EVALUATE_H
#define EVALUATE_H

/**
 *  @file   evaluate.h
 *
 *  @brief  Interface definition for evaluate.c
 *
 */

void init_brains(void);
void set_factor( int index, int factor );
int  get_factor( int index );
int  evaluate_position( int value_list[], bool do_full_eval );

#endif
