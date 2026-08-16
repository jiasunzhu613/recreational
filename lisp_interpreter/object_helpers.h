#ifndef OBJECT_HELPERS_H
#define OBJECT_HELPERS_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "object.h"

Object *obj_box(Object_Type type);
// Intern object pooling
// TODO: try to merge intern pool and environment functions into one common interface
// TODO: maybe change pool object too?
Object *pool_put(Object **pool, symbol sym);
Object *pool_search(Object **pool, symbol sym);

// List-specific Helpers
bool is_object_list(Object *obj);
int list_len(Object *list);
Object *list_index(Object *list, int ind);
Object *list_index_get(Object *list, int ind);

// Printing
void print_list(Object *obj);
void print_sexpression(Object *obj);

#endif // OBJECT_HELPERS_H