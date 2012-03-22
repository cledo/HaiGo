SUBDIRS = src tests new_src

cover: FORCE
	@mkdir -p cover
	@./configure CFLAGS="--coverage -g -O0"

	## ---- Unit Tests ---- ##
	@prove t/00_make_clean.t t/01_make.t t/02_make_check.t
	@lcov -d tests -c -o cover/haigo.info

	## ---- System Tests ---- ##
	## @prove t/00_make_clean.t t/01_make.t t/0[4-9]* t/[1-9]*
	## @lcov -d src -c -o cover/haigo.info

	## ---- Unit and System Tests ---- ##
	## @prove
	## @lcov -d src -d tests -c -o cover/haigo.info

	@genhtml -o cover cover/haigo.info
	@make cover-clean

cover-clean: FORCE
	@lcov -d src/ -d tests/ -z
	@rm -f src/*.gcov
	@rm -f src/*.gcda
	@rm -f src/*.gcno
	@rm -f tests/*.gcov
	@rm -f tests/.*.gcov
	@rm -f tests/*.gcda
	@rm -f tests/*.gcno

doxy: FORCE
	@doxygen Doxyfile

FORCE:

