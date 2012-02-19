#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "global_const.h"
#include "run_program.h"
#include "io.h"
#include "board.h"
#include "move.h"
#include "global_tools.h"
#include "sgf.h"
#include "./brains/all_brains.h"

/**
 * @file    run_program.c
 *
 * @brief   The file run_program.c contains the main working loop.
 *
 * The file run_program.c contains the main working loop and defines all the
 * GTP commands the program knows about.
 */


//! @brief  The default help message shown with -h
//! @return Nothing
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

static void read_opts( int argc, char ** argv );
static void print_help_message(void);
static void print_version(void);
static void set_quit_program(void);
static bool is_color_valid( char color_str[], int *color );
static bool is_vertex_valid( char vertex_str[], int *i, int *j );
static bool is_vertex_pass( char vertex_str[] );

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
static void gtp_fixed_handicap( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );

/* Core play commands */
static void gtp_play( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_genmove( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );
static void gtp_undo( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );

/* Regression commands */
static void gtp_loadsgf( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );

/* Debug commands */
static void gtp_showboard( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] );


/* SGF parsing commands */
bool sgf_komi( char *value );
bool sgf_size( char *value );
bool sgf_add( int color, char **value, int value_count );
bool sgf_move( int color, char *value );


/* Misc functions */
void i_to_x( int i, char x[] );
void j_to_y( int j, char y[] );
void add_handicap( int i, int j, char output[] );

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

    while ( ( opt = getopt( argc, argv, VALID_OPTIONS ) ) != INVALID ) {
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
    my_strcpy( known_commands[i].command, "fixed_handicap", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_fixed_handicap);
    my_strcpy( known_commands[i].command, "play", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_play);
    my_strcpy( known_commands[i].command, "showboard", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_showboard);
    my_strcpy( known_commands[i].command, "genmove", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_genmove);
    my_strcpy( known_commands[i].command, "undo", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_undo);
    my_strcpy( known_commands[i].command, "loadsgf", MAX_TOKEN_LENGTH );
    known_commands[i++].function = (*gtp_loadsgf);

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
 *  select_command() receives a command data structure and calls the function
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
    init_move_history();

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
    init_move_history();

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
 *  @sa     Go Text Protocol version 2, 6.3.2 Setup Commands
 * 
 *  @ingroup GTP_Setup_Commands
 */
void gtp_komi( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{

    // Check arg here!
    
    komi = atof( gtp_argv[0] );

    return;
}

/**
 * @brief       Sets fixed handicap stones.
 *
 * Sets the fixed handicap stones according to the GTP protocol.
 *
 * @param[in]   gtp_argc    Number of arguments of GTP command
 * @param[in]   gtp_argv    Array of all arguments for GTP command
 * @return      Nothing
 * @sa          Go Text Protocol version 2, 4.1.1 Fixed Handicap Placement
 * @todo        Description still missing!!
 */
void gtp_fixed_handicap( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int  i, j;
    int  handicap;
    char output[40];
    int  max_handicap;
    int  edge_distance;
    int  board_size = get_board_size();

    // Handicap only on empty boards:
    if ( get_move_number() > 0 ) {
        set_output_error();
        add_output("board not empty");

        return;
    }

    // No handicap on mini boards:
    if ( board_size <= 6 ) {
        set_output_error();
        add_output("invalid handicap");

        return;
    }

    // Determine proper edge_distance and max_handicap:
    edge_distance = ( board_size > 12     ) ? 4 : 3;
    max_handicap  = ( board_size % 2 == 0 ) ? 4 : 9;
    if ( board_size == 7 ) {
        max_handicap = 4;
    }

    // Check handicap number:
    handicap = atoi( gtp_argv[0] );
    if ( handicap < 2 || handicap > max_handicap ) {
        set_output_error();
        add_output("invalid handicap");

        return;
    }

    output[0] = '\0';

    i = board_size - edge_distance;
    j = board_size - edge_distance;
    add_handicap( i, j, output );

    i = edge_distance - 1;
    j = edge_distance - 1;
    strcat( output, " " );
    add_handicap( i, j, output );

    if ( handicap == 2 ) {
        add_output(output);

        return;
    }

    i = board_size - edge_distance;
    j = edge_distance - 1;
    strcat( output, " " );
    add_handicap( i, j, output );

    if ( handicap == 3 ) {
        add_output(output);

        return;
    }

    i = edge_distance - 1;
    j = board_size - edge_distance;
    strcat( output, " " );
    add_handicap( i, j, output );

    if ( handicap == 4 ) {
        add_output(output);

        return;
    }

    if ( handicap == 5 || handicap == 7 || handicap == 9 ) {
        i = board_size / 2;
        j = board_size / 2;
        strcat( output, " " );
        add_handicap( i, j, output );
    }
    if ( handicap == 5 ) {
        add_output(output);

        return;
    }

    i = edge_distance - 1;
    j = board_size / 2;
    strcat( output, " " );
    add_handicap( i, j, output );

    i = board_size - edge_distance;
    j = board_size / 2;
    strcat( output, " " );
    add_handicap( i, j, output );

    if ( handicap == 6 || handicap == 7 ) {
        add_output(output);

        return;
    }

    i = board_size / 2;
    j = edge_distance - 1;
    strcat( output, " " );
    add_handicap( i, j, output );

    i = board_size / 2;
    j = board_size - edge_distance;
    strcat( output, " " );
    add_handicap( i, j, output );


    add_output(output);

    return;
}

/**
 * @brief       Adds given stone as handicap.
 *
 * Adds a black stone for the given vertex as handicap. The verteces are
 * appended to the given output string.
 *
 * @param[in]   i   Horizontal coordinate
 * @param[in]   j   Vertical coordinate
 * @param[out]  output  String of verteces of handicap stones.
 * @return      Nothing
 */
void add_handicap( int i, int j, char output[] )
{
    char x[2];
    char y[3];

    set_vertex( BLACK, i, j );
    i_to_x( i, x );
    j_to_y( j, y );
    strcat( output, x );
    strcat( output, y );

    return;
}

/**
 * @brief       Converts i to x.
 *
 * Converts the board coordinate i to its string representation.
 *
 * @param[in]   i   Horizontal numerical board coordinate.
 * @param[out]  x   The string representation of i.
 * @return      Nothing
 * @sa          j_to_y()
 */
void i_to_x( int i, char x[] )
{

    if ( i >= 8 ) {
        i++;
    }
    x[0] = i + 65;
    x[1] = '\0';

    return;
}

/**
 * @brief       Converts j to y.
 *
 * Converts the board coordinate j to its string representation.
 *
 * @param[in]   j   Vertical numerical board coordinate.
 * @param[out]  y   The string representation of j.
 * @return      Nothing
 * @sa          i_to_x()
 */
void j_to_y( int j, char y[] )
{

    j++;
    if ( j < 10 ) {
        y[0] = (char)( j + 48 );
        y[1] = '\0';
    }
    else {
        y[0] = (char)(int)( j / 10 + 48 );
        y[1] = (char)( j % 10 + 48 );
        y[2] = '\0';
    }

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


    // Check if first argument is black or white:
    if ( ! is_color_valid( gtp_argv[0], &color ) ) {
        set_output_error();
        add_output("invalid color");

        return;
    }

    // Check if vertex is PASS:
    if ( is_vertex_pass( gtp_argv[1] ) ) {
        create_next_move();
        set_move_pass(color);
        push_move();

        return;
    }

    // Check if vertex is valid:
    if ( ! is_vertex_valid( gtp_argv[1], &i, &j ) ) {
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
    if ( is_move_ko( color, i, j ) ) {
        set_output_error();
        add_output("illegal move");

        return;
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
        // If only one stone has been captured, it must be the first one in
        // the captured_now list:
        set_move_ko( captured_now[0][0], captured_now[0][1] );
    }

    // Add move to move history:
    push_move();

    return;
}

/**
 * @brief       Checks if given color string is valid.
 *
 * Takes a given color string (as taken from the play command) and checks if
 * its a valid color. The internal color (BLACK|WHITE) is written into color.
 * A true|false value is returned.
 *
 * @param[in]   color_str   The string representation of the color.
 * @param[out]  *color      When the color is valid gets BLACK or WHITE, NULL
 *                          otherwise.   
 * @return      true|false
 */
bool is_color_valid( char color_str[], int *color )
{
    bool is_valid = false;

    str_toupper(color_str);
    if ( strcmp( color_str, "B" ) == 0 || strcmp( color_str, "BLACK" ) == 0 ) {
        *color = BLACK;
        is_valid = true;
    }
    else if ( strcmp( color_str, "W" ) == 0 || strcmp( color_str, "WHITE" ) == 0 ) {
        *color = WHITE;
        is_valid = true;
    }
    else {
        color = NULL;
    }

    return is_valid;
}

/**
 * @brief       Checks if a given vertex is valid.
 *
 * Takes a given vertex string, checks if its valid and returns true or false.
 * If the vertex is valid the coordinates are written into the pointers *i and
 * *j.
 *
 * @param[in]   vertex_str  The given vertex as string.
 * @param[out]  *i          Pointer where the horizontal coordinate is written to.
 * @param[out]  *j          Pointer where the vertical coordinate is written to.
 * @return      true|false
 * @note        'I' is not a valid coordinate.
 */
bool is_vertex_valid( char vertex_str[], int *i, int *j )
{
    int x, y;
    bool is_valid = false;

    // Check vertex if first coordinate is valid:
    x = (int) toupper( vertex_str[0] ) - 65;
    if ( x == 8 ) {
        return false;
    }
    if ( x > 8 ) {
        x--;
    }
    if ( x < 0 || x >= get_board_size() ) {
        return false;
    }

    // Check if second coordinate is valid:
    vertex_str[0] = ' ';
    y = (int) atoi( vertex_str );
    y--;
    if ( y < 0 || y >= get_board_size() ) {
        return false;
    }

    is_valid = true;
    *i = x;
    *j = y;

    return is_valid;
}

/**
 * @brief       Checks if a given vertex is a PASS move.
 *
 * Checks if a given vertex string represents a PASS move.
 *
 * @param[in]   vertex_str  The given vertex string.
 * @return      true|false
 */
bool is_vertex_pass( char vertex_str[] )
{
    bool is_pass = false;

    str_toupper(vertex_str);
    if ( strcmp( vertex_str, "PASS" ) == 0 ) {
        is_pass = true;
    }

    return is_pass;
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

/**
 * @brief       Generates a move for the given color.
 *
 * Generates and performs a move for the given color.
 *
 * @param[in]   gtp_argc    Number of arguments of GTP command
 * @param[in]   gtp_argv    Array of all arguments for GTP command
 * @return      Nothing
 * @sa          Go Text Protokol version 2, 6.3.3 Core Play Commands
 * @todo        Must also update captured stones.
 * @todo        Move must be added to move history.
 * @todo        Must be able to output "pass" or "resign"!
 *
 * @ingroup GTP_Core_Play_Commands
 */
void gtp_genmove( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int k;
    int color;
    int nr_of_valid_moves;
    int valid_moves[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int i, j;
    int nr_of_removed_stones;
    int captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int group_nr;
    int nr_of_liberties;
    int group_size;

    char x[2];
    char y[3];
    char vertex[4] = "\0";

    // TEST:
    int move_value;

    // Initialise captured stones list:
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        captured_now[k][0] = INVALID;
        captured_now[k][1] = INVALID;
    }

    // Initialise valid_moves:
    for ( k = 0; k < BOARD_SIZE_MAX * BOARD_SIZE_MAX; k++ ) {
        valid_moves[k][0] = INVALID;
        valid_moves[k][1] = INVALID;
    }

    // Check if color is valid:
    if ( ! is_color_valid( gtp_argv[0], &color ) ) {
        set_output_error();
        add_output("invalid color");
        return;
    }

    // Get list of pseudo valid moves:
    nr_of_valid_moves = get_pseudo_valid_move_list( color, valid_moves );
    // Remove zero liberty moves from pseudo valid moves:
    nr_of_valid_moves = get_valid_move_list( color, nr_of_valid_moves, valid_moves );

    // No valid move possible:
    if ( nr_of_valid_moves == 0 ) {
        create_next_move();
        set_move_pass(color);
        push_move();

        add_output("pass");

        return;
    }

    // Use brain_random as placeholder for now:
    move_value = brain_random( &i, &j, valid_moves, nr_of_valid_moves );

    set_vertex( color, i, j );
    create_groups();
    count_liberties();
    set_groups_size();
    nr_of_removed_stones = remove_stones( color * -1 );
    if ( nr_of_removed_stones > 0 ) {
        create_groups();
        count_liberties();
        set_groups_size();
    }

    nr_of_removed_stones = get_captured_now(captured_now);

    create_next_move();
    set_move_vertex( color, i, j );
    set_move_captured_stones(captured_now);

    group_nr        = get_group_nr( i, j );
    nr_of_liberties = get_nr_of_liberties(group_nr);
    group_size      = get_size_of_group(group_nr);

    // Check if this is a ko:
    if ( nr_of_removed_stones == 1 && group_size == 1 && nr_of_liberties == 1 ) {
        // If only one stone has been captured, it must be the first one in
        // the captured_now list:
        set_move_ko( captured_now[0][0], captured_now[0][1] );
    }
    push_move();

    // Create vertex for output:
    if ( i >= 8 ) {
        i++;
    }
    i += 65;
    x[0] = (char) i;
    x[1] = '\0';

    j++;
    y[0] = (char)(int)( j / 10 + 48 );
    y[1] = (char)( j % 10 + 48 );
    y[2] = '\0';
    if ( y[0] == '0' ) {
        y[0] = y[1];
        y[1] = '\0';
    }

    strcat( vertex, x );
    strcat( vertex, y );
    add_output(vertex);

    return;
}

/**
 * @brief       Loads an SGF file
 *
 * Loads a given SGF file
 *
 * @param[in]   gtp_argc    Number of arguments of GTP command
 * @param[in]   gtp_argv    Array of all arguments for GTP command
 * @return      Nothing
 * @sa          Go Text Protokol version 2, 6.3.5 Regression Commands
 */
void gtp_loadsgf( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    char filename[MAX_FILENAME_LENGTH];
    int  move_number = INVALID;

    FILE   *sgf_file;
    struct stat file_attr;
    int    file_size;

    int  c;
    char *file_content;
    int  k = 0;
    //int  m = 0;

    struct node_st     *sgf_tree;
    struct property_st *property;
    //char *value;
    int property_count;
    int value_count;
    bool is_sgf_ok = false;

    //int l;    // DEBUG

    // Check for missing arguments:
    if ( gtp_argc < 1 ) {
        set_output_error();
        add_output("missing argument: filename");

        return;
    }

    my_strcpy( filename, gtp_argv[0], MAX_FILENAME_LENGTH );

    // Check for optional argument move_number:
    if ( gtp_argc >= 2 ) {
        move_number = atoi( gtp_argv[1] );
        if ( move_number <= 0 ) {
            move_number = INVALID;
        }
    }

    // Get file size:
    stat( filename, &file_attr );
    file_size = file_attr.st_size;

    if ( file_size == 0 ) {
        set_output_error();
        add_output("cannot load file");

        return;
    }

    file_content = malloc( ( sizeof(char) * file_size ) + 1 );

    // Read SGF file:
    sgf_file = fopen( filename, "r" );
    if ( sgf_file == NULL ) {
        set_output_error();
        add_output("cannot load file");

        return;
    }
    while ( ( c = fgetc(sgf_file) ) != EOF ) {
        file_content[k] = c;
        k++;
    }
    fclose(sgf_file);
    file_content[k] = '\0';

    sgf_tree = parse_sgf(file_content);

    // DEBUG:
    /*
    while ( sgf_tree->number != -1 ) {
        printf( "# Node Nr.: %d\n", sgf_tree->number );
        printf( "#    Main:    %d\n", sgf_tree->is_main );
        printf( "#    TLevel:  %d\n", sgf_tree->tree_level );
        printf( "#    TNr:     %d\n", sgf_tree->tree_nr );
        printf( "#    Par.:    %d\n", sgf_tree->parent->number );
        printf( "#    Prop.:   %d\n", sgf_tree->property_count );
        for ( k = 0; k < sgf_tree->property_count; k++ ) {
            printf( "####    PName:  %s\n", (sgf_tree->property + k)->name );
            printf( "####    PNr.:   %d\n", (sgf_tree->property + k)->number );
            printf( "####    ValNr.: %d\n", (sgf_tree->property + k)->value_count );
            for ( l = 0; l < (sgf_tree->property + k)->value_count; l++ ) {
                printf( "########    Val:    %s\n", *((sgf_tree->property + k)->value + l) );
            }
        }

        sgf_tree++;
    }
    */

    while ( sgf_tree->is_main && sgf_tree->number != -1 ) {
        property_count = sgf_tree->property_count;
        for ( k = 0; k < property_count; k++ ) {
            property    = sgf_tree->property + k;
            value_count = property->value_count;

            // Properties for captured stones missing, ko, handicap, maybe others too ...
            if ( strcmp( property->name, "FF" ) == 0 ) {
                if ( strcmp( *(property->value), "4" ) == 0 ) {
                    is_sgf_ok = true;
                }
            }
            else if ( strcmp( property->name, "SZ" ) == 0 ) {
                is_sgf_ok = sgf_size( *(property->value) );
            }
            else if ( strcmp( property->name, "KO" ) == 0 ) {
                is_sgf_ok = sgf_komi( *(property->value) );
            }
            else if ( strcmp( property->name, "AB" ) == 0 ) {
                is_sgf_ok = sgf_add( BLACK, property->value, value_count );
            }
            else if ( strcmp( property->name, "AW" ) == 0 ) {
                is_sgf_ok = sgf_add( WHITE, property->value, value_count );
            }
            else if ( strcmp( property->name, "B" ) == 0 ) {
                is_sgf_ok = sgf_move( BLACK, *(property->value) );
            }
            else if ( strcmp( property->name, "W" ) == 0 ) {
                is_sgf_ok = sgf_move( WHITE, *(property->value) );
            }
            else {
                continue;
            }


            if ( ! is_sgf_ok ) {
                break;
            }

        }
        if ( ! is_sgf_ok ) {
            break;
        }

        if ( sgf_tree->number == move_number ) {
            break;
        }

        sgf_tree++;
    }

    free(file_content);

    if ( ! is_sgf_ok ) {
        set_output_error();
        add_output("cannot load file");
    }

    return;
}

/**
 * @brief       Sets the board size.
 *
 * Sets the size of the board when an SGF property of SZ is found.
 *
 * @param[in]   value   The value of the SZ property
 * @return      true|false
 * @note        Returns false if board size is invalid.
 */
bool sgf_size( char *value )
{
    int board_size;

    board_size = atoi(value);

    if ( board_size < BOARD_SIZE_MIN || board_size > BOARD_SIZE_MAX ) {
        return false;
    }

    init_board(board_size);

    return true;
}

/**
 * @brief       Adds a stone to the board.
 *
 * Adds a stone of the given color to the board when an SGF property of AB or
 * AW is found.
 *
 * @param[in]   color   Color of the stone to set.
 * @param[in]   value   Pointer to list of verteces (as strings).
 * @param[in]   value_count   Number of verteces to set.
 * @return      true|false
 */
bool sgf_add( int color, char **value, int value_count )
{
    int k;
    char *v;
    int i, j;

    int board_size = get_board_size();

    for ( k = 0; k < value_count; k++ ) {
        v = *(value + k);

        if ( strlen(v) > 2 ) {
            return false;
        }

        i = v[0] - 97;
        j = board_size - ( v[1] - 97 ) - 1;

        if ( i < 0 || i > board_size ) {
            return false;
        }
        if ( j < 0 || j > board_size ) {
            return false;
        }
        
        set_vertex( color, i, j );

    }

    return true;
}

/**
 * @brief       Performs a move.
 *
 * Performs a move when an B or W property is found in an SGF file.
 *
 * @param[in]   color   Color of side to move.
 * @param[out]  value   The SGF property value of the move to perform.
 * @return      true|false
 * @note        If the given property value does not describe a vertex the
 *              function returns false.
 */
bool sgf_move( int color, char *value )
{
    int i, j;
    int nr_of_removed_stones;
    int group_nr;
    int nr_of_liberties;
    int group_size;
    int captured_now[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];

    int board_size = get_board_size();

    // Check if this is a PASS:
    if ( strlen(value) == 0 ) {
        create_next_move();
        set_move_pass(color);
        push_move();

        return true;
    }

    if ( strlen(value) > 2 ) {
        return false;
    }

    i = value[0] - 97;
    j = board_size - ( value[1] - 97 ) - 1;

    if ( i < 0 || i > board_size ) {
        return false;
    }
    if ( j < 0 || j > board_size ) {
        return false;
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
        //set_output_error();
        //add_output("illegal move");
        return false;
    }

    nr_of_removed_stones = get_captured_now(captured_now);

    create_next_move();
    set_move_vertex( color, i, j );
    set_move_captured_stones(captured_now);

    // Check if this is a ko:
    if ( nr_of_removed_stones == 1 && group_size == 1 && nr_of_liberties == 1 ) {
        // If only one stone has been captured, it must be the first one in
        // the captured_now list:
        set_move_ko( captured_now[0][0], captured_now[0][1] );
    }

    // Add move to move history:
    push_move();


    return true;
}

/**
 * @brief       Sets komi.
 *
 * Sets the komi value when an KO property is found in an SGF file.
 *
 * @param[in]   value   Pointer to komi value as string.
 * @return      true
 */
bool sgf_komi( char *value )
{

    komi = atof(value);

    return true;
}

/**
 * @brief       Takes back move.
 *
 * The GTP undo command takes back the last move. It restores the board, the
 * number of captured stones and removes the last move from the move history.
 *
 * @param[in]   gtp_argc    Number of arguments of GTP command
 * @param[in]   gtp_argv    Array of all arguments for GTP command
 * @return      Nothing
 */
static void gtp_undo( int gtp_argc, char gtp_argv[][MAX_TOKEN_LENGTH] )
{
    int  move_number;
    int  color;
    int  i, j;
    bool is_pass;
    int  count_stones;
    int  stones[BOARD_SIZE_MAX * BOARD_SIZE_MAX][2];
    int  k;

    move_number = get_move_number();

    if ( move_number <= 0 ) {
        set_output_error();
        add_output("cannot undo");

        return;
    }

    color        = get_last_move_color();
    i            = get_last_move_i();
    j            = get_last_move_j();
    is_pass      = get_last_move_pass();
    count_stones = get_last_move_count_stones();

    set_vertex( EMPTY, i, j );

    if ( count_stones > 0 ) {
        get_last_move_stones(stones);
        for ( k = 0; k < count_stones; k++ ) {
            set_vertex( color * -1, stones[k][0], stones[k][1] );
        }

        if ( color == BLACK ) {
            set_black_captured( get_black_captured() - count_stones );
        }
        else {
            set_white_captured( get_white_captured() - count_stones );
        }
    }

    pop_move();

    return;
}

