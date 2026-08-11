#include "parser.h"

char *trim_whitespace(char *buffer) {
    char *p = buffer;
    while (*p == ' ' || *p == '\n' || *p == '\t') {
        p++;
    }

    return p;
}

bool is_whitespace(char *buffer) {
    char c = *buffer;
    return c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

char *parse_fixnum(Object *obj, char *p) {
    fixnum expr = 0;
    int is_negative = 0;

    if (*p == '-') {
        is_negative = 1;
        p++;
    }

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

    return p;
}

char *parse_boolean(Object *obj, char *p) {
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

char *parse_symbol(Object **obj, char *p, Object **pool) {
    symbol expr = NULL;

    size_t length = 0;
    while (!is_whitespace(p) && *p != ')') { // TODO: change
        length++;
        p++;
    }

    expr = (symbol)malloc(length);
    memcpy(expr, p - length, length); // because we ++ at the end of the while loop too

    // check if symbol is already registered to return interned object
    Object *interned = pool_search(pool, expr);

    if (interned != NULL) {
        *obj = interned; // assign interned symbol object
        return p;
    }

    // TODO: intern nil to a singleton instance
    if (length == 3 && strcmp(expr, NIL) == 0) { // special case: nil
        (*obj)->value.nil = NULL;
        (*obj)->type = OBJECT_NIL;
    } else {
        // add to global intern pool
        Object *pool_result = pool_put(pool, expr);
        *obj = pool_result;
        // TODO: check if success or not, error handling
    }

    return p;
}

char *parse_pair(Object *obj, char *p, Object **pool) {
    p++;
    if (*p == ')') {
        obj->type = OBJECT_NIL;
        obj->value.nil = NULL;
        return ++p;
    }

    obj->type = OBJECT_PAIR;

    Object *car = (Object *)calloc(1, sizeof(Object));
    p = parse_sexpression(&car, p, pool);

    obj->value.pair[0] = car;
    if (*p == ')') {
        Object *cdr = (Object *)calloc(1, sizeof(Object));
        cdr->type = OBJECT_NIL;
        cdr->value.nil = NULL;

        obj->value.pair[1] = cdr;
        p++;
    } else { // guaranteed to be technically of type PAIR
        Object *cdr = (Object *)calloc(1, sizeof(Object));
        p = parse_pair(cdr, p, pool);

        obj->value.pair[1] = cdr;
    }

    return p;
}

// TODO: size is actually unused right now
char *parse_sexpression(Object **obj, char *buffer, Object **pool) {
    char *p = trim_whitespace(buffer);

    if (*p == '#') {
        p = parse_boolean(*obj, p);
    } else if ((*p == '-' && !is_whitespace(p + 1)) || (*p >= '0' && *p <= '9')) {
        p = parse_fixnum(*obj, p);
    } else if (*p == '(') { // TODO: handle explicit pair construction
        p = parse_pair(*obj, p, pool);
    } else if (*p == '\'') { // TODO: handle explicit pair construction
        p++;                 // increment to get rid of '
        p = parse_sexpression(obj, p, pool);
        Object *quoted = (Object *)calloc(1, sizeof(Object));
        quoted->type = OBJECT_QUOTE;
        quoted->value.quote = *obj;
        *obj = quoted;
    } else {
        p = parse_symbol(obj, p, pool); // we will have nil as a special case in parsing a symbol
    }

    return p;
}