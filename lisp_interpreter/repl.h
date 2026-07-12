#ifndef REPL_HEADER
#define REPL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // for int64_t et al.
#include <stdbool.h> // for bool
#include <assert.h>

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

// Functions=
char* trim_whitespace(char *buffer);
bool is_whitespace(char *buffer);
bool is_object_list(Object *obj);
char* parse_fixnum(Object *obj, char *p);
char* parse_boolean(Object *obj, char *p);
char* parse_symbol(Object *obj, char *p);
char* parse_nil(Object *obj, char *p);
char* parse_pair(Object *obj, char *p);
char* parse_sexpression(Object *obj, char *buffer);
void print_list(Object *obj);
void print_sexpression(Object *obj);


#endif // REPL_HEADER