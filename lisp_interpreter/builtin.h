#ifndef BUILTIN_H
#define BUILTIN_H

#include "types.h"

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

#endif // BUILTIN_H
