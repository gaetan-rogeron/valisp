#include "types.h"
#include "allocateur.h"

enum valisp_type {entier, chaine, symbole, couple, prim, spec};

struct cons{
    sexpr car;
    sexpr cdr;
};

struct prim {
    char *nom;
    primitive f;
};


union valisp_data{
    int32_t INTEGER;
    char *STRING;
    struct cons CONS;
    struct prim PRIMITIVE;
};

struct valisp_object {
    enum valisp_type type;
    union valisp_data data;
};

void afficher(sexpr e){
    if (e== NULL){
        printf("nil");
    }else {
        printf("<???>");
    }
}