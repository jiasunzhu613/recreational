#include "object_helpers.h"

Object *obj_box(Object_Type type) {
    Object *obj = (Object *)calloc(1, sizeof(Object));
    obj->type = type;

    return obj;
}

Object *pool_put(Object **pool, symbol sym) {
    // always pair up
    Object *entry = (Object *)calloc(1, sizeof(Object));
    entry->type = OBJECT_SYMBOL;
    entry->value.symbol = sym;

    Object *new_pool = (Object *)calloc(1, sizeof(Object));
    new_pool->type = OBJECT_PAIR;
    new_pool->value.pair[0] = entry;
    new_pool->value.pair[1] = *pool;

    print_sexpression(new_pool);

    *pool = new_pool;
    return entry;
}

Object *pool_search(Object **pool, symbol sym) {
    Object *p = *pool;

    while (p->type != OBJECT_NIL) {
        Object *entry =
            p->value.pair[0]; // TODO: might need to validate this, this should be a entry though
        symbol entry_sym = entry->value.symbol; // get symbol from entry

        if (strcmp(entry_sym, sym) == 0) {
            return entry;
        }

        p = p->value.pair[1];
    }

    printf("DEBUG: DID NOT FIND SHIT IN POOL_SEARCH\n");
    return NULL; // TODO: decide what to do here?
}

bool is_object_list(Object *obj) {
    if (obj->type != OBJECT_PAIR) {
        return false;
    }

    assert(obj->value.pair[0] != NULL && obj->value.pair[1] != NULL);
    if (obj->value.pair[0] != NULL && obj->value.pair[1]->type == OBJECT_NIL) {
        return true;
    }

    return is_object_list(obj->value.pair[1]);
}

int list_len(Object *list) {
    int count = 0;
    while (list->value.pair[1]->type != OBJECT_NIL) {
        count++;
        list = list->value.pair[1];
    }

    return count + 1;
}

Object *list_index(Object *list, int ind) {
    while (ind > 0 && list->type != OBJECT_NIL) {
        ind--;
        list = list->value.pair[1];
    }

    if (ind > 0) {
        return NULL;
    }

    return list;
}

Object *list_index_get(Object *list, int ind) { return list_index(list, ind)->value.pair[0]; }

void print_list(Object *obj) {
    // Guarantee the object coming in is of list like structure initially
    // Being OBJECT_PAIR is baseline requirement
    assert(obj->type == OBJECT_PAIR);

    while (obj->value.pair[1]->type != OBJECT_NIL) {
        print_sexpression(obj->value.pair[0]);
        printf(" ");
        obj = obj->value.pair[1];
    }
    print_sexpression(obj->value.pair[0]);
}

void print_sexpression(Object *obj) {
    // TODO: this should change
    if (obj == NULL) {
        return;
    }

    switch (obj->type) {
    case OBJECT_FIXNUM:
        printf("%lld", obj->value.fixnum);
        break;
    case OBJECT_BOOLEAN:
        printf("#%c", obj->value.boolean ? 't' : 'f');
        break;
    case OBJECT_SYMBOL:
        printf("%s", obj->value.symbol);
        break;
    case OBJECT_NIL:
        printf("nil");
        break;
    case OBJECT_QUOTE:
        print_sexpression(obj->value.quote);
        break;
    case OBJECT_PAIR:
        // Check if pair construction looks like list construction
        if (is_object_list(obj)) {
            printf("(");
            print_list(obj);
            printf(")");
        } else { // Print in pair syntax instead
            printf("(");
            print_sexpression(obj->value.pair[0]);
            printf(" . ");
            print_sexpression(obj->value.pair[1]);
            printf(")");
        }

        break;
    }
}
