#ifndef REPL_HEADER
#define REPL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // for int64_t et al.
#include <stdbool.h> // for bool
#include <assert.h>

#define NIL "nil"
#define BUILTIN_VAL "val" // e.g. (val x 7) => assigns x to 7
#define BUILTIN_IF "if" // e.g. (if condition action-for-true action-for-false)
#define BUILTIN_ADD "+" // e.g. (+ 6 7)
#define BUILTIN_SUB_NEG "-" // e.g. (- 6) or (- 7 6)
#define BUILTIN_MUL "*" // e.g. (* 6 7)

typedef int64_t fixnum;
typedef bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
typedef char* symbol;

typedef union Object_Value Object_Value;
typedef enum Object_Type Object_Type;
typedef struct Object Object;

enum Object_Type {
    OBJECT_NIL,
    OBJECT_FIXNUM,
    OBJECT_BOOLEAN,
    OBJECT_SYMBOL,
    OBJECT_PAIR,
};

union Object_Value {
    int64_t fixnum;
    bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
    char* symbol;
    struct Object *nil; // might not be needed?
    struct Object *pair[2]; // struct Object* is able to compile because pointer have fixed size
};

struct Object {
    Object_Type type;
    Object_Value value;
};

// Environment
bool env_put(Object **env, Object *key, Object *value); // then we must create entry out of key and value
Object* env_search(Object **env, Object *key); // returns Object* which represents the value in env

// Builtin Lisp Functions
Object* builtin_val(Object *list, Object **env);
Object* builtin_if(Object *list, Object **env);
Object* builtin_add(Object *list, Object **env);
Object* builtin_sub_neg(Object *list, Object **env); // if 1 argument, return negation, else, subtract values in sequence
Object* builtin_mul(Object *list, Object **env);
// Object* builtin_div(Object *list, Object **env);


// Functions
char* trim_whitespace(char *buffer);
bool is_whitespace(char *buffer);
bool is_object_list(Object *obj);
int list_len(Object *list);
Object* list_index(Object *list, int ind);
Object* list_index_get(Object *list, int ind);

char* parse_fixnum(Object *obj, char *p);
char* parse_boolean(Object *obj, char *p);
char* parse_symbol(Object *obj, char *p);
char* parse_pair(Object *obj, char *p);
char* parse_sexpression(Object *obj, char *buffer);
void print_list(Object *obj);
void print_sexpression(Object *obj);
bool is_built_in(Object *obj, char *func_name); // TODO: need to verify type of first element in list first
Object* eval_sexpression(Object *obj, Object **env); // evaluate sexpression list

int eval(char *buffer, Object **env); // main evaluation entrypoint

#endif // REPL_HEADER