#ifndef RUN_POGRAM_H
#define RUN_POGRAM_H

#include "io.h"

/// Defines the number of known GTP commands.
#define COUNT_KNOWN_COMMANDS 13

void init_known_commands(void);
void select_command( struct command *command_data );

int run( int argc, char **argv );

#endif
