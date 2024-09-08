CC := gcc
OS=$(shell uname -s)
CFLAGS := -I inc -std=c99 -Wall -Werror -Wextra -Wpedantic
CLANG = clang-format

LDFLAGS := -lncurses
OBJS_FSMSWITCHCASE := out/tetris.o out/fsm.o out/backend.o out/frontend.o

GCOV = -fprofile-arcs -ftest-coverage

TESTS = tests/*.c

ifeq ($(OS),Darwin)
  	OPEN = open
	LC = -lcheck
endif
ifeq ($(OS),Linux)
	OPEN = xdg-open
	LC = -lcheck -lsubunit -lrt -lpthread -lm -D_GNU_SOURCE
endif

all: install

install: out $(OBJS_FSMSWITCHCASE)

	$(CC) $(CFLAGS) $(OBJS_FSMSWITCHCASE) -o tetris $(LDFLAGS)

uninstall:

	rm -rf tetris

dvi: 
	cat Doxyfile

dist: install
	tar -cf tetris.tar tetris


out:
	mkdir -p out

out/%.o: brick_game/tetris/%.c
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: gui/cli/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf out/*.o
	rm -rf tetris
	rm -rf tetris_test
	rm -rf *.o *.a *.gcda *.gcno 
	rm -rf *.info *.gcov *.html 
	rm -rf *.dSYM *.info *.log
	rm -rf test report .gitignore .gitkeep .check

style:
	$(CLANG) -i */*/*.c */*/*.h
	$(CLANG) -i */*.c 
	$(CLANG) -n */*/*.c */*/*.h
	$(CLANG) -n */*.c 

gcov_report: clean
	$(CC) $(GCOV)  brick_game/tetris/backend.c $(TESTS) -o test -lcheck -lm
	./test
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info
	$(OPEN) ./report/index.html

test:
	gcc -o tetris_test -fprofile-arcs -ftest-coverage brick_game/tetris/backend.c tests/*.c -lcheck -lm
	./tetris_test
