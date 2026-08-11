FORMAT_FILES := main.c repl.c ast.c parser.c object_helpers.c

CC = gcc
CFLAGS = -Wall -Wextra
TARGET = main
SRCS = repl.c ast.c parser.c object_helpers.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): main.o $(OBJS)
	$(CC) $(CFLAGS) -o main.out main.o $(OBJS)

debug: main.o $(OBJS)
	$(CC) $(CFLAGS) -g -o debug.out main.o $(OBJS)

test: test.o $(OBJS)
	$(CC) $(CFLAGS) -o test.out test.o $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: format
format:
	clang-format -i $(FORMAT_FILES)

.PHONY: format-check
format-check:
	clang-format --dry-run --Werror $(FORMAT_FILES)

.PHONY: clean
clean:
	rm -f *.o main.out debug.out test.out
