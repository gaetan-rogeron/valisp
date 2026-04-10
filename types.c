#include "types.h"
#include "allocateur.h"
#include "memoire.h"
#include "erreurs.h"
#include <stdio.h> 
#include <string.h>

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
    if (e == NULL){
        printf("nil");
        return;
    }
    
    switch (e->type){
    case entier:
        printf("%d",e->data.INTEGER);
        break;
    case chaine:
        printf("\"%s\"",e->data.STRING);
        break;
    case symbole:
        printf("%s", e->data.STRING);
        break;
    default: 
        printf("<\?\?\?>");
        break;
    }

   
}

sexpr new_integer(int32_t i){
    sexpr obj = (sexpr)valisp_malloc(sizeof(struct valisp_object));
    
    obj->type = entier;
    obj->data.INTEGER = i;
    
    return obj;
}

bool integer_p(sexpr val){
    if (val == NULL) return false;
    return val->type == entier;
}

int32_t get_integer(sexpr val){
    if (!integer_p(val)) {
        ERREUR_FATALE("Erreur de typage : l'objet n'est pas un entier.");
    }
    return val->data.INTEGER;
}

char *chaine_vers_memoire(const char *c) {
    size_t taille;
    char *copie;

    if (c == NULL) return NULL;
    taille = strlen(c) + 1;
    
    copie = (char *)valisp_malloc(taille);

    strcpy(copie, c);
    
    return copie;
}

sexpr new_string(char *c){
    sexpr obj = (sexpr)valisp_malloc(sizeof(struct valisp_object));
    obj->type = chaine;
    
    obj->data.STRING = chaine_vers_memoire(c);
    
    return obj;
}

bool string_p(sexpr val){
    if (val == NULL) return false;
    return val->type == chaine;
}

char *get_string(sexpr val){
     if (!string_p(val)){
        ERREUR_FATALE("Erreur de typage : l'objet n'est pas un string.");
     }
     return val->data.STRING;
}

sexpr new_symbol(char *c){
    sexpr obj;
    if (c == NULL) return NULL;
    if (strcmp(c, "nil") == 0) return NULL;

    obj = (sexpr)valisp_malloc(sizeof(struct valisp_object));

    obj->type = symbole;
    obj->data.STRING = chaine_vers_memoire(c);

    return obj;
}

bool symbol_p(sexpr val){
    if (val == NULL) return true;  
    return val->type == symbole;
}

char *get_symbol(sexpr val) {
    if (val == NULL) return "nil";
    
    if (!symbol_p(val)) {
        ERREUR_FATALE("Erreur de typage : l'objet n'est pas un symbole.");
    }
    return val->data.STRING;
}

bool symbol_match_p(sexpr val, const char *chaine) {
    if (!symbol_p(val)) return false;
    
    return strcmp(get_symbol(val), chaine) == 0;
}

