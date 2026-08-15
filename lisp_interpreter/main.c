#include "repl.h"

// TODO: right now there are for sure memory leaks because we dont free mallocs for list and symbol
int main() {
    char *buffer = NULL;
    size_t size = 0;

    // TODO: update these to hashmaps or something better later!
    // TODO: maybe add pool into Env struct?
    Env *env = (Env *)calloc(1, sizeof(Env));
    env->vars = (Object *)calloc(1, sizeof(Object));
    env->vars->type = OBJECT_NIL;
    env->vars->value.nil = NULL;
    env->funcs = (Object *)calloc(1, sizeof(Object));
    env->funcs->type = OBJECT_NIL;
    env->funcs->value.nil = NULL;

    Object *pool = (Object *)calloc(1, sizeof(Object));
    pool->type = OBJECT_NIL;
    pool->value.nil = NULL;

    while (1) {
        printf(">>> "); // Print prompt

        ssize_t num_chars = getline(&buffer, &size, stdin);

        // Exit on ^D or EOF
        if (num_chars == -1) {
            printf("\n");
            return 0;
        }

        int error = eval(buffer, env, &pool);
        if (error) {
            return error;
        }
        // Make sure to free heap memory allocated for buffer
        free(buffer);
        buffer = NULL;
    }
    return 0;
}