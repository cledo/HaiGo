SUBDIRS = src tests

cover: FORCE
	@mkdir -p cover
	@./configure CFLAGS="--coverage -g -O0"
	@prove
	@lcov -d src -c -o cover/haigo.info
	@genhtml -o cover cover/haigo.info

cover-clean: FORCE
	@rm -f src/*.gcov
	@rm -f src/*.gcda
	@rm -f src/*.gcno
	@rm -f tests/*.gcov
	@rm -f tests/*.gcda
	@rm -f tests/*.gcno
	@lcov -d src/ -d tests/ -z

FORCE:

