FORMAT_FILES := main.c repl.c repl.h test.c

.PHONY: format format-check

main: repl.h repl.c main.c
	gcc -o main.out main.c repl.c

debug: repl.h repl.c main.c
	gcc -g -o debug.out main.c repl.c

test: repl.h repl.c test.c
	gcc -o test.out test.c repl.c

format:
	clang-format -i $(FORMAT_FILES)

format-check:
	clang-format --dry-run --Werror $(FORMAT_FILES)
