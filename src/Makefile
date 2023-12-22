.PHONY: all install open uninstall dist test gcov clean leak div

GCC = gcc -Wall -Werror -Wextra -std=c++17 
TEST_FLAGS = -lgtest -lgtest_main -lstdc++ -lm -g 
TEST = $(GCC) tests/test.cc models/parse.cc $(TEST_FLAGS) -o s21_test
GCOVFLAGS = -fprofile-arcs -ftest-coverage

OS = $(shell uname)

ifeq ($(OS), Darwin)
	TEST_LIBS = -lcheck -lgcov
	MOVE = mkdir build && mv view/3dViewer.app build
	OPEN = cd build && open .
	LEAK = leaks -atExit -- ./s21_test 
	MISSMATCH = --ignore-errors mismatch
else
	TEST_LIBS = -lcheck -lsubunit -pthread -lm -lrt -lgcov
	MOVE = mkdir build && mv view/3dViewer build
	OPEN = build/3dViewer
	LEAK = valgrind --leak-check=full ./s21_test
endif

all: uninstall install

install:
	qmake6 -o view/Makefile 3dViewer.pro
	cd view/ && make 
	$(MOVE)
	rm -rf view
	$(OPEN)

open:
	$(OPEN)

uninstall:
	rm -rf build/

dist:
	mkdir 3dViewer_v2.1/
	cp -a controllers models objs tests views patterns Makefile main.cc readme.txt 3dViewer.pro 3dViewer_v2.1
	tar cvzf 3dViewer_v2.1.tgz 3dViewer_v2.1/
	rm -rf 3dViewer_v2.1/

test: 
	$(TEST)
	./s21_test

gcov: 
	$(GCC) $(GCOVFLAGS) tests/test.cc models/parse.cc $(TEST_FLAGS) -o report.out
	./report.out
	lcov -t "gcov_report" -o gcov_report.info --no-external -c -d . $(MISSMATCH)
	genhtml -o report/ gcov_report.info
	rm -f *.gcno *.gcda *.info report.out *.gcov
	open ./report/index.html

clean: 
	find . \( -name '*.o' -o -name '*.a' -o -name '*.gcno' -o -name '*.gcda' -o -name '*.info' \) -exec rm -rf {} \;
	rm -rf report report_out report.out.dSYM s21_test.dSYM s21_test 3dViewer_v2.0.tgz view 3dViewer.pro.*

clang:
	find . -name '*.cc' -o -name '*.h' | xargs clang-format -n -style=Google

leak: 
	$(TEST)
	$(LEAK)

dvi:
	open readme.txt
