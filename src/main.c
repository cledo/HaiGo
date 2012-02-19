#include <stdlib.h>
#include "run_program.h"

/**
 *  @mainpage HaiGo v.0.1
 *
 *  This is just a placeholder text.
 *  
 *  @author     Clemens Dorner
 *  @version    0.1
 *  @date       2012-01-22
 *
 */

/**
 * @file    main.c
 *
 * @brief   The file main.c only contains the main() function.
 *
 * The file main.c only contains the main() function. Besides the code it also
 * contains the doxygen documentation for the main page.
 */


/**
 *  @brief The main() function is only a wrapper for the run() function.
 * 
 *  The main() function only calls run(). This is because main() itself cannot
 *  be unit-tested with check. Therefore the real work is done by run().
 *  
 *  @param[in]  argc  Number of command line arguments
 *  @param[in]  argv  Array of all command line arguments
 *  @return     EXIT_SUCCESS | EXIT_FAILURE
 *  @sa         info check
 */
int main( int argc, char **argv )
{
    int exit_value = EXIT_FAILURE;

    exit_value = run( argc, argv );

    return exit_value;
}

