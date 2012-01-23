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


/*******************************************************************/
/*  Usage     : read_gtp_input()                                   */
/*  Purpose   : Reads a GTP command from STDIN.                    */
/*  Parameter : Pointer to command struct                          */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : Compare with "gnugo --mode gtp"                    */
/*            : Input larger than SIZE_INPUT_BUFFER is simply      */
/*            : ignored.                                           */
/*  See also  : http://www.lysator.liu.se/~gunnar/gtp/             */
/*******************************************************************/

/**
 *  @brief Read GTP command from STDIN.
 *
 *  Reads a GTP command from STDIN. Input which is larger than
 *  SIZE_INPUT_BUFFER is simply ignored. The behaviour \e should be the same as
 *  with "gnugo --mode gtp".
 *
 *  @param[in]  *command_data  Pointer to struct command
 *  @return     nothing
 *  @sa         <a href=@link3>GTP version 2.0</a>
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

/*******************************************************************/
/*  Usage     : set_output_error()                                 */
/*  Purpose   : Sets the variable output_error to true             */
/*  Parameter : None                                               */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : This function should be called if the output is    */
/*            : considered an error.                               */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void set_output_error(void) {

    output_error = true;

    return;
}

/*******************************************************************/
/*  Usage     : get_output_error()                                 */
/*  Purpose   : Rteurns the state of output_error (true|false)     */
/*  Parameter : None                                               */
/*  Returns   : true|false                                         */
/*  Throws    : Nothing                                            */
/*  Comment   :                                                    */
/*  See also  : [n/a]                                              */
/*******************************************************************/
bool get_output_error(void) {

    return output_error;
}

/*******************************************************************/
/*  Usage     : add_output()                                       */
/*  Purpose   : Adds the given string to the output variable       */
/*  Parameter : String                                             */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : Only complete lines must be given to this          */
/*            : function. A new line is appended automatically.    */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void add_output( const char to_output[] ) {

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


/*******************************************************************/
/*  Usage     : print_output()                                     */
/*  Purpose   : Prints the content of output to STDOUT.            */
/*  Parameter : Command ID                                         */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   :                                                    */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void print_output( int command_id ) {

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

/*******************************************************************/
/*  Usage     : trim()                                             */
/*  Purpose   : Trims string.                                      */
/*  Parameter : Input string                                       */
/*  Returns   : Trimmed string                                     */
/*  Throws    : Nothing                                            */
/*  Comment   : Whitespace at the beginning and end are dropped.   */
/*            : Two or more whitespaces are substituted by one     */
/*            : whitespace.                                        */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void trim( char * input ) {
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

/*******************************************************************/
/*  Usage     : drop_comment()                                     */
/*  Purpose   : Drop trailing comment from input string.           */
/*  Parameter : Input string                                       */
/*  Returns   : String without trailing comment.                   */
/*  Throws    : Nothing                                            */
/*  Comment   : The comment character '#' is substituted with '\0' */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void drop_comment( char * input ) {
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

/*******************************************************************/
/*  Usage     : is_input_empty()                                   */
/*  Purpose   : Returns true if no input has been given.           */
/*  Parameter : None                                               */
/*  Returns   : true|false                                         */
/*  Throws    : Nothing                                            */
/*  Comment   :                                                    */
/*  See also  : [n/a]                                              */
/*******************************************************************/
bool is_input_empty(void) {

    return input_empty;
}

/*******************************************************************/
/*  Usage     : parse_gtp_input()                                  */
/*  Purpose   : Parses the GTP input.                              */
/*  Parameter : Input string, command                              */
/*  Returns   : GTP command and optional arguments                 */
/*  Throws    : Nothing                                            */
/*  Comment   : The input has to go through drop_comment() and     */
/*            : trim() before being parsed.                        */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void parse_gtp_input( char * command_input_buffer, char tokens[][MAX_TOKEN_LENGTH] ) {
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

/*******************************************************************/
/*  Usage     : init_tokens()                                      */
/*  Purpose   : Sets all token data strings to '\0'                */
/*  Parameter : None                                               */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : None                                               */
/*  See also  : [n/a]                                              */
/*******************************************************************/
void init_tokens( char tokens[][MAX_TOKEN_LENGTH] ) {
    int i;

    for ( i = 0; i < MAX_TOKEN_COUNT; i++ ) {
        tokens[i][0] = '\0';
    }

    return;
}

/*******************************************************************/
/*  Usage     : identify_tokens()                                  */
/*  Purpose   : Tries to identify the different parts of the       */
/*            : tokens array: id, command name, arguments          */
/*  Parameter : Tokens array, Pointer to struct command            */
/*  Returns   : Nothing                                            */
/*  Throws    : Nothing                                            */
/*  Comment   : This function moves the data from the tokens array */
/*            : to the command struct.                             */
/*  See also  : [n/a]                                              */
/*******************************************************************/
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
