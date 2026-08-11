#ifndef REPL_H
#define REPL_H

#include <assert.h>
#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "object.h"
#include "object_helpers.h"
#include "parser.h"

// Functions
bool is_built_in(Object *first,
                 char *func_name); // TODO: need to verify type of first element in list first
bool is_one_of_built_in(Object *first);

struct Function {
    symbol name;
    Object *(*func)(Expression*, Object**);
    // int num_args;
};

// Builtin Lisp Functions
// TODO: add a function mapping?s
Object *builtin_add(Expression *call_exp, Object **env);
Object *builtin_sub_neg(Expression *call_exp, Object **env); // if 1 argument, return negation, else, subtract values in sequence
Object *builtin_mul(Expression *call_exp, Object **env);

Object *builtin_atom(Expression *call_exp, Object **env);  // (atom x)
Object *builtin_eq(Expression *call_exp, Object **env);    // (eq a b) => compares if atoms are equivalent or if memory
                                     // addresses are the same
Object *builtin_cdr(Expression *call_exp, Object **env);   // (cdr list) => returns left side of a cons cell
Object *builtin_car(Expression *call_exp, Object **env);   // (car list) => returns remaining pair of list
Object *builtin_cons(Expression *call_exp, Object **env);  // (cons a b) => constructs cons cell from two elements
// Object *builtin_cond(Expression *call_exp,
//                      Object **env); // (cond (condition1 action1) (condition2 action2) ...)

// Object *eval_all(Object *obj, Object **env);
Object *eval_ast(Expression *exp, Object **env); // evaluate ast
Object *apply_func(Expression *call_exp, Object **env);

int eval(char *buffer, Object **env, Object **pool); // main evaluation entrypoint

#endif // REPL_H