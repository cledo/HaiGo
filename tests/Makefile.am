AM_CFLAGS = -Wall
TESTS = check_run_program check_io check_board
check_PROGRAMS = check_run_program check_io check_board

check_run_program_SOURCES = check_run_program.c $(top_builddir)/src/run_program.c $(top_builddir)/src/io.c $(top_builddir)/src/board.c $(top_builddir)/src/global_tools.c
check_run_program_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@
check_run_program_LDADD = @CHECK_LIBS@

check_io_SOURCES = check_io.c $(top_builddir)/src/io.c $(top_builddir)/src/global_tools.c
check_io_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@
check_io_LDADD = @CHECK_LIBS@

check_board_SOURCES = check_board.c $(top_builddir)/src/board.c
check_board_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@
check_board_LDADD = @CHECK_LIBS@

