#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "global_const.h"
#include "io.h"
#include "global_tools.h"

char command_input_buffer[SIZE_INPUT_BUFFER];

bool input_empty    = false;
bool output_error   = false;
char output[MAX_OUTPUT_LENGTH];


/**
 * @brief  Read GTP command from STDIN.
 *
 * Reads a GTP command from STDIN. Input which is larger than
 * SIZE_INPUT_BUFFER is simply ignored. The behaviour \e should be the same as
 * with "gnugo --mode gtp".
 *
 * @param[in]  *command_data  Pointer to struct command
 * @return     nothing
 * @sa         <a href=@link3>GTP version 2.0</a>
 */
void read_gtp_input( struct command *command_data )
{
    int c = '\n';
    int i = 0;
    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_LENGTH];

    init_tokens(tokens);

    input_empty  = false;
    output_error = false;

    do {
        c = getchar();
        command_input_buffer[i] = (char) c;
        i++;
    } while ( c != '\n' && i < SIZE_INPUT_BUFFER );

    // Overwrite last char with newline
    command_input_buffer[i-1] = '\0';

    drop_comment(command_input_buffer);
    trim(command_input_buffer);

    if ( strlen(command_input_buffer) == 0 ) {
        input_empty = true;
        return;
    }

    parse_gtp_input( command_input_buffer, tokens );
    identify_tokens( tokens, command_data );

    return;
}

/**
 * @brief       Sets flag that indicates an GTP error.
 *
 * This function sets a flag that indicates that an error according to the Go
 * Text Protocol has happened.
 *
 * @return      nothing
 * @sa          <a href=@link4>GTP version 2.0, 2.7 Error Messages</a>
 * @note        This function must be called if the output is considered an
 *              error.
 */
void set_output_error(void)
{
    output_error = true;

    return;
}

/**
 * @brief       Checks if the output is considered an GTP error.
 *
 * Returns the value of a flag, that indicates if an output is considered an
 * error according to the Go Text Protocol.
 *
 * @return      true|false
 * @sa          <a href=@link4>GTP version 2.0, 2.7 Error Messages</a>
 */
bool get_output_error(void)
{
    return output_error;
}

/**
 * @brief       Adds a given string to the output string.
 *
 * A given string is added (appended) to the output variable, which will be
 * printed to STDOUT later.
 *
 * @param[in]   to_output   string
 * @return      nothing
 * @note        Only complete lines must be given to this function. A new line
 *              character is appended automatically.
 * @warning     If the resulting string exceeds MAX_OUTPUT_LENGTH, the program
 *              exits with an error.
 */
void add_output( const char to_output[] )
{
    int new_output_length
        = (int)( strlen(output) + strlen(to_output) + 1 );
    if ( new_output_length > MAX_OUTPUT_LENGTH ) {
        fprintf( stderr, "MAX_OUTPUT_LENGTH exceeded\n" );
        exit(EXIT_FAILURE);
    }

    strcat( output, to_output );
    strcat( output, "\n" );

    return;
}


/**
 * @brief       Prints a GTP answer to STDOUT.
 *
 * This function takes the output string and prints it to STDOUT. Depending on
 * the value of the output_error flag a leading '=' or '?' is printed. An
 * optional command id may be printed too.
 *
 * @param[in]   command_id  ID of the last given command; may be zero
 * @return      nothing
 * @sa          <a href=@link3>GTP version 2.0</a>
 * @note        A command_id must be given always, but it will only be printed
 *              to STDOUT if it is larger than zero.
 */
void print_output( int command_id )
{
    /*
    if ( input_empty == true ) {
        input_empty = false;
        return;
    }
    */

    if ( output_error == false ) {
        printf("=");
    }
    else {
        printf("?");
    }

    if ( command_id >= 0 ) {
        printf( "%d", command_id );
    }

    printf(" ");
    
    // If output is empty we fill it with an empty string to
    // get that additional newline:
    if ( strlen(output) == 0 ) {
        add_output("");
    }

    printf( "%s\n", output );

    my_strcpy( output, "", MAX_OUTPUT_LENGTH );
    //output_error = false;

    return;
}

/**
 * @brief       Trims whitespaces in given string.
 *
 * Whitespaces at the beginning and at the end of a given string are dropped.
 * Two or more adjacent whitespaces are trimmed down to one whitespace
 * anywhere within the string. The changes are done inplace.
 *
 * @param[in]   input   string
 * @return      nothing
 * @note        The changes are done inplace. The given string is changed.
 */
void trim( char * input )
{
    char temp_input[SIZE_INPUT_BUFFER];
    char current_char = '\0';
    char last_char    = '\0';
    int i = 0;
    int j = 0;

    for ( i = 0; i < SIZE_INPUT_BUFFER; i++ ) {
        current_char = input[i];

        /* Skip leading whitespace */
        if ( isspace(current_char) && j == 0 ) {
            continue;
        }
        
        /* Write only one whitespace */
        if ( isspace(last_char) && !isspace(current_char) && current_char != '\0' ) {
            temp_input[j] = ' ';
            j++;
        }

        /* Write non-whitespace characters */
        if ( ! isspace(current_char) ) {
            temp_input[j] = current_char;
            j++;
        }

        last_char = current_char;
    }

    temp_input[j] = '\0';

    strncpy( input, temp_input, SIZE_INPUT_BUFFER );

    return;
}

/**
 * @brief       Drops a trailing comment from an GTP input string.
 *
 * A comment from an GTP input command is dropped.
 *
 * @param[in]   input   GTP input command
 * @return      nothing
 * @sa          <a href="@link5">GTP version 2.0, 2.9 Comments</a>
 * @note        The comment character '#' is simply replaced with '\\0'.
 */
void drop_comment( char * input )
{
    int i = 0;
    char current_char = '\0';

    for ( i = 0; i < SIZE_INPUT_BUFFER; i++ ) {
        current_char = input[i];
        if ( current_char == '#' ) {
            input[i] = '\0';
            break;
        }
        if ( current_char == '\0' ) {
            break;
        }
    }

    return;
}

/**
 * @brief       Checks if an GTP input is empty.
 *
 * Checks if a GTP command is empty by returning the value of the input_empty
 * flag.
 *
 * @return      nothing
 * @sa          read_gtp_input() sets the input_empty flag.
 */
bool is_input_empty(void)
{
    return input_empty;
}

/**
 * @brief       Parses GTP input command into list of tokens.
 *
 * The string containing a GTP input command is taken and split into a list of
 * tokens. The input must go through drop_comment() and trim() before this
 * function is called!
 *
 * @param[in]   command_input_buffer    GTP input string
 * @param[out]  tokens                  Array of strings
 * @return      nothing
 * @note        The last token is set to '\\0' to mark the end of the
 *              arguments later on.
 */
void parse_gtp_input( char * command_input_buffer, char tokens[][MAX_TOKEN_LENGTH] )
{
    char current_char = '\0';
    int i = 0;  // Index of input buffer
    int j = 0;  // Counts number of tokens
    int k = 0;  // Index of each token

    // Get tokens from input:
    for ( i = 0; i < SIZE_INPUT_BUFFER; i++ ) {
        current_char = command_input_buffer[i];
        if ( ! isspace(current_char) && current_char != '\0' ) {
            if ( k < MAX_TOKEN_LENGTH ) {
                tokens[j][k] = current_char;
                k++;
            }
            else {
                set_output_error();
                add_output( "MAX_TOKEN_LENGTH exceeded" );
                init_tokens(tokens);
                return;
            }
        }
        else {
            tokens[j][k] = '\0';
            j++;
            k = 0;

            if ( j >= MAX_TOKEN_COUNT ) {
                set_output_error();
                add_output( "MAX_TOKEN_COUNT exceeded" );
                init_tokens(tokens);
                return;
            }

            // Set terminating argument:
            tokens[j][0] = '\0';
        }

        if ( current_char == '\0' ) {
            break;
        }
    }

    return;
}

/**
 * @brief       Initializes tokens data structure.
 *
 * Initializes the tokens data structure, which is an array of strings, by
 * setting all elements to '\\0'.
 *
 * @param[out]  tokens  Array of strings
 * @return      nothing
 */
void init_tokens( char tokens[][MAX_TOKEN_LENGTH] )
{
    int i;

    for ( i = 0; i < MAX_TOKEN_COUNT; i++ ) {
        tokens[i][0] = '\0';
    }

    return;
}


/**
 * @brief       Identifies the tokens in the tokens data structure.
 *
 * Identifies the tokens in the tokens data structure and rebuilds this data
 * into a command data structure.
 * The following tokens will be identified:
 *  - command ID (optional)
 *  - command name
 *  - array of arguments (optional)
 *
 * @param[in]   tokens          Array of tokens
 * @param[out]  command_data    Pointer to struct command
 * @return      nothing
 * @note        The tokens array must have been filled by parse_gtp_input()
 *              before.
 */
void identify_tokens( char tokens[][MAX_TOKEN_LENGTH], struct command *command_data ) {
    int id;
    int arg_start;  // Index of first argument
    int i, j;

    // Check if first token is regular id,
    // if not it must be the command name.
    id = atoi( tokens[0] );
    if ( id > 0 ) {
        command_data->id = id;
        my_strcpy( command_data->name, tokens[1], MAX_TOKEN_LENGTH );
        arg_start = 2;
    }
    else if ( id < 0 ) {
        command_data->id = -1;
        my_strcpy( command_data->name, tokens[1], MAX_TOKEN_LENGTH );
        arg_start = 2;
    }
    else {
        command_data->id = -1;
        my_strcpy( command_data->name, tokens[0], MAX_TOKEN_LENGTH );
        arg_start = 1;
    }

    // Check for special case id 0:
    if ( strcmp( tokens[0], "0" ) == 0 ) {
        command_data->id = 0;
        my_strcpy( command_data->name, tokens[1], MAX_TOKEN_LENGTH );
        arg_start = 2;
    }

    // Copy arguments into command struct:
    j = 0;
    for ( i = arg_start; i < MAX_TOKEN_COUNT; i++ ) {
        if ( tokens[i][0] == '\0' ) {
            break;
        }
        my_strcpy( command_data->gtp_argv[j], tokens[i], MAX_TOKEN_LENGTH );
        j++;
    }
    command_data->gtp_argc = j;

    // Add terminating argument:
    my_strcpy( command_data->gtp_argv[j], "\0", MAX_TOKEN_LENGTH );

    // DEBUG
    /*
    printf( "ID: %d\n", command_data->id );
    printf( "NAME: %s\n", command_data->name );
    printf( "ARGC: %d\n", command_data->argc );
    for ( i = 0; i < j; i++ ) {
        printf( "ARG %d: %s\n", i, command_data->argv[i] );
    }
    */

    return;
}
