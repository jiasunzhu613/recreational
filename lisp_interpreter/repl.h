#ifndef REPL_H
#define REPL_H

#include <assert.h>
#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "builtin.h"
#include "object.h"
#include "object_helpers.h"
#include "parser.h"

// Functions
bool is_built_in(Object *first,
                 char *func_name); // TODO: need to verify type of first element in list first
bool is_one_of_built_in(Object *first);

// Object *eval_all(Object *obj, Object **env);
Object *eval_ast(Expression *exp, Object **env); // evaluate ast
Object *apply_func(Expression *call_exp, Object **env);

int eval(char *buffer, Object **env, Object **pool); // main evaluation entrypoint

#endif // REPL_H