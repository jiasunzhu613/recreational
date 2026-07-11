#ifndef REPL_HEADER
#define REPL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // for int64_t et al.
#include <stdbool.h> // for bool

typedef int64_t fixnum;
typedef bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
typedef char character;

typedef union Object_Value Object_Value;
typedef enum Object_Type Object_Type;
typedef struct Object Object;

enum Object_Type {
    OBJECT_FIXNUM = 0,
    OBJECT_BOOLEAN = 1
};

union Object_Value {
    int64_t fixnum;
    bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
    char character;
};

struct Object {
    Object_Type type;
    Object_Value value;
};

// Functions
char* trim_whitespace(char *buffer);
char* parse_fixnum(Object *obj, char *p);
char* parse_boolean(Object *obj, char *p);
Object parse_sexpression(char *buffer);


#endif // REPL_HEADER