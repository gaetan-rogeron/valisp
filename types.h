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

sexpr new_symbol(char *c);
bool symbol_p(sexpr val);
char *get_symbol(sexpr val);
bool symbol_match_p(sexpr val, const char *chaine);

sexpr cons(sexpr e1, sexpr e2);
bool cons_p(sexpr e);
sexpr car(sexpr e);
sexpr cdr(sexpr e);
void set_car(sexpr e, sexpr nouvelle);
void set_cdr(sexpr e, sexpr nouvelle);
void afficher_liste(sexpr e);
void afficher(sexpr e);

sexpr new_primitive(char *nom, primitive p);
sexpr new_speciale(char *nom, primitive p);
bool prim_p(sexpr val);
bool spec_p(sexpr val);
char* get_name(sexpr p);
primitive get_prim(sexpr p);
sexpr run_prim(sexpr p, sexpr liste, sexpr env);

bool sexpr_equal(sexpr e1, sexpr e2);

#endif