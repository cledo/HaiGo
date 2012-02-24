#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "global_tools.h"

/**
 * @file    global_tools.c
 *
 * @brief   Defines globally available helper functions.
 *
 */

/**
 *  @brief Converts a given string into upper case.
 *
 *  Converts a given string into upper case. The string is converted inplace.
 *  No copying is being done.
 *
 *  @param[in]  string  a given string
 *  @return     nothing
 *  @sa         [n/a]
 */
void str_toupper( char string[] )
{
    int i;
    int str_length = (int) strlen(string);

    for ( i = 0; i < str_length; i++ ) {
        string[i] = toupper( string[i] );
    }

    return;
}

/**
 *  @brief Safe form of string copy
 *
 *  my_strcpy() copies one string into another but does a size check before
 *  that.
 *
 *  @param[out] destination         string copied to
 *  @param[in]  source              string copied from
 *  @param[in]  destination_length  length of destination buffer
 *  @return     nothing
 *  @sa         Discussion of strncpy: @link1
 *  @sa         You cannot use sizeof here. See: <a href=@link2>CERT</a>
 */
void my_strcpy( char destination[], char source[], int destination_length )
{
    int source_length = (int) strlen(source);

    if ( source_length <= destination_length ) {
        strcpy( destination, source );
        //strncpy( destination, source, source_length );
    }
    else {
        fprintf( stderr, "Cannot copy destination string into source string\n");
        exit(1);
    }

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

