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

char* parse_fixnum(Object *obj, char *p) {
    fixnum expr = 0;
    int is_negative = 0;

    if (*p == '-') {
        is_negative = 1;
        p++;
    }

    while (*p != '\n') {
        if (!(*p >= '0' && *p <= '9')) {
            perror("Expected digit\n");
            exit(1);
        }
        expr *= 10;
        expr += *p - '0';
        p++;
    }

    if (is_negative) {
        expr *= -1;
    }

    obj->value.fixnum = expr;
    obj->type = OBJECT_FIXNUM;

    return p;
}

char* parse_boolean(Object *obj, char *p) {
    p++;
    boolean expr = false;

    if (*p == 't') {
        expr = true;
    } else if (*p != 'f') {
        perror("Expected #t or #f\n");
        exit(1);
    }

    obj->value.boolean = expr;
    obj->type = OBJECT_BOOLEAN;

    return p++;
}

// TODO: size is actually unused right now
Object parse_sexpression(char *buffer) {
    char *p = trim_whitespace(buffer);
    Object obj;

    if (*p == '#') {
        p = parse_boolean(&obj, p);
    } else if (*p == '-' || (*p >= '0' && *p <= '9')) {
        p = parse_fixnum(&obj, p);
    }

    return obj;
}

int eval(char *buffer) {
    // TODO: need AST parser???
    Object obj = parse_sexpression(buffer);
    
    switch (obj.type) {
    case OBJECT_FIXNUM:
        printf("%lld\n", obj.value.fixnum);
        break;
    case OBJECT_BOOLEAN:
        printf("#%c\n", obj.value.boolean ? 't' : 'f');
        break;
    }

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
        
        int error = eval(buffer);
        if (error) {
            return error;
        }
        // Make sure to free heap memory allocated for buffer
        free(buffer);
        buffer = NULL;
    }
    return 0;
}