/* AST data model definitions*/

#include "repl.h"

typedef Object Value; // Self-resolving object types
typedef struct Expression Expression; // Expression tagged union
typedef enum Expression_Type Expression_Type;
typedef union Expression_Statement Expression_Statement;
typedef struct If If;
typedef struct And And;
typedef struct Or Or;
typedef struct Apply Apply;
typedef struct Call Call;
typedef struct Val Val;
typedef struct Def_Expression Def_Expression;
typedef enum Def_Expression_Type Def_Expression_Type;
typedef union Def_Expression_Statement Def_Expression_Statement;

enum Expression_Type {
    EXPR_LITERAL,
    EXPR_IF, 
    EXPR_VAR,
    EXPR_AND,
    EXPR_OR,
    EXPR_APPLY,
    EXPR_CALL,
    EXPR_DEF
};

enum Def_Expression_Type {
    EXPR_VAL,
    // EXPR_DEFINE
};

// Operations
struct If {
    Expression* condition;
    Expression* if_true;
    Expression* if_false;
};

struct And {
    Expression* left;
    Expression* right; // TODO: is this enough for a recursive definition?
};

struct Or {
    Expression* left;
    Expression* right; // TODO: is this enough for a recursive definition?
};

// Function call for primitive functions
struct Apply {
    symbol name;
    Expression* args;
};

// Function call for closures
struct Call {
    symbol name;
    Expression* args;
};

// Function call for closures
struct Val {
    symbol name;
    Expression* assign_value;
};

union Def_Expression_Statement {
    Val val_expr;
};

struct Def_Expression {
    Def_Expression_Type type;
    Def_Expression_Statement* statement;
};

union  Expression_Statement {
    Value* literal_expr;
    If if_expr;
    And and_expr;
    Or or_expr;
    Apply apply_expr;
    Call call_expr;
    Def_Expression* def_expr;
};

struct Expression {
    Expression_Type type;
    Expression_Statement* statement;
};


Expression* build_ast(Object *obj);
