#ifndef REPL_HEADER
#define REPL_HEADER

#include <assert.h>
#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "builtin.h"

// Primitive Function Names
#define NIL "nil"
#define IF "if"
#define AND "and"
#define OR "or"
#define VAL "val"

// Environment
bool env_put(Object **env, Object *key,
             Object *value);                   // then we must create entry out of key and value
Object *env_search(Object **env, Object *key); // returns Object* which represents the value in env

// Intern object pooling
// TODO: try to merge intern pool and environment functions into one common interface
Object *pool_put(Object **pool, symbol sym);
Object *pool_search(Object **pool, symbol sym);

// Functions
char *trim_whitespace(char *buffer);
bool is_whitespace(char *buffer);
bool is_object_list(Object *obj);
int list_len(Object *list);
Object *list_index(Object *list, int ind);
Object *list_index_get(Object *list, int ind);
bool is_built_in(Object *first,
                 char *func_name); // TODO: need to verify type of first element in list first
bool is_one_of_built_in(Object *first);

// TODO: this is kinda bad design
char *parse_fixnum(Object *obj, char *p);
char *parse_boolean(Object *obj, char *p);
char *parse_symbol(Object **obj, char *p, Object **pool);
char *parse_pair(Object *obj, char *p, Object **pool);
char *parse_sexpression(Object **obj, char *buffer, Object **pool);
void print_list(Object *obj);
void print_sexpression(Object *obj);
// Object *eval_all(Object *obj, Object **env);
Object *eval_ast(Expression *exp, Object **env); // evaluate ast
Object *apply_func(Expression *call_exp, Object **env);

// AST Functions
Expression *build_ast(Object *obj);

int eval(char *buffer, Object **env, Object **pool); // main evaluation entrypoint

#endif // REPL_HEADER