#ifndef REPL_H
#define REPL_H

#include <assert.h>
#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "env.h"
#include "object.h"
#include "object_helpers.h"
#include "parser.h"

typedef struct Function Function;

struct Function {
    symbol name;
    Object *(*func)(Expression *, Env *);
    // int num_args;
};

// Functions
bool is_built_in(Object *first,
                 char *func_name); // TODO: need to verify type of first element in list first
bool is_one_of_built_in(Object *first);

// Builtin Lisp Functions
// TODO: add a function mapping?s
Object *builtin_add(Expression *call_exp, 
                    Env *env);
Object *builtin_sub_neg(Expression *call_exp,
                        Env *env); // if 1 argument, return negation, else, subtract values in sequence
Object *builtin_mul(Expression *call_exp, 
                    Env *env);

// TODO: the following comparison operators currently only support fixnums 
Object *builtin_gt(Expression *call_exp, 
                    Env *env);
Object *builtin_lt(Expression *call_exp, 
                    Env *env);
Object *builtin_leq(Expression *call_exp, 
                    Env *env);
Object *builtin_geq(Expression *call_exp, 
                    Env *env);
Object *builtin_eq(Expression *call_exp, 
                    Env *env);
Object *builtin_atom(Expression *call_exp, 
                     Env *env); // (atom x)
Object *builtin_eql(Expression *call_exp,
                   Env *env); // (eq a b) => compares if atoms are equivalent or if memory
                                  // addresses are the same
// "Content of Address part of Register"
Object *builtin_car(Expression *call_exp,
                    Env *env); // (car list) => returns left side of a cons cell
// "Content of Decrement part of Register"
Object *builtin_cdr(Expression *call_exp,
                    Env *env); // (cdr list) => returns remaining pair of list
Object *builtin_cons(Expression *call_exp,
                     Env *env); // (cons a b) => constructs cons cell from two elements
// Object *builtin_cond(Expression *call_exp,
//                      Env *env); // (cond (condition1 action1) (condition2 action2) ...)

// Object *eval_all(Object *obj, Env *env);
Object *eval_ast(Expression *exp, Env *env); // evaluate ast
Object *apply_func(Expression *call_exp, Env *env);

int eval(char *buffer, Env *env, Object **pool); // main evaluation entrypoint

#endif // REPL_H