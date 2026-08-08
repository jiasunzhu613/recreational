#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h> // for bool
#include <stdint.h>  // for int64_t et al.

typedef int64_t fixnum;
typedef bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
typedef char *symbol;

typedef union Object_Value Object_Value;
typedef enum Object_Type Object_Type;
typedef struct Object Object;
typedef struct Function Function;

typedef Object Value;                 // Self-resolving object types
typedef struct Expression Expression; // Expression tagged union
typedef enum Expression_Type Expression_Type;
typedef union Expression_Statement Expression_Statement;
typedef struct If If;
typedef struct Var Var;
typedef struct And And;
typedef struct Or Or;
typedef struct Apply Apply;
typedef struct Call Call;
typedef struct Val Val;
typedef struct Def_Expression Def_Expression;
typedef enum Def_Expression_Type Def_Expression_Type;
typedef union Def_Expression_Statement Def_Expression_Statement;

enum Object_Type { // TODO: add double and string?
    OBJECT_NIL,
    OBJECT_FIXNUM,
    OBJECT_BOOLEAN,
    OBJECT_SYMBOL,
    OBJECT_PAIR,
};

union Object_Value {
    int64_t fixnum;
    bool boolean; // We will use the scheme method of representing boolean values: #t,  #f
    char *symbol;
    struct Object *nil;     // might not be needed?
    struct Object *pair[2]; // struct Object* is able to compile because pointer have fixed size
};

struct Object {
    Object_Type type;
    Object_Value value;
    bool quoted;
};

struct Function {
    symbol name;
    Object *(*func)(Expression*, Object**);
    // int num_args;
};

enum Expression_Type { EXPR_LITERAL, EXPR_IF, EXPR_VAR, EXPR_AND, EXPR_OR, EXPR_CALL, EXPR_DEF };

enum Def_Expression_Type {
    EXPR_VAL,
    // EXPR_DEFINE
};

// Operations
struct If {
    Expression *condition;
    Expression *if_true;
    Expression *if_false;
};

struct Var {
    symbol name;
};

struct And {
    Expression *left;
    Expression *right; // TODO: is this enough for a recursive definition?
};

struct Or {
    Expression *left;
    Expression *right; // TODO: is this enough for a recursive definition?
};

// Function call for closures
struct Call {
    symbol name;
    Expression **args;
    int num_args;
};

// Function call for closures
struct Val {
    symbol name;
    Expression *assign_value;
};

union Def_Expression_Statement {
    Val val_expr;
};

struct Def_Expression {
    Def_Expression_Type type;
    Def_Expression_Statement *statement;
};

union Expression_Statement {
    Value *literal_expr;
    If if_expr;
    Var var_expr;
    And and_expr;
    Or or_expr;
    Call call_expr;
    Def_Expression *def_expr;
};

struct Expression {
    Expression_Type type;
    Expression_Statement *statement;
};

#endif // TYPES_H
