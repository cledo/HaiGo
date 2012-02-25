AM_CFLAGS = -Wall
TESTS = check_run_program check_io check_board check_move check_global_tools
check_PROGRAMS = check_run_program check_io check_board check_move check_global_tools

check_run_program_SOURCES = check_run_program.c $(top_builddir)/src/run_program.c $(top_builddir)/src/io.c $(top_builddir)/src/board.c $(top_builddir)/src/move.c $(top_builddir)/src/global_tools.c $(top_builddir)/src/brains.c $(top_builddir)/src/sgf.c $(top_builddir)/src/search.c $(top_builddir)/src/evaluate.c
check_run_program_CFLAGS  = $(AM_CFLAGS) @CHECK_CFLAGS@
check_run_program_LDADD   = @CHECK_LIBS@

check_io_SOURCES = check_io.c $(top_builddir)/src/io.c $(top_builddir)/src/global_tools.c
check_io_CFLAGS  = $(AM_CFLAGS) @CHECK_CFLAGS@
check_io_LDADD   = @CHECK_LIBS@

check_board_SOURCES = check_board.c $(top_builddir)/src/board.c $(top_builddir)/src/move.c $(top_builddir)/src/evaluate.c $(top_builddir)/src/brains.c $(top_builddir)/src/search.c $(top_builddir)/src/global_tools.c
check_board_CFLAGS  = $(AM_CFLAGS) @CHECK_CFLAGS@
check_board_LDADD   = @CHECK_LIBS@

check_move_SOURCES = check_move.c $(top_builddir)/src/move.c $(top_builddir)/src/board.c $(top_builddir)/src/evaluate.c $(top_builddir)/src/brains.c $(top_builddir)/src/search.c $(top_builddir)/src/global_tools.c
check_move_CFLAGS  = $(AM_CFLAGS) @CHECK_CFLAGS@
check_move_LDADD   = @CHECK_LIBS@

check_global_tools_SOURCES = check_global_tools.c $(top_builddir)/src/global_tools.c
check_global_tools_CFLAGS  = $(AM_CFLAGS) @CHECK_CFLAGS@
check_global_tools_LDADD   = @CHECK_LIBS@

