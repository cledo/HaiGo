#ifndef RUN_POGRAM_H
#define RUN_POGRAM_H

#include "io.h"

/**
 * @file    run_program.h
 *
 * @brief   Interface definition for run_program.c
 *
 */

#define COUNT_KNOWN_COMMANDS 21 //!< Defines the number of known GTP commands.

void init_known_commands(void);
void select_command( struct command *command_data );

int run( int argc, char **argv );

#endif

