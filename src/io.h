#ifndef IO_H
#define IO_H


#include <stdbool.h>
#include "global_const.h"

#define SIZE_INPUT_BUFFER 256   //!< Size of input buffer


/**
 * @file    io.h
 *
 * @brief   Interface definition for io.c
 *
 */

//! @brief  Data structure to define GTP commands.
struct command {
    int  id;                                            //!< Id of GTP command
    char name[MAX_TOKEN_LENGTH];                        //!< Name of GTP command
    char gtp_argv[MAX_TOKEN_COUNT+1][MAX_TOKEN_LENGTH]; //!< Argument vector for GTP command
    int  gtp_argc;                                      //!< Argument count for GTP command
};

/**
 * @name Input related functions
 *
 */

//@{
void read_gtp_input( /*@out@*/ struct command *command_data );
void trim( char * input );
void drop_comment( char * input );
void parse_gtp_input( char * command_input_buffer, char command[][MAX_TOKEN_LENGTH] );
void init_tokens( char tokens[][MAX_TOKEN_LENGTH] );
void identify_tokens( char tokens[][MAX_TOKEN_LENGTH], struct command *command_data );
bool is_input_empty(void);
//@}

/**
 * @name Output related functions
 *
 */

//@{
void add_output( const char to_output[] );
void set_output_error(void);
bool get_output_error(void);
void print_output( int command_id );
//@}

#endif

