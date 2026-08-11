FORMAT_FILES := main.c repl.c repl.h test.c

.PHONY: format format-check

CC := gcc

main: repl.h repl.c main.c ast.c parser.c object_helpers.c
	CC -o main.out main.c repl.c ast.c parser.c object_helpers.c

debug: repl.h repl.c main.c ast.c parser.c
	CC -g -o debug.out main.c repl.c ast.c parser.c

test: repl.h repl.c test.c ast.c parser.c
	CC -o test.out test.c repl.c ast.c parser.c

format:
	clang-format -i $(FORMAT_FILES)

format-check:
	clang-format --dry-run --Werror $(FORMAT_FILES)
