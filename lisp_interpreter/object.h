#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.

// Primitive Function Names
#define NIL "nil"
#define IF "if"
#define AND "and"
#define OR "or"
#define QUOTE "quote"
#define VAL "val"

typedef int64_t fixnum;
typedef bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
typedef char *symbol;

typedef union Object_Value Object_Value;
typedef enum Object_Type Object_Type;
typedef struct Object Object;
typedef struct Function Function;

enum Object_Type { // TODO: add double and string?
    OBJECT_NIL,
    OBJECT_FIXNUM,
    OBJECT_BOOLEAN,
    OBJECT_SYMBOL,
    OBJECT_PAIR,
    OBJECT_QUOTE,
};

union Object_Value {
    int64_t fixnum;
    bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
    char *symbol;
    struct Object *nil;     // might not be needed?
    struct Object *pair[2]; // struct Object* is able to compile because pointer have fixed size
    struct Object *quote;
};

struct Object {
    Object_Type type;
    Object_Value value;
};

#endif // TYPES_H
