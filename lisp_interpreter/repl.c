/* 
Read, Evaluate, Print Loop for interpreting LISP

Recreational programming session referencing:
- https://groups.csail.mit.edu/mac/classes/6.001/abelson-sussman-lectures/
*/

#include "repl.h"


char* trim_whitespace(char *buffer) {
    char *p = buffer;
    while (*p == ' ' || *p == '\n' || *p == '\t') { p++; }
    
    return p;
}

bool is_whitespace(char *buffer) {
    char c = *buffer;
    return c == ' ' || c == '\n' || c == '\t';
}

char* parse_fixnum(Object *obj, char *p) {
    fixnum expr = 0;
    int is_negative = 0;

    if (*p == '-') {
        is_negative = 1;
        p++;
    }

    // printf("DEBUG: remaining => %s\n", p);
    while (*p >= '0' && *p <= '9') {
        expr *= 10;
        expr += *p - '0';
        p++;
    }

    if (is_negative) {
        expr *= -1;
    }

    obj->value.fixnum = expr;
    obj->type = OBJECT_FIXNUM;

    // printf("DEBUG: remaining => %s\n", p);
    return p;
}

char* parse_boolean(Object *obj, char *p) {
    p++; // increment for # delimiter
    boolean expr = false;

    if (*p == 't') {
        expr = true;
    } else if (*p != 'f') {
        fprintf(stderr, "Expected #t or #f\n");
        exit(1);
    }

    obj->value.boolean = expr;
    obj->type = OBJECT_BOOLEAN;
    return ++p;
}

char* parse_symbol(Object *obj, char *p) {
    p++; // increment for ' delimiter
    symbol expr = NULL;

    size_t length = 0;
    while (!is_whitespace(p) && *p != ')') {
        length++;
        p++;
    }

    // TODO: if we need this, we need to figure out how to deallocate this memory
    // maybe runtime GC is enough?
    expr = (symbol) malloc(length);
    memcpy(expr, p - length, length); // because we ++ at the end of the while loop too
    
    obj->value.symbol = expr;
    obj->type = OBJECT_SYMBOL;

    return p;
}

char* parse_nil(Object *obj, char *p) {
    p++;
    if (*p++ != 'i') {
        fprintf(stderr, "Expected: nil\n");
        exit(1);
    }

    if (*p++ != 'l') {
        fprintf(stderr, "Expected: nil\n");
        exit(1);
    }

    obj->value.nil = NULL;
    obj->type = OBJECT_NIL;

    return p;
}

char* parse_pair(Object *obj, char *p) {
    p++;

    obj->type = OBJECT_PAIR;

    Object *car = (Object *) calloc(1, sizeof(Object));
    p = parse_sexpression(car, p);
    
    obj->value.pair[0] = car;
    if (*p == ')') {
        Object *cdr = (Object *) calloc(1, sizeof(Object));
        cdr->type = OBJECT_NIL;
        cdr->value.nil = NULL;

        obj->value.pair[1] = cdr;
    } else { // guaranteed to be technically of type PAIR
        Object *cdr = (Object *) calloc(1, sizeof(Object));
        p = parse_pair(cdr, p);

        obj->value.pair[1] = cdr;
    }

    return p;
}

// TODO: size is actually unused right now
char* parse_sexpression(Object *obj, char *buffer) {
    char *p = trim_whitespace(buffer);

    if (*p == '#') {
        p = parse_boolean(obj, p);
    } else if (*p == '-' || (*p >= '0' && *p <= '9')) {
        p = parse_fixnum(obj, p);
    } else if (*p == '\'') {
        p = parse_symbol(obj, p);
    } else if (*p == 'n') {
        p = parse_nil(obj, p);
    } else if (*p == '(') { // TODO: handle explicit pair construction
        p = parse_pair(obj, p);
    } else {
        fprintf(stderr, "Got unknown symbols: %s", buffer);
        exit(1);
    }

    return p;
}

void print_sexpression(Object obj) {
    switch (obj.type) {
    case OBJECT_FIXNUM:
        printf("%lld", obj.value.fixnum);
        break;
    case OBJECT_BOOLEAN:
        printf("#%c", obj.value.boolean ? 't' : 'f');
        break;
    case OBJECT_SYMBOL:
        printf("%s", obj.value.symbol);
        break;
    case OBJECT_NIL:
        printf("nil");
        break;
    case OBJECT_PAIR:
        printf("(");
        // printf("DEBUG: mem address of car during print: %p\n", obj.value.pair[0]);
        print_sexpression(*(obj.value.pair[0]));
        printf(" . ");
        print_sexpression(*(obj.value.pair[1]));
        printf(")");
        break;
    }

}

int eval(char *buffer) {
    // TODO: need AST parser???
    Object obj = {};

    buffer = parse_sexpression(&obj, buffer);
    
    print_sexpression(obj);
    printf("\n");

    return 0;
}

// TODO: right now there are for sure memory leaks because we dont free mallocs for list and symbol 
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