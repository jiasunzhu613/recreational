#ifndef ENV_H
#define ENV_H

#include "object.h"
#include "ast.h"

// Environment
typedef struct Env Env;

// Helper definition to append values to a dynamic array, realloc'ing memory if needed
#define env_funcs_append(env, val) \
    do { \
        if (env->size >= env->capacity) { \
            if (env->capacity == 0) { \
                env->capacity = 256; \
            } else { \
                env->capacity *= 2; \
            } \
            env->funcs = realloc(env->funcs, env->capacity * sizeof(struct Defun)); \
        } \
        env->funcs[env->size] = val; \
        env->size++; \
    } while (0)

struct Env {
    // for variables
    Object *vars;

    // for functions
    Defun *funcs; // TODO: should this be Defun?
    int size;
    int capacity;
};

Env *create_env(); // TODO: maybe use for creating new envs for functions to use
bool env_put(Env *env, Object *key,
             Object *value);                  // then we must create entry out of key and value
Object *env_search(Env *env, symbol key); // returns Object* which represents the value in env

#endif // ENV_H