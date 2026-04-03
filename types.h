#ifndef VALISP_TYPES_H
#define VALISP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

struct valisp_object;
typedef struct valisp_object* sexpr;

typedef sexpr (*primitive) (sexpr, sexpr);

void afficher(sexpr e);

#endif