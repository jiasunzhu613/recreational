#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "object.h"
#include "object_helpers.h"

char *trim_whitespace(char *buffer);
bool is_whitespace(char *buffer);
char *parse_fixnum(Object *obj, char *p);
char *parse_boolean(Object *obj, char *p);
char *parse_symbol(Object **obj, char *p, Object **pool);
char *parse_pair(Object *obj, char *p, Object **pool);
char *parse_sexpression(Object **obj, char *buffer, Object **pool);

#endif // PARSER_H