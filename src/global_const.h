#ifndef GLOBAL_CONST_H
#define GLOBAL_CONST_H

/**
 * @file    global_const.h
 *
 * @brief   Definition of global constants
 *
 */

//! Defines the name of the program.
#define PROGRAM_NAME    "HaiGo"
//! Defines the current version of the program.
#define PROGRAM_VERSION "0.1"
//! Defines the valid command line options.
#define VALID_OPTIONS   "hv"

//! Defines invalid entry.
#define INVALID -1

//! Defines the implemented version of the Go Text Protocol.
#define GTP_VERSION     "2"

//! Defines the maximum length of any given GTP token, either command or argument.
#define MAX_TOKEN_LENGTH 20
//! Defines the maximum number of given GTP tokens for any command plus arguments.
#define MAX_TOKEN_COUNT  10

//! Sets the length of the GTP output buffer.
#define MAX_OUTPUT_LENGTH 2048

//! Constant for an empty field.
#define EMPTY   0
//! Constant for black stone.
#define BLACK   1
//! Constant for white stone.
#define WHITE  -1

//! Defines the character which is shown for an empty field.
#define FIELD_EMPTY "."
//! Defines the character which is shown for a star field.
#define FIELD_HOSHI "+"
//! Defines the character which is shown for a white stone.
#define WHITE_STONE "0"
//! Defines the character which is shown for a black stone.
#define BLACK_STONE "X"

//! Defines the minimum board size which is accepted.
#define BOARD_SIZE_MIN   2
//! Defines the maximum board size which is accepted.
#define BOARD_SIZE_MAX  25
//! Defines the default board size.
#define BOARD_SIZE_DEFAULT  19

//! Defines the maximum number of moves.
#define MOVE_HISTORY_MAX    1024

//! Defines the maximum length of a file name.
#define MAX_FILENAME_LENGTH 256

//! Defines default level of depth for serch tree.
#define DEFAULT_SEARCH_LEVEL    1

//! Defines name of log file for search tree.
#define LOG_FILE    "haigo.log"

//! Defines size of transposition hash table.
//#define HASH_TABLE_SIZE 1048576     // 2 ^ 20
#define HASH_TABLE_SIZE 33554432  // 2 ^ 25

//! Defines maximal quiescence search depth:
#define MAX_QSEARCH_DEPTH   6

#endif

