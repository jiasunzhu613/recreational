FORMAT_FILES := main.c repl.c ast.c parser.c object_helpers.c

CC = gcc
CFLAGS = -Wall -Wextra
TARGET = main
SRCS = repl.c ast.c parser.c object_helpers.c
OBJS = $(SRCS:.c=.o)

.DEFAULT_GOAL := all

.PHONY: help
help: ## Show this help message
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

all: $(TARGET) ## Build main.out (default)

$(TARGET): main.o $(OBJS)
	$(CC) $(CFLAGS) -o main.out main.o $(OBJS)

debug: main.o $(OBJS) ## Build debug.out with debug symbols
	$(CC) $(CFLAGS) -g -o debug.out main.o $(OBJS)

test: test.o $(OBJS) ## Build test.out
	$(CC) $(CFLAGS) -o test.out test.o $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: format
format: ## Format the C source files
	clang-format -i $(FORMAT_FILES)

.PHONY: format-check
format-check: ## Check source formatting without changing files
	clang-format --dry-run --Werror $(FORMAT_FILES)

.PHONY: clean
clean: ## Remove generated object files and executables
	rm -f *.o main.out debug.out test.out
