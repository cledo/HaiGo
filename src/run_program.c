#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "global_const.h"
#include "run_program.h"
#include "io.h"
#include "board.h"
#include "move.h"
#include "global_tools.h"


//! The default help message shown with -h
static const char help_message[] =
"This is a placeholder for the help message.\n\
This message is shown when the program is called\n\
with the command line argument -h.\n";

//! If set to 1 the main control loop exits.
static int quit_program = 0;

//! @brief Connects one given command name with proper function pointer.
//! @todo Check if function declaration needs parameters!
struct command_func {
    char command[MAX_TOKEN_LENGTH]; //!< Sets the name of the GTP command.
    void (*function)();             //!< Sets the pointer to the function for GTP command.
};

//! Array of known (implemented) commands
static struct command_func known_commands[COUNT_KNOWN_COMMANDS];

//! The current komi value
static float komi = 0.0;

static void init_known_commands(void);
static void read_opts( int argc, char ** argv );
static void select_command( struct command *command_data );
static void print_help_message(void);
static void print_version(void);
static void set_quit_program(void);

/* Administrative commands */
static void gtp_protocol_version( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_name( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_version( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_known_command( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_list_commands( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_quit( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );

/* Setup commands */
static void gtp_boardsize( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_clear_board( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_komi( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );

/* Core play commands */
static void gtp_play( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
/// @todo These functions have to be implemmted: gtp_genmove, gtp_undo.
/// - void gtp_genmove( int argc, char argv[][MAX_TOKEN_LENGTH] );
/// - void gtp_undo( int argc, char argv[][MAX_TOKEN_LENGTH] );

/* Debug commands */
static void gtp_showboard( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );


/**
 *  @brief Substitute for main() function, because main() itself cannot be unit-tested with check.
 * 
 *  The run() function performs the following tasks:
 *      -# Initialization
 *      -# STDOUT buffer size set to NULL
 *      -# Checking command line arguments (like -h, -v, etc.)
 *      -# Starting the working loop
 * 
 *  @param[in]  argc    Number of command line arguments (same as for main()).
 *  @param[in]  argv    Array of all command line arguments (same as for main()).
 *  @return         EXIT_SUCCESS | EXIT_FAILURE
 *  @sa     info check
 */
int run( int argc, char **argv )
{
    struct command command_data;

    // Initialization
    init_board(BOARD_SIZE_DEFAULT);
    init_known_commands();
    init_move_history();

    // STDOUT must be unbuffered:
    setbuf( stdout, NULL );

    // Read command line arguments:
    read_opts( argc, argv );

    // Working loop:
    while ( quit_program == 0 ) {
        read_gtp_input(&command_data);

        // Ignore empty lines:
        if ( is_input_empty() == true ) {
            continue;
        }

        if ( get_output_error() == false ) {
            select_command(&command_data);
        }

        print_output(command_data.id);
    }

    free_board();

    return EXIT_SUCCESS;
}


/**
 *  @brief Sets control variable for working loop.
 * 
 *  The set_quit_program() function sets the variable quit_program
 *  to 1. When this variable is 1, the control loop stops and the
 *  program exits.
 *  
 *  @return nothing
 *  @sa     [n/a]
 */
void set_quit_program(void)
{

    quit_program = 1;

    return;
}

/**
 *  @brief Parses command line arguments.
 * 
 *  read_opts() parses the command line arguments with which the
 *  program has been started. ( Do not confuse these with the
 *  arguments given to a GTP command. ) For certain arguments --
 *  like -h or -v -- the appropriate function is called.
 * 
 *  @param[in]  argc    Number of arguments (same as for main())
 *  @param[in]  argv    Array of arguments (same as for main())
 *  @return     nothing
 *  @sa         man 3 getopt
 *  @todo       The call to free_board() under the default switch should
 *              probably be subsituted by a more sophisticated clean_up()
 *              function.
 */
void read_opts( int argc, char **argv )
{
    int opt;

    while ( ( opt = getopt( argc, argv, VALID_OPTIONS ) ) != -1 ) {
        switch (opt) {
            case 'h':
                print_help_message();
                set_quit_program();
                break;
            case 'v':
                print_version();
                set_quit_program();
                break;
            default:
                free_board();   // See @todo above.
                exit(EXIT_FAILURE);
        }
    }

    return;
}

/**
 *  @brief Prints help message to STDOUT.
 *
 *  print_help_message() prints a help message to STDOUT.
 *  The help message itself is defined in help_message.
 *  This function is called when the command line paramter -h is set.
 *
 *  @return     nothing
 *  @sa         [n/a]
 */
void print_help_message(void)
{

    printf( "%s", help_message );

    return;
}

/**
 *  @brief Prints name and version number to STDOUT.
 *
 *  print_version() prints the name aof the program and its version number of
 *  the program to STDOUT.
 *  The name is defined in PROGRAM_NAME, the version is defined in
 *  PROGRAM_VERSION.
 *  This function is called when the command line parameter -v is set.
 *
 *  @return     nothing
 *  @sa         [n/a]
 */
void print_version(void)
{

    printf( "%s %s\n", PROGRAM_NAME, PROGRAM_VERSION );

    return;
}

/**
 *  @brief Initializes all known GTP commands.
 *
 *  Initializes a data structure for all implemented GTP commands.
 *  The data structure is an array of pairs of names and function pointers.
 *
 *  @return     nothing
 *  @sa         [n/a]
 */
void init_known_commands(void)
{
    int i = 0;

    my_strcpy( known_commands[i].command, "protocol_version", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_protocol_version);
    my_strcpy( known_commands[i].command, "name", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_name);
    my_strcpy( known_commands[i].command, "version", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_version);
    my_strcpy( known_commands[i].command, "known_command", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_known_command);
    my_strcpy( known_commands[i].command, "list_commands", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_list_commands);
    my_strcpy( known_commands[i].command, "quit", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_quit);
    my_strcpy( known_commands[i].command, "boardsize", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_boardsize);
    my_strcpy( known_commands[i].command, "clear_board", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_clear_board);
    my_strcpy( known_commands[i].command, "komi", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_komi);
    my_strcpy( known_commands[i].command, "play", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_play);
    my_strcpy( known_commands[i].command, "showboard", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_showboard);

    //DEBUG:
    my_strcpy( known_commands[i].command, "showgroups", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*print_groups);

    if ( i != COUNT_KNOWN_COMMANDS ) {
        fprintf(
            stderr
          , "Commands implemented: %d\nCOUNT_KNOWN_COMMANDS: %d\n"
          , i, COUNT_KNOWN_COMMANDS
        );
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 *  @brief Selects the function that is associated with a certain GTP command.
 *
 *  select_command() receives a comman data structure and calls the function
 *  which is associated with this GTP command.
 *
 *  @param[in]  *command_data   struct command
 *  @return     nothing
 *  @sa         [n/a]
 */
void select_command( struct command *command_data )
{
    int  i;
    bool is_command = false;

    for ( i = 0; i < COUNT_KNOWN_COMMANDS; i++ ) {
        if ( strcmp( known_commands[i].command, command_data->name ) == 0 ) {
            is_command = true;
            known_commands[i].function( command_data->gtp_argc, command_data->gtp_argv );
            break;
        }
    }

    if ( is_command == false ) {
        set_output_error();
        add_output("unknown command");
    }

    return;
}


/// @defgroup GTP_Commands Go Text Protocol Commands
/// The following functions are implemented as defined in the
/// Go Text Protocol version 2

/// @defgroup GTP_Administrative_Commands Go Text Protocol Administrative Commands
/// @ingroup GTP_Commands

/**
 *  @brief Quits the program.
 * 
 *  gtp_quit() quits the whole program by calling set_quit_program().
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_quit( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    set_quit_program();

    return;
}

/**
 *  @brief Shows the program's version number.
 * 
 *  gtp_version() shows the version number as defined by PROGRAM_VERSION.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_version( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    add_output(PROGRAM_VERSION);

    return;
}

/**
 *  @brief Shows the used GTP version number.
 * 
 *  gtp_protocol_version() shows the currently used Go Text Protocol
 *  version number. Currently this is version number 2 as defined
 *  in GTP_VERSION.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_protocol_version( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    add_output(GTP_VERSION);

    return;
}

/**
 *  @brief Shows the program's name.
 * 
 *  gtp_name() shows the name of the program as defined by PROGRAM_NAME.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_name( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    add_output(PROGRAM_NAME);

    return;
}

/**
 *  @brief Shows whether a given GTP command is implemented or not.
 * 
 *  gtp_known_command() \<command_name\> returns either the string
 *  "true" or "false" therefore showing whether a given GTP command
 *  is known or not.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     "true" | "false"
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_known_command( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int  i;
    bool is_command_known = false;

    if ( gtp_argc < 1 ) {
        set_output_error();
        add_output( "missing argument: command_name" );
        return;
    }
    if ( gtp_argc > 1 ) {
        set_output_error();
        add_output( "only one argument required: command_name" );
        return;
    }

    for ( i = 0; i < COUNT_KNOWN_COMMANDS; i++ ) {
        if ( strcmp( known_commands[i].command, gtp_argv[0] ) == 0 ) {
            is_command_known = true;
            break;
        }
    }

    if ( is_command_known == true ) {
        add_output("true");
    }
    else {
        add_output("false");
    }

    return;
}

/**
 *  @brief Shows a list of all know GTP commands.
 * 
 *  gtp_list_commands() shows the name of the program as defined
 *  by PROGRAM_NAME.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.1 Administrative Commands
 * 
 *  @ingroup GTP_Administrative_Commands
 */
void gtp_list_commands( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int i;

    for ( i = 0; i < COUNT_KNOWN_COMMANDS; i++ ) {
        add_output( known_commands[i].command );
    }

    return;
}

/// @defgroup GTP_Setup_Commands Go Text Protocol Setup Commands
/// @ingroup GTP_Commands

/**
 *  @brief Changes the current board size.
 * 
 *  gtp_boardsize() changes the current size of the board.
 *  by PROGRAM_NAME.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.2 Setup Commands
 * 
 *  @ingroup GTP_Setup_Commands
 */
void gtp_boardsize( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int board_size = (int) atoi( gtp_argv[0] );

    if ( board_size < BOARD_SIZE_MIN || board_size > BOARD_SIZE_MAX ) {
        set_output_error();
        add_output("unacceptable size");
        return;
    }

    free_board();
    init_board(board_size);

    return;
}

/**
 *  @brief Clears the board.
 * 
 *  gtp_clear_board() clears the board. The number of captured stones
 *  is set to zero for both colors. The move history is reset to
 *  empty.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.2 Setup Commands
 * 
 *  @ingroup GTP_Setup_Commands
 *
 * @todo    Set number of captured stones to zero; clear move history.
 */
void gtp_clear_board( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int board_size = get_board_size();

    free_board();
    init_board(board_size);

    // number of captured stones must be set to zero
    // move history must be emptied

    return;
}

/**
 *  @brief Sets komi.
 * 
 *  gtp_komi() sets the komi to the given value.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.2 Setup Commands
 * 
 *  @ingroup GTP_Setup_Commands
 */
void gtp_komi( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    // Check arg here!
    
    komi = atof( gtp_argv[0] );

    return;
}

/// @defgroup GTP_Core_Play_Commands Go Text Protocol Core Play Commands
/// @ingroup GTP_Commands

/**
 *  @brief Description missing!
 * 
 *  gtp_play() Description missing!
 *
 *  @todo Description is missing!
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.3 Core Play Commands
 * 
 *  @ingroup GTP_Core_Play_Commands
 */
void gtp_play( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int color;
    int i, j;
    int nr_of_removed_stones;
    int group_nr;
    int nr_of_liberties;
    int group_size;
    int captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int ko_i, ko_j;

    // Check if first argument is black or white:
    str_toupper( gtp_argv[0] );
    if ( strcmp( gtp_argv[0], "B" ) == 0 || strcmp( gtp_argv[0], "BLACK" ) == 0 ) {
        color = BLACK;
    }
    else if ( strcmp( gtp_argv[0], "W" ) == 0 || strcmp( gtp_argv[0], "WHITE" ) == 0 ) {
        color = WHITE;
    }
    else {
        set_output_error();
        add_output("invalid color");
        return;
    }

    // Check vertex if first coordinate is valid:
    i = (int) toupper( gtp_argv[1][0] ) - 65;
    if ( i > 8 ) {
        i--;
    }
    if ( i < 0 || i >= get_board_size() ) {
        set_output_error();
        add_output("invalid coordinate");
        return;
    }

    // Check if second coordinate is valid:
    gtp_argv[1][0] = ' ';
    j = (int) atoi( gtp_argv[1] );
    j--;
    if ( j < 0 || j >= get_board_size() ) {
        set_output_error();
        add_output("invalid coordinate");
        return;
    }

    // Check if vertex is empty:
    if ( get_vertex( i, j ) != EMPTY ) {
        set_output_error();
        add_output("illegal move");
        return;
    }

    // Check for ko repetition:
    ko_i = get_move_last_ko_i();
    ko_j = get_move_last_ko_j();
    if ( ko_i != -1 && ko_j != -1 ) {
        if ( ko_i == i && ko_j == j ) {
            set_output_error();
            add_output("illegal move");
            return;
        }
    }


    set_vertex( color, i, j );

    create_groups();
    count_liberties();
    set_groups_size();

    nr_of_removed_stones = remove_stones( color * -1 );

    // If board has changed, rebuild groups and liberties:
    if ( nr_of_removed_stones > 0 ) {
        create_groups();
        count_liberties();
        set_groups_size();
    }

    group_nr        = get_group_nr( i, j );
    nr_of_liberties = get_nr_of_liberties(group_nr);
    group_size      = get_size_of_group(group_nr);

    // If liberties are zero, move is invalid
    if ( nr_of_liberties == 0 ) {
        set_vertex( EMPTY, i, j );
        set_output_error();
        add_output("illegal move");
        return;
    }

    nr_of_removed_stones = get_captured_now(captured_now);

    create_next_move();
    set_move_vertex( color, i, j );
    set_move_captured_stones(captured_now);

    // Check if this is a ko:
    if ( nr_of_removed_stones == 1 && group_size == 1 && nr_of_liberties == 1 ) {
        set_move_ko( captured_now[0][0], captured_now[0][1] );
    }

    // Add move to move history:
    push_move();

    return;
}

/// @defgroup GTP_Debug_Commands Go Text Protocol Debug Commands
/// @ingroup GTP_Commands

/**
 *  @brief Shows a simple ASCII board.
 * 
 *  gtp_showboard() gets a string representation of the board and
 *  sends it to the board_output variable, so it can then be printed.
 * 
 *  @param[in]  gtp_argc    Number of arguments of GTP command
 *  @param[in]  gtp_argv    Array of all arguments for GTP command
 *  @return     nothing
 *  @sa     Go Text Protokol version 2, 6.3.6 Debug Commands
 * 
 *  @ingroup GTP_Debug_Commands
 */
void gtp_showboard( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    char board_output[MAX_OUTPUT_LENGTH];

    get_board_as_string(board_output);
    add_output(board_output);

    return;
}
