#ifndef VALISP_TYPES_H
#define VALISP_TYPES_H

#include <stdint.h>

#undef bool
#undef true
#undef false
typedef int bool;
#define true 1
#define false 0
struct valisp_object;
typedef struct valisp_object* sexpr;

typedef sexpr (*primitive) (sexpr, sexpr);

void afficher(sexpr e);

sexpr new_integer(int32_t i);
bool integer_p(sexpr val);
int32_t get_integer(sexpr val);

char *chaine_vers_memoire(const char *c);
sexpr new_string(char *c);
bool string_p(sexpr val);
char *get_string(sexpr val);

#endif