#include "ast.h"

Expression *build_ast(Object *obj) {
    Expression *expression = (Expression *)calloc(1, sizeof(Expression));
    expression->statement = (Expression_Statement *)calloc(1, sizeof(Expression_Statement));

    switch (obj->type) {
    case OBJECT_NIL:    // fallthrough
    case OBJECT_FIXNUM: // fallthrough
    case OBJECT_QUOTE:  // fallthrough
    case OBJECT_BOOLEAN:
        expression->type = EXPR_LITERAL;
        expression->statement->literal_expr = obj;
        break;
    case OBJECT_SYMBOL:
        expression->type = EXPR_VAR;
        expression->statement->var_expr = (Var){.name = obj->value.symbol};
        break;
    case OBJECT_PAIR:
        if (!is_object_list(obj)) {
            expression->type = EXPR_LITERAL;
            expression->statement->literal_expr = obj;
            break;
        }

        Object *first = list_index_get(obj, 0);

        // Handle definition expressions first
        // TODO: needs lots of error handling!!
        if (strcmp(first->value.symbol, IF) == 0) {
            // TODO: error handling
            expression->type = EXPR_IF;
            expression->statement->if_expr.condition = build_ast(list_index_get(obj, 1));
            expression->statement->if_expr.if_true = build_ast(list_index_get(obj, 2));
            expression->statement->if_expr.if_false = build_ast(list_index_get(obj, 3));
        } else if (strcmp(first->value.symbol, AND) == 0) {
            expression->type = EXPR_AND;
            expression->statement->and_expr.left = build_ast(list_index_get(obj, 1));
            expression->statement->and_expr.right = build_ast(list_index_get(obj, 2));
        } else if (strcmp(first->value.symbol, OR) == 0) {
            expression->type = EXPR_OR;
            expression->statement->or_expr.left = build_ast(list_index_get(obj, 1));
            expression->statement->or_expr.right = build_ast(list_index_get(obj, 2));
        } else if (strcmp(first->value.symbol, QUOTE) == 0) {
            // build object into a quote object first then build_ast with quoted object
            Object *quoted = (Object *)calloc(1, sizeof(Object));
            quoted->type = OBJECT_QUOTE;
            quoted->value.quote = list_index_get(obj, 1);

            expression->type = EXPR_QUOTE;
            expression->statement->quote_expr.value = build_ast(quoted);
        } else if (strcmp(first->value.symbol, VAL) == 0) {
            expression->type = EXPR_DEF;
            Def_Expression *def_expr = (Def_Expression *)calloc(1, sizeof(Def_Expression));
            def_expr->statement =
                (Def_Expression_Statement *)calloc(1, sizeof(Def_Expression_Statement));
            Object *name = list_index_get(obj, 1);
            Object *value = list_index_get(obj, 2);

            // TODO: need to check types
            def_expr->type = EXPR_VAL;
            def_expr->statement->val_expr.name = name->value.symbol;
            def_expr->statement->val_expr.assign_value = build_ast(value);
            expression->statement->def_expr = def_expr;
        } else if (strcmp(first->value.symbol, DEFUN) == 0) {
            expression->type = EXPR_DEF;
            Def_Expression *def_expr = (Def_Expression *)calloc(1, sizeof(Def_Expression));
            def_expr->statement =
                (Def_Expression_Statement *)calloc(1, sizeof(Def_Expression_Statement));
            Object *name = list_index_get(obj, 1);
            Object *args = list_index_get(obj, 2); // TODO: check if args is a list
            Object *body = list_index_get(obj, 3);

            // TODO: parse args into array of symbols

            def_expr->statement->defun_expr.num_args = list_len(args);
            symbol *args_symbols = (symbol *)calloc(def_expr->statement->defun_expr.num_args, sizeof(symbol));
            printf("DEBUG just gonna print arguments list:");
            for (int i = 0; args->type != OBJECT_NIL; i++) {
                Object *first = args->value.pair[0];
                if (first->type != OBJECT_SYMBOL) {
                    fprintf(stderr, "ERROR: expected only symbols within arguments list\n");
                    exit(1);
                }

                args_symbols[i] = first->value.symbol;
                printf(" %s ", args_symbols[i]);
                args = args->value.pair[1];
            }
            printf("\n");

            // TODO: need to check types
            def_expr->type = EXPR_DEFUN;
            def_expr->statement->defun_expr.name = name->value.symbol;
            def_expr->statement->defun_expr.args = args_symbols;
            def_expr->statement->defun_expr.body = build_ast(body);
            expression->statement->def_expr = def_expr;
        } else {
            // TODO: need to check types
            expression->type = EXPR_CALL;
            expression->statement->call_expr.name = first->value.symbol;

            Expression **args = (Expression **)calloc(list_len(obj), sizeof(Expression *));
            Object *args_object = list_index(obj, 1);

            // Build all list elements into array of AST expressions
            int i;
            for (i = 0; args_object->type != OBJECT_NIL; i++) {
                args[i] = build_ast(args_object->value.pair[0]);
                args_object = args_object->value.pair[1];
            }

            expression->statement->call_expr.args = args;
            expression->statement->call_expr.num_args = i;
        }
        break;
    }

    return expression;
}