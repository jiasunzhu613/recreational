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
                       &(Function){.name = "*", .func = builtin_mul}, // TODO: division?
                       &(Function){.name = "atom", .func = builtin_atom},
                       &(Function){.name = "cdr", .func = builtin_cdr},
                       &(Function){.name = "car", .func = builtin_car},
                       &(Function){.name = "cons", .func = builtin_cons},
                       &(Function){.name = "eq", .func = builtin_eq},
                       NULL};

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

// BUILTIN Functions
// TODO: find a way to move these out?

Object *builtin_add(Expression *call_exp, Env *env) {
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

Object *builtin_sub_neg(Expression *call_exp, Env *env) {
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

Object *builtin_mul(Expression *call_exp, Env *env) {
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

Object *builtin_atom(Expression *call_exp, Env *env) {
    if (call_exp->statement->call_expr.num_args != 1) {
        return NULL;
    }

    Object *inner_elem = eval_ast(call_exp->statement->call_expr.args[0], env);
    Object *ret = (Object *)calloc(1, sizeof(Object));
    ret->type = OBJECT_BOOLEAN;

    ret->value.boolean = !is_object_list(inner_elem);

    return ret;
}

Object *builtin_eq(Expression *call_exp, Env *env) {
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

Object *builtin_car(Expression *call_exp, Env *env) {
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

Object *builtin_cdr(Expression *call_exp, Env *env) {
    if (call_exp->statement->call_expr.num_args != 1) {
        return NULL;
    }

    Object *inner_list = eval_ast(call_exp->statement->call_expr.args[0], env);
    if (inner_list->type != OBJECT_PAIR) {
        return NULL;
    }

    return inner_list->value.pair[1];
}

Object *builtin_cons(Expression *call_exp, Env *env) {
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
// Object *builtin_cond(Expression *call_exp, Env *env) {
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
// Object *eval_all(Expression *exps, Env *env) {
//     Object *p = obj;
//     while (p->type != OBJECT_NIL) {
//         p->value.pair[0] = eval_sexpression(p->value.pair[0], env);
//         p = p->value.pair[1];
//     }

//     return obj;
// }

Object *eval_ast(Expression *exp, Env *env) {
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
        case EXPR_DEFUN:
            // TODO: register function into Environment?
            // TODO: we can't evaluate the body because we need the AST expression for handling variables later on
            env_funcs_append(env, exp->statement->def_expr->statement->defun_expr);
            printf("DEBUG: size of env func table: %d\n", env->size);
            break;
        }
        break;
    case EXPR_CALL:
        return apply_func(exp, env);
    }

    return NULL;
}

// TODO: this needs to change to support functions
Object *apply_func(Expression *call_exp, Env *env) {
    for (int i = 0; builtin[i] != NULL; i++) {
        if (strcmp(builtin[i]->name, call_exp->statement->call_expr.name) == 0) {
            printf("DEBUG: FOUND FUNCTION!!!\n");
            return builtin[i]->func(call_exp,
                                    env); // TODO: need to fix all builtin function calls!!!
        }
    }

    // TODO; check function table?
    printf("DEBUG: GOT INTO apply_func\n");
    for (int i = 0; i < env->size; i++) {
        printf("DEBUG: found name: %s, num_args: %d\n", env->funcs[i].name, env->funcs[i].num_args);
        if (strcmp(env->funcs[i].name, call_exp->statement->call_expr.name) == 0) {
            printf("DEBUG: FOUND FUNCTION IN ENV\n");
            Defun func = env->funcs[i];

            // Create new environment to run function in 
            Env *local = create_env();

            // Fill environment with values
            // TODO: prob need some error checking here too for args names and values
            if (func.num_args != call_exp->statement->call_expr.num_args) {
                fprintf(stderr, "ERROR: number of arguments don't match when making function call\n");
                exit(1);
            }

            for (int i = 0; i < func.num_args; i++) {
                Object *key = obj_box(OBJECT_SYMBOL);
                key->value.symbol = func.args[i];
                Object *value = eval_ast(call_exp->statement->call_expr.args[i], env);
                env_put(local, key, value); // TODO: do we need smt to ensure type?
            }

            return eval_ast(func.body, local);
        }
    }

    return NULL;
}

int eval(char *buffer, Env *env, Object **pool) {
    // TODO: need AST parser???
    Object *obj = (Object *)calloc(1, sizeof(Object));

    buffer = parse_sexpression(&obj, buffer, pool); // parse object from buffer

    Expression *ast_expr = build_ast(obj);

    Object *eval_obj = eval_ast(ast_expr, env); // evaluate object

    print_sexpression(eval_obj); // print object
    printf("\n");

    return 0;
}
