SUBDIRS = src tests

cover: FORCE
	@mkdir -p cover
	@./configure CFLAGS="--coverage -g -O0"
	@prove
	@lcov -d src -d tests -c -o cover/haigo.info
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

