#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "global_tools.h"

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
    if ( (int) strlen(source) <= destination_length ) {
        strcpy( destination, source );
    }
    else {
        fprintf( stderr, "Cannot copy destination string into source string\n");
        exit(1);
    }

    return;
}
