#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "object.h"
#include "object_helpers.h"

// Primitive Function Names
#define NIL "nil"
#define IF "if"
#define AND "and"
#define OR "or"
#define QUOTE "quote"
#define VAL "val"
#define DEFUN "defun"

typedef Object Value;                 // Self-resolving object types
typedef struct Expression Expression; // Expression tagged union
typedef enum Expression_Type Expression_Type;
typedef union Expression_Statement Expression_Statement;
typedef struct If If;
typedef struct Var Var;
typedef struct And And;
typedef struct Or Or;
typedef struct Quote Quote;
typedef struct Apply Apply;
typedef struct Call Call;
typedef struct Def_Expression Def_Expression;
typedef enum Def_Expression_Type Def_Expression_Type;
typedef union Def_Expression_Statement Def_Expression_Statement;
typedef struct Val Val;
typedef struct Defun Defun;

enum Expression_Type { EXPR_LITERAL, EXPR_IF, EXPR_VAR, EXPR_AND, EXPR_OR, EXPR_QUOTE, EXPR_CALL, EXPR_DEF };

enum Def_Expression_Type {
    EXPR_VAL,
    EXPR_DEFUN
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

struct Quote {
    Expression *value; // should this be Object*? maybe we just convert to literal directly in build_ast?
};

// Function call for functions and closures
struct Call {
    symbol name;
    Expression **args;
    int num_args;
};

struct Val {
    symbol name;
    Expression *assign_value;
};

struct Defun {
    symbol name;
    // We will create local env for functions, and we will have array of symbols for names
    int num_args;
    symbol *args;
    Expression *body; // Will use Call object to call function
};

// Function call for closures
union Def_Expression_Statement {
    Val val_expr;
    Defun defun_expr;
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
    Quote quote_expr;
    Call call_expr;
    Def_Expression *def_expr;
};

struct Expression {
    Expression_Type type;
    Expression_Statement *statement;
};

// AST Functions
Expression *build_ast(Object *obj);

#endif // AST_H