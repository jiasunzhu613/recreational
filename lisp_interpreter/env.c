#include "env.h"

Env *create_env() {
    Env *env = (Env *)calloc(1, sizeof(Env));
    env->vars = (Object *)calloc(1, sizeof(Object));
    env->vars->type = OBJECT_NIL;
    env->vars->value.nil = NULL;
    env->funcs = NULL;
    
    env->size = 0;
    env->capacity = 0;

    return env;
}

bool env_put(Env *env, Object *key, Object *value) {
    // always pair up
    Object *entry = (Object *)calloc(1, sizeof(Object));
    entry->type = OBJECT_PAIR;
    entry->value.pair[0] = key;
    entry->value.pair[1] = value;

    Object *new_vars_env = (Object *)calloc(1, sizeof(Object));
    new_vars_env->type = OBJECT_PAIR;
    new_vars_env->value.pair[0] = entry;
    new_vars_env->value.pair[1] = env->vars;

    print_sexpression(new_vars_env);

    env->vars = new_vars_env;
    return true;
}

Object *env_search(Env *env, symbol key) {
    // TODO: need some way to compare the value of key which will be a symbol and what is in the
    // env, compare strings, strcmp key should never be a non-symbol
    Object *p = env->vars;

    while (p->type != OBJECT_NIL) {
        Object *entry =
            p->value.pair[0]; // TODO: might need to validate this, this should be a entry though
        Object *entry_key = entry->value.pair[0]; // get first elem from entry pair
        // printf("DEBUG: on entry\n");
        // print_sexpression(entry);
        // printf("\n");
        if (entry_key->type != OBJECT_SYMBOL) {
            fprintf(stderr, "CRITICAL: environment entry has non-symbol key\n");
            exit(1);
        }

        if (strcmp(entry_key->value.symbol, key) == 0) {
            // printf("DEBUG: FOUND MATCHING KEYS\n");
            // printf("DEBUG: RETURING ");
            // print_sexpression(entry->value.pair[1]);
            // printf("\n");
            return entry->value.pair[1];
        }

        p = p->value.pair[1];
    }

    printf("DEBUG: DID NOT FIND SHIT IN ENV_SEARCH\n");
    return NULL; // TODO: decide what to do here?
}
