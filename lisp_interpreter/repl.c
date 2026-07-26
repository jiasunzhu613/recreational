/* 
Read, Evaluate, Print Loop for interpreting LISP

DISCLAIMER: this may not be an exact one to one replication of any specific well-known LISP dialect

Recreational programming session referencing:
- https://groups.csail.mit.edu/mac/classes/6.001/abelson-sussman-lectures/
- https://bernsteinbear.com/blog/lisp/
*/

#include "repl.h"

Function *builtin[] = {
    &(Function){.name="if", .func=builtin_if},
    &(Function){.name="+", .func=builtin_add},
    &(Function){.name="-", .func=builtin_sub_neg},
    &(Function){.name="*", .func=builtin_mul},
    &(Function){.name="quote", .func=builtin_quote}, // TODO: special, maybe dont need to put here
    &(Function){.name="atom", .func=builtin_atom},
    &(Function){.name="cdr", .func=builtin_cdr},
    &(Function){.name="car", .func=builtin_car},
    &(Function){.name="cons", .func=builtin_cons},
    &(Function){.name="cond", .func=builtin_cond}, // TODO: special, maybe dont need to put here
    NULL
};

bool env_put(Object **env, Object *key, Object *value) {
    // always pair up
    Object *entry = (Object*) calloc(1, sizeof(Object));
    entry->type = OBJECT_PAIR;
    entry->value.pair[0] = key;
    entry->value.pair[1] = value;

    Object *new_env = (Object*) calloc(1, sizeof(Object));
    new_env->type = OBJECT_PAIR;
    new_env->value.pair[0] = entry;
    new_env->value.pair[1] = *env;

    print_sexpression(new_env);

    *env = new_env;
    return true;
}

Object* env_search(Object **env, Object *key) {
    // TODO: need some way to compare the value of key which will be a symbol and what is in the env, compare strings, strcmp
    // key should never be a non-symbol
    assert(key->type == OBJECT_SYMBOL); // TODO: maybe mark with better logging instead

    Object *p = *env;

    while (p->type != OBJECT_NIL) {
        Object *entry = p->value.pair[0]; // TODO: might need to validate this, this should be a entry though
        Object *entry_key = entry->value.pair[0]; // get first elem from entry pair
        printf("DEBUG: on entry\n");
        print_sexpression(entry);
        printf("\n");
        if (entry_key->type != OBJECT_SYMBOL) {
            fprintf(stderr, "CRITICAL: environment entry has non-symbol key\n");
            exit(1);
        }

        if (strcmp(entry_key->value.symbol, key->value.symbol) == 0) {
            printf("DEBUG: FOUND MATCHING KEYS\n");
            printf("DEBUG: RETURING ");
            print_sexpression(entry->value.pair[1]);
            printf("\n");
            return entry->value.pair[1];
        }

        p = p->value.pair[1];
    }

    printf("DEBUG: DID NOT FIND SHIT IN ENV_SEARCH\n");
    return NULL; // TODO: decide what to do here?
}

char* trim_whitespace(char *buffer) {
    char *p = buffer;
    while (*p == ' ' || *p == '\n' || *p == '\t') { p++; }
    
    return p;
}

bool is_whitespace(char *buffer) {
    char c = *buffer;
    return c == ' ' || c == '\n' || c == '\t';
}

bool is_object_list(Object *obj) {
    if (obj->type != OBJECT_PAIR) {
        return false;
    }

    assert(obj->value.pair[0] != NULL && obj->value.pair[1] != NULL);
    if (obj->value.pair[0] != NULL && obj->value.pair[1]->type == OBJECT_NIL) {
        return true;
    }

    return is_object_list(obj->value.pair[1]);
}

int list_len(Object *list) {
    int count = 0;
    while (list->value.pair[1]->type != OBJECT_NIL) {
        count++;
        list = list->value.pair[1];
    }

    return count + 1;
}

Object* list_index(Object *list, int ind) {
    while (ind > 0 && list->type != OBJECT_NIL) {
        ind--;
        list = list->value.pair[1];
    }

    if (ind > 0) {
        return NULL;
    }

    return list;
}

Object* list_index_get(Object *list, int ind) {
    return list_index(list, ind)->value.pair[0];
}

char* parse_fixnum(Object *obj, char *p) {
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
    symbol expr = NULL;

    size_t length = 0;
    while (!is_whitespace(p) && *p != ')') { // TODO: change
        length++;
        p++;
    }

    expr = (symbol) malloc(length);
    memcpy(expr, p - length, length); // because we ++ at the end of the while loop too

    if (length == 3 && strcmp(expr, NIL) == 0) { // special case: nil
        obj->value.nil = NULL;
        obj->type = OBJECT_NIL;
    } else {        
        obj->value.symbol = expr;
        obj->type = OBJECT_SYMBOL;
    }

    return p;
}

char* parse_pair(Object *obj, char *p) {
    p++;
    if (*p == ')') {
        obj->type = OBJECT_NIL;
        obj->value.nil = NULL;
        return ++p;
    }

    obj->type = OBJECT_PAIR;

    Object *car = (Object *) calloc(1, sizeof(Object));
    p = parse_sexpression(car, p);
    
    obj->value.pair[0] = car;
    if (*p == ')') {
        Object *cdr = (Object *) calloc(1, sizeof(Object));
        cdr->type = OBJECT_NIL;
        cdr->value.nil = NULL;

        obj->value.pair[1] = cdr;
        p++;
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

    // else if (*p == '\'') {
    //     p = parse_symbol(obj, p);
    // } 
    if (*p == '#') {
        p = parse_boolean(obj, p);
    } else if (*p == '-' || (*p >= '0' && *p <= '9')) {
        p = parse_fixnum(obj, p);
    } else if (*p == '(') { // TODO: handle explicit pair construction
        p = parse_pair(obj, p);
    } else if (*p == '\'') { // TODO: handle explicit pair construction
        p++; // increment to get rid of '
        p = parse_sexpression(obj, p);
        obj->quoted = true;
    } else {
        p = parse_symbol(obj, p); // we will have nil as a special case in parsing a symbol
    }

    return p;
}

void print_list(Object *obj) {
    // Guarantee the object coming in is of list like structure initially
    // Being OBJECT_PAIR is baseline requirement
    assert(obj->type == OBJECT_PAIR);
    
    while (obj->value.pair[1]->type != OBJECT_NIL) {
        print_sexpression(obj->value.pair[0]);
        printf(" ");
        obj = obj->value.pair[1];
    }
    print_sexpression(obj->value.pair[0]);
}

void print_sexpression(Object *obj) {
    // TODO: this should change
    if (obj == NULL) {
        return;
    }

    switch (obj->type) {
    case OBJECT_FIXNUM:
        printf("%lld", obj->value.fixnum);
        break;
    case OBJECT_BOOLEAN:
        printf("#%c", obj->value.boolean ? 't' : 'f');
        break;
    case OBJECT_SYMBOL:
        printf("%s", obj->value.symbol);
        break;
    case OBJECT_NIL:
        printf("nil");
        break;
    case OBJECT_PAIR:
        // Check if pair construction looks like list construction
        if (is_object_list(obj)) {
            printf("(");
            print_list(obj);
            printf(")");
        } else { // Print in pair syntax instead
             printf("(");
            print_sexpression(obj->value.pair[0]);
            printf(" . ");
            print_sexpression(obj->value.pair[1]);
            printf(")");
        }
    
        break;
    }
}

// NOTE: we guarantee that the Object coming in is of list type
// TODO: maybe change this to loop through enums and then return an enum as well?
bool is_built_in(Object *first, char *func_name) {
    if (!(first->type == OBJECT_SYMBOL)) {
        return false;
    }

    return strcmp(first->value.symbol, func_name) == 0;
}

Object* builtin_val(Object *list, Object **env) {
    if (list_len(list) != 2) {
        return NULL; // TODO: decide if this return value is good or not
    }

    Object *env_key = list_index_get(list, 0);
    Object *env_value = eval_sexpression(list_index_get(list, 1), env);

    // Make sure env_key is a symbol
    if (env_key->type != OBJECT_SYMBOL) {
        return NULL;
    }

    bool success = env_put(env, env_key, env_value);
    printf("DEBUG: env_put in builtinval => %d\n", success);
    return NULL;
}

Object* builtin_if(Object *list) {
    if (list_len(list) != 3) {
        return NULL; // TODO: decide if this return value is good or not
    }

    Object *condition = list_index_get(list, 0);
    Object *action_true = list_index_get(list, 1);
    Object *action_false = list_index_get(list, 2);

    if (condition->type != OBJECT_BOOLEAN) {
        return NULL; // TODO: this should emit an error
    }

    if (condition->value.boolean) {
        return action_true;
    } else {
        return action_false;
    }
}

Object* builtin_add(Object *list) {
    if (list_len(list) < 2) {
        return NULL;
    }

    fixnum res = 0;
    Object *elements = list;
    print_sexpression(list);
    while (elements->type == OBJECT_PAIR) {
        Object *element = elements->value.pair[0];
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res += element->value.fixnum;
        elements = elements->value.pair[1];
    }

    Object *result = (Object*)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object* builtin_sub_neg(Object *list) {
    int len = list_len(list);
    if (len < 1) {
        return NULL;
    }

    if (len == 1) {
        Object *elem = list_index_get(list, 0);
        if (elem->type != OBJECT_FIXNUM) {
            return NULL;
        }

        fixnum res = elem->value.fixnum * -1;
        Object *result = (Object*)calloc(1, sizeof(Object));
        result->type = OBJECT_FIXNUM;
        result->value.fixnum = res;

        return result;
    }

    Object *elem = list_index_get(list, 0);
    if (elem->type != OBJECT_FIXNUM) {
        return NULL;
    }
    fixnum res = elem->value.fixnum;
    
    Object *elements = list_index(list, 1);
    while (elements->type == OBJECT_PAIR) {
        Object *element = elements->value.pair[0];
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res -= element->value.fixnum;
        elements = elements->value.pair[1];
    }

    Object *result = (Object*)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object* builtin_mul(Object *list) {
    if (list_len(list) < 2) {
        return NULL;
    }

    fixnum res = 1;
    Object *elements = list;
    while (elements->type == OBJECT_PAIR) {
        Object *element = elements->value.pair[0];
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res *= element->value.fixnum;
        elements = elements->value.pair[1];
    }

    Object *result = (Object*)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object* builtin_quote(Object *list) {
      if (list_len(list) != 1) {
        return NULL;
      }

      Object *obj = list_index_get(list, 0);
      obj->quoted = true;

      return obj;
}

Object* builtin_atom(Object *list){
    return NULL;  
}

Object* builtin_eq(Object *list){
    return NULL;  
}

Object* builtin_cdr(Object *list){
    return NULL;  
}

Object* builtin_car(Object *list){
    return NULL;  
}

Object* builtin_cons(Object *list){
    return NULL;  
}

Object* builtin_cond(Object *list){
    return NULL;  
}

// We guarantee that objects coming in are in cons list format
Object* eval_all(Object *obj, Object **env) {
    Object *p = obj;
    while (p->value.pair[1]->type != OBJECT_NIL) {
        p->value.pair[0] = eval_sexpression(p->value.pair[0], env);
        p = p->value.pair[1];
    }

    print_sexpression(obj);
    return obj;
}

Object* eval_sexpression(Object *obj, Object **env) {
    /*
    Evaluate based on type

    Some types are directly self-resolving (meaning we know exactly what we 
    need to do with them without any processing)

    Some other types like lists, we need to do additional parsing on top to know whether we need to
    evaluate a built-in function, a lambda, or simply just print out a list.
    */

    switch (obj->type) {
    case OBJECT_FIXNUM: // fallthrough
    case OBJECT_BOOLEAN: // fallthrough
    case OBJECT_NIL:
        return obj;
    case OBJECT_SYMBOL: // symbol search
        // TODO: add quoted flag check
        Object *res = env_search(env, obj);
        return res; // TODO: tbf i dont think this behaviour is good
    case OBJECT_PAIR:
        // Actual case where the OBJECT_PAIR really is a pair => just print "car" and "cdr"
        if (!is_object_list(obj)) {
            return obj;
        }

        if (obj->quoted) {
            return obj;
        }
        
        // TODO: change to eval-apply pattern
        Object* first = list_index_get(obj, 0);
        if (first->type != OBJECT_SYMBOL) {
            return obj;
        }

        // Special functions
        if (is_built_in(first, "val")) {
            return builtin_val(list_index(obj, 1), env);
        } else if (is_built_in(first, "quote")) {
            return builtin_quote(list_index(obj, 1));
        } else if (is_built_in(first, "cond")) {
            return builtin_cond(list_index(obj, 1)); // TODO
        }
        // Generic functions: pre-evaluate all arguments
        return apply_func(first->value.symbol, eval_all(list_index(obj, 1), env));
    }

    return NULL;
}

Object* apply_func(symbol func_name, Object *args) {
    for (int i = 0; builtin[i] != NULL; i++) {
        if (strcmp(builtin[i]->name, func_name) == 0) {
            return builtin[i]->func(args);
        }
    }

    return NULL;
}

int eval(char *buffer, Object **env) {
    // TODO: need AST parser???
    Object obj = {};

    buffer = parse_sexpression(&obj, buffer); // parse object from buffer
    
    Object *eval_obj = eval_sexpression(&obj, env); // evaluate object

    print_sexpression(eval_obj); // print object
    printf("\n");

    return 0;
}
