/*
Read, Evaluate, Print Loop for interpreting LISP

DISCLAIMER: this may not be an exact one to one replication of any specific well-known LISP dialect

Recreational programming session referencing:
- https://groups.csail.mit.edu/mac/classes/6.001/abelson-sussman-lectures/
- https://bernsteinbear.com/blog/lisp/
*/

#include "repl.h"

// NOTE: {} syntax provides the data, but we
// still need to cast to Function type!
Function *builtin[] = {&(Function){.name = "+", .func = builtin_add},
                       &(Function){.name = "-", .func = builtin_sub_neg},
                       &(Function){.name = "*", .func = builtin_mul},
                       &(Function){.name = "atom", .func = builtin_atom},
                       &(Function){.name = "cdr", .func = builtin_cdr},
                       &(Function){.name = "car", .func = builtin_car},
                       &(Function){.name = "cons", .func = builtin_cons},
                       &(Function){.name = "eq", .func = builtin_eq},
                       NULL};

bool env_put(Object **env, Object *key, Object *value) {
    // always pair up
    Object *entry = (Object *)calloc(1, sizeof(Object));
    entry->type = OBJECT_PAIR;
    entry->value.pair[0] = key;
    entry->value.pair[1] = value;

    Object *new_env = (Object *)calloc(1, sizeof(Object));
    new_env->type = OBJECT_PAIR;
    new_env->value.pair[0] = entry;
    new_env->value.pair[1] = *env;

    print_sexpression(new_env);

    *env = new_env;
    return true;
}

Object *env_search(Object **env, symbol key) {
    // TODO: need some way to compare the value of key which will be a symbol and what is in the
    // env, compare strings, strcmp key should never be a non-symbol
    Object *p = *env;

    while (p->type != OBJECT_NIL) {
        Object *entry =
            p->value.pair[0]; // TODO: might need to validate this, this should be a entry though
        Object *entry_key = entry->value.pair[0]; // get first elem from entry pair
        printf("DEBUG: on entry\n");
        print_sexpression(entry);
        printf("\n");
        if (entry_key->type != OBJECT_SYMBOL) {
            fprintf(stderr, "CRITICAL: environment entry has non-symbol key\n");
            exit(1);
        }

        if (strcmp(entry_key->value.symbol, key) == 0) {
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

Object *pool_put(Object **pool, symbol sym) {
    // always pair up
    Object *entry = (Object *)calloc(1, sizeof(Object));
    entry->type = OBJECT_SYMBOL;
    entry->value.symbol = sym;

    Object *new_pool = (Object *)calloc(1, sizeof(Object));
    new_pool->type = OBJECT_PAIR;
    new_pool->value.pair[0] = entry;
    new_pool->value.pair[1] = *pool;

    print_sexpression(new_pool);

    *pool = new_pool;
    return entry;
}

Object *pool_search(Object **pool, symbol sym) {
    Object *p = *pool;

    while (p->type != OBJECT_NIL) {
        Object *entry =
            p->value.pair[0]; // TODO: might need to validate this, this should be a entry though
        symbol entry_sym = entry->value.symbol; // get symbol from entry

        if (strcmp(entry_sym, sym) == 0) {
            return entry;
        }

        p = p->value.pair[1];
    }

    printf("DEBUG: DID NOT FIND SHIT IN POOL_SEARCH\n");
    return NULL; // TODO: decide what to do here?
}

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

Object *list_index(Object *list, int ind) {
    while (ind > 0 && list->type != OBJECT_NIL) {
        ind--;
        list = list->value.pair[1];
    }

    if (ind > 0) {
        return NULL;
    }

    return list;
}

Object *list_index_get(Object *list, int ind) { return list_index(list, ind)->value.pair[0]; }

// NOTE: we guarantee that the Object coming in is of list type
// TODO: maybe change this to loop through enums and then return an enum as well?
bool is_built_in(Object *first, char *func_name) {
    if (!(first->type == OBJECT_SYMBOL)) {
        return false;
    }

    return strcmp(first->value.symbol, func_name) == 0;
}

bool is_one_of_built_in(Object *first) {
    for (int i = 0; builtin[i] != NULL; i++) {
        if (strcmp(builtin[i]->name, first->value.symbol) == 0) {
            return true;
        }
    }

    return false;
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

Expression *build_ast(Object *obj) {
    Expression *expression = (Expression *)calloc(1, sizeof(Expression));
    expression->statement = (Expression_Statement *)calloc(1, sizeof(Expression_Statement));

    switch (obj->type) {
    case OBJECT_NIL:    // fallthrough
    case OBJECT_FIXNUM: // fallthrough
    case OBJECT_QUOTE:  // fallthrough
    case OBJECT_BOOLEAN:
        expression->type = EXPR_LITERAL;
        expression->statement->literal_expr = obj;
        break;
    case OBJECT_SYMBOL:
        expression->type = EXPR_VAR;
        expression->statement->var_expr = (Var){.name = obj->value.symbol};
        break;
    case OBJECT_PAIR:
        if (!is_object_list(obj)) {
            expression->type = EXPR_LITERAL;
            expression->statement->literal_expr = obj;
            break;
        }

        Object *first = list_index_get(obj, 0);

        // Handle definition expressions first
        // TODO: needs lots of error handling!!
        if (strcmp(first->value.symbol, IF) == 0) {
            // TODO: error handling
            expression->type = EXPR_IF;
            expression->statement->if_expr.condition = build_ast(list_index_get(obj, 1));
            expression->statement->if_expr.if_true = build_ast(list_index_get(obj, 2));
            expression->statement->if_expr.if_false = build_ast(list_index_get(obj, 3));
        } else if (strcmp(first->value.symbol, AND) == 0) {
            expression->type = EXPR_AND;
            expression->statement->and_expr.left = build_ast(list_index_get(obj, 1));
            expression->statement->and_expr.right = build_ast(list_index_get(obj, 2));
        } else if (strcmp(first->value.symbol, OR) == 0) {
            expression->type = EXPR_OR;
            expression->statement->or_expr.left = build_ast(list_index_get(obj, 1));
            expression->statement->or_expr.right = build_ast(list_index_get(obj, 2));
        } else if (strcmp(first->value.symbol, QUOTE) == 0) {
            // build object into a quote object first then build_ast with quoted object
            Object *quoted = (Object *)calloc(1, sizeof(Object));
            quoted->type = OBJECT_QUOTE;
            quoted->value.quote = list_index_get(obj, 1);

            expression->type = EXPR_QUOTE;
            expression->statement->quote_expr.value = build_ast(quoted);
        } else if (strcmp(first->value.symbol, VAL) == 0) {
            expression->type = EXPR_DEF;
            Def_Expression *def_expr = (Def_Expression *)calloc(1, sizeof(Def_Expression));
            def_expr->statement =
                (Def_Expression_Statement *)calloc(1, sizeof(Def_Expression_Statement));
            Object *name = list_index_get(obj, 1);
            Object *value = list_index_get(obj, 2);

            // TODO: need to check types
            def_expr->type = EXPR_VAL;
            def_expr->statement->val_expr.name = name->value.symbol;
            def_expr->statement->val_expr.assign_value = build_ast(value);
            expression->statement->def_expr = def_expr;
        } else {
            // TODO: need to check types
            printf("DEBUG: IN BUILT AST, GOT: %s\n", first->value.symbol);
            expression->type = EXPR_CALL;
            expression->statement->call_expr.name = first->value.symbol;

            Expression **args = (Expression **)calloc(list_len(obj), sizeof(Expression *));
            Object *args_object = list_index(obj, 1);

            // Build all list elements into array of AST expressions

            int i;
            for (i = 0; args_object->type != OBJECT_NIL; i++) {
                args[i] = build_ast(args_object->value.pair[0]);
                args_object = args_object->value.pair[1];
            }

            expression->statement->call_expr.args = args;
            expression->statement->call_expr.num_args = i;
        }
        break;
    }

    return expression;
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
    case OBJECT_QUOTE:
        print_sexpression(obj->value.quote);
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

// TODO: print AST?

// BUILTIN Functions

Object *builtin_add(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args < 2) {
        return NULL;
    }

    fixnum res = 0;
    for (int i = 0; i < call_exp->statement->call_expr.num_args; i++) {
        Object *element = eval_ast(call_exp->statement->call_expr.args[i], env);
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res += element->value.fixnum;
    }

    Object *result = (Object *)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object *builtin_sub_neg(Expression *call_exp, Object **env) {
    int len = call_exp->statement->call_expr.num_args;
    if (len < 1) {
        return NULL;
    }

    if (len == 1) { // negation
        Object *elem = eval_ast(call_exp->statement->call_expr.args[0], env);
        if (elem->type != OBJECT_FIXNUM) {
            return NULL;
        }

        fixnum res = elem->value.fixnum * -1;
        Object *result = (Object *)calloc(1, sizeof(Object));
        result->type = OBJECT_FIXNUM;
        result->value.fixnum = res;

        return result;
    }

    Object *elem = eval_ast(call_exp->statement->call_expr.args[0], env);
    if (elem->type != OBJECT_FIXNUM) {
        return NULL;
    }
    fixnum res = elem->value.fixnum;

    for (int i = 1; i < len; i++) {
        Object *element = eval_ast(call_exp->statement->call_expr.args[i], env);
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res -= element->value.fixnum;
    }

    Object *result = (Object *)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object *builtin_mul(Expression *call_exp, Object **env) {
    int len = call_exp->statement->call_expr.num_args;
    if (len < 2) {
        return NULL;
    }

    fixnum res = 1;
    for (int i = 0; i < len; i++) {
        Object *element = eval_ast(call_exp->statement->call_expr.args[i], env);
        if (element->type != OBJECT_FIXNUM) {
            return NULL;
        }

        res *= element->value.fixnum;
    }

    Object *result = (Object *)calloc(1, sizeof(Object));
    result->type = OBJECT_FIXNUM;
    result->value.fixnum = res;

    return result;
}

Object *builtin_atom(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args != 1) {
        return NULL;
    }

    Object *inner_elem = eval_ast(call_exp->statement->call_expr.args[0], env);
    Object *ret = (Object *)calloc(1, sizeof(Object));
    ret->type = OBJECT_BOOLEAN;

    ret->value.boolean = !is_object_list(inner_elem);

    return ret;
}

Object *builtin_eq(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args != 2) {
        return NULL;
    }

    Object *obj1 = eval_ast(call_exp->statement->call_expr.args[0], env);
    Object *obj2 = eval_ast(call_exp->statement->call_expr.args[1], env);

    Object *res = (Object *)calloc(1, sizeof(Object));
    res->type = OBJECT_BOOLEAN;
    res->value.boolean = obj1 == obj2;

    return res;
}

Object *builtin_cdr(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args != 1) {
        return NULL;
    }

    Object *inner_list = eval_ast(call_exp->statement->call_expr.args[0], env);
    if (inner_list->type != OBJECT_PAIR) {
        return NULL;
    }

    return inner_list->value.pair[1];
}

Object *builtin_car(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args != 1) {
        return NULL;
    }

    Object *inner_list = eval_ast(call_exp->statement->call_expr.args[0], env);
    printf("DEBUG: TYPE OF INNERLIST: %d\n", inner_list->type);
    if (inner_list->type != OBJECT_PAIR) {
        return NULL;
    }

    return inner_list->value.pair[0];
}

Object *builtin_cons(Expression *call_exp, Object **env) {
    if (call_exp->statement->call_expr.num_args != 2) {
        return NULL;
    }

    Object *obj = (Object *)calloc(1, sizeof(Object));
    obj->type = OBJECT_PAIR;
    obj->value.pair[0] = eval_ast(call_exp->statement->call_expr.args[0], env);
    obj->value.pair[1] = eval_ast(call_exp->statement->call_expr.args[1], env);

    return obj;
}

// TODO: this need to have special handling
// Object *builtin_cond(Expression *call_exp, Object **env) {
//     if (call_exp->statement->call_expr.num_args < 1) {
//         return NULL;
//     }

//     for (int i = 0; i < call_exp->statement->call_expr.num_args; i++) {
//         Object *pair = eval_ast(call_exp->statement->call_expr.args[i], env);
//         if (!is_object_list(pair)) {
//             return NULL;
//         }
//         if (list_len(pair) != 2) {
//             return NULL;
//         }

//         Object *condition = list_index_get(pair, 0);
//         if (condition->type != OBJECT_BOOLEAN) {
//             return NULL;
//         }

//         if (condition->value.boolean) {
//             return list_index_get(pair, 1);
//         }
//     }

//     return NULL; // no condition matched
// }

// We guarantee that objects coming in are in cons list format
// Object *eval_all(Expression *exps, Object **env) {
//     Object *p = obj;
//     while (p->type != OBJECT_NIL) {
//         p->value.pair[0] = eval_sexpression(p->value.pair[0], env);
//         p = p->value.pair[1];
//     }

//     return obj;
// }

Object *eval_ast(Expression *exp, Object **env) {
    /*
    Evaluate based on type

    Some types are directly self-resolving (meaning we know exactly what we
    need to do with them without any processing)

    Some other types like lists, we need to do additional parsing on top to know whether we need to
    evaluate a built-in function, a lambda, or simply just print out a list.
    */

    switch (exp->type) {
    case EXPR_LITERAL:
        Object *literal = exp->statement->literal_expr;
        if (literal->type == OBJECT_QUOTE) {
            literal = literal->value.quote;
        }
        return literal;
    case EXPR_VAR: // symbol search
        Object *res = env_search(env, exp->statement->var_expr.name);
        return res; // TODO: tbf i dont think this behaviour is good, might need to change the NULL
                    // default behaviour
    case EXPR_IF:
        Object *cond = eval_ast(exp->statement->if_expr.condition, env);

        // Check cond type
        if (cond->type != OBJECT_BOOLEAN) {
            return NULL; // TODO: determine behaviour
        }

        return cond->value.boolean ? eval_ast(exp->statement->if_expr.if_true, env)
                                   : eval_ast(exp->statement->if_expr.if_false, env);
    case EXPR_AND: {
        Object *left = eval_ast(exp->statement->and_expr.left, env);
        Object *right = eval_ast(exp->statement->and_expr.right, env);

        if (left->type != OBJECT_BOOLEAN || right->type != OBJECT_BOOLEAN) {
            return NULL;
        }

        Object *ret = (Object *)calloc(1, sizeof(Object));
        ret->type = OBJECT_BOOLEAN;
        ret->value.boolean = left->value.boolean && right->value.boolean;

        return ret;
    }
    case EXPR_OR: {
        Object *left = eval_ast(exp->statement->and_expr.left, env);
        Object *right = eval_ast(exp->statement->and_expr.right, env);

        if (left->type != OBJECT_BOOLEAN || right->type != OBJECT_BOOLEAN) {
            return NULL;
        }

        Object *ret = (Object *)calloc(1, sizeof(Object));
        ret->type = OBJECT_BOOLEAN;
        ret->value.boolean = left->value.boolean || right->value.boolean;

        return ret;
    }
    case EXPR_QUOTE:
        return eval_ast(exp->statement->quote_expr.value, env);
    case EXPR_DEF:
        Def_Expression *def_exp = exp->statement->def_expr;
        switch (def_exp->type) {
        case EXPR_VAL:
            Object *key = (Object *)calloc(1, sizeof(Object));
            key->type = OBJECT_SYMBOL;
            key->value.symbol = def_exp->statement->val_expr.name;

            bool success =
                env_put(env, key, eval_ast(def_exp->statement->val_expr.assign_value, env));
            // TODO: check success?
            break;
        }
        break;
    case EXPR_CALL:
        return apply_func(exp, env);
    }

    return NULL;
}

Object *apply_func(Expression *call_exp, Object **env) {
    for (int i = 0; builtin[i] != NULL; i++) {
        if (strcmp(builtin[i]->name, call_exp->statement->call_expr.name) == 0) {
            printf("DEBUG: FOUND FUNCTION!!!\n");
            return builtin[i]->func(call_exp,
                                    env); // TODO: need to fix all builtin function calls!!!
        }
    }

    return NULL;
}

int eval(char *buffer, Object **env, Object **pool) {
    // TODO: need AST parser???
    Object *obj = (Object *)calloc(1, sizeof(Object));

    buffer = parse_sexpression(&obj, buffer, pool); // parse object from buffer

    Expression *ast_expr = build_ast(obj);

    Object *eval_obj = eval_ast(ast_expr, env); // evaluate object

    print_sexpression(eval_obj); // print object
    printf("\n");

    return 0;
}
