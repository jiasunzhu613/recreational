/* 
Read, Evaluate, Print Loop for interpreting LISP

Recreational programming session referencing:
- https://groups.csail.mit.edu/mac/classes/6.001/abelson-sussman-lectures/
*/

#include "repl.h"

/*
Evaluate part of REPL loop

3 => 3

(+ 3.6 3)

AST representation:
  +
/  \
3  3
*/
char* trim_whitespace(char *buffer) {
    char *p = buffer;
    while (*p == ' ' || *p == '\n' || *p == '\t') { p++; }
    
    return p;
}

// TODO: size is actually unused right now
fixnum parse_expression(char *buffer, size_t size) {
    char *p = trim_whitespace(buffer);
    fixnum expr = 0;

    while (*p != '\n') {
        if (!(*p >= '0' && *p <= '9')) {
            perror("Expected digit\n");
            exit(1);
        }

        expr *= 10;
        expr += *p - '0';
        p++;
    }

    return expr;
}

int eval(char *buffer, size_t size) {
    // TODO: need AST parser???
    fixnum expr = parse_expression(buffer, size);
    printf("%lld\n", expr);

    return 0;
}

int main() {
    char *buffer = NULL;
    size_t size = 0;

    while (1) {
        printf(">>> "); // Print prompt

        ssize_t num_chars = getline(&buffer, &size, stdin);

        // Exit on ^D or EOF
        if (num_chars == -1) {
            printf("\n");
            return 0;
        }
        
        int error = eval(buffer, size);
        if (error) {
            return error;
        }
        // Make sure to free heap memory allocated for buffer
        free(buffer);
        buffer = NULL;
    }
    return 0;
}