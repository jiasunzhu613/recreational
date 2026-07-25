#include "repl.h"

int main() {
    // Object *env = (Object*)calloc(1, sizeof(Object));
    // env->type = OBJECT_NIL;
    // env->value.nil = NULL;
    
    // Object key = {};
    // key.type = OBJECT_SYMBOL;
    // key.value.symbol = "x";

    // Object val = {};
    // val.type = OBJECT_FIXNUM;
    // val.value.fixnum = 1000;

    // env = env_put(env, &key, &val);
    // Object *val_found = env_search(env, &key);
    // if (val_found == NULL) {
    //     printf("key %s not found\n", key.value.symbol);
    // }
    // print_sexpression(val_found);

    char *buffer = "(1 2 3)";
    Object obj;

    buffer = parse_sexpression(&obj, buffer);
    print_sexpression(&obj);

    Object *indexed = list_index(&obj, 10);
    if (indexed == NULL) {
        printf("indexed too far\n");
    }
    print_sexpression(indexed);

    printf("Received list length: %d\n", list_len(&obj));

    return 0;
}