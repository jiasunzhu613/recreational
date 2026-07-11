#ifndef REPL_HEADER
#define REPL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // for int64_t et al.

// AST Structures
// Just a tree with variable number of children
typedef int64_t word;
typedef int64_t fixnum;

// Functions
char* trim_whitespace(char *buffer);
fixnum parse_expression(char *buffer, size_t size);


#endif // REPL_HEADER