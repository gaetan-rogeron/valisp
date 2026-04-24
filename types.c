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
    case couple:
        printf("(");
        afficher_liste(e);
        printf(")");
    case prim:
        printf("#p<%s>", e->data.PRIMITIVE.nom);
        break;
    case spec:
        printf("#s<%s>", e->data.PRIMITIVE.nom);
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

sexpr cons(sexpr e1, sexpr e2) {
    sexpr obj = (sexpr)valisp_malloc(sizeof(struct valisp_object));
    obj->type = couple;
    obj->data.CONS.car = e1;
    obj->data.CONS.cdr = e2;
    return obj;
}

bool cons_p(sexpr e) {
    if (e == NULL) return false;
    return e->type == couple;
}

sexpr car(sexpr e){
    if (e == NULL) ERREUR_FATALE("car applique sur nil");
    if (!cons_p(e)) ERREUR_FATALE("car appliqué sur un objet qui n'est pas un cons.");
    return e->data.CONS.car;
}

sexpr cdr(sexpr e){
    if (e == NULL) ERREUR_FATALE("cdr appliqué sur nil (liste vide).");
    if (!cons_p(e)) ERREUR_FATALE("cdr appliqué sur un objet qui n'est pas un cons.");
    return e->data.CONS.cdr;
}

void set_car(sexpr e, sexpr nouvelle) {
    if (e == NULL) ERREUR_FATALE("set_car appliqué sur nil.");
    if (!cons_p(e)) ERREUR_FATALE("set_car appliqué sur un objet qui n'est pas un cons.");
    e->data.CONS.car = nouvelle;
}

void set_cdr(sexpr e, sexpr nouvelle) {
    if (e == NULL) ERREUR_FATALE("set_cdr appliqué sur nil.");
    if (!cons_p(e)) ERREUR_FATALE("set_cdr appliqué sur un objet qui n'est pas un cons.");
    e->data.CONS.cdr = nouvelle;
}

void afficher_liste(sexpr e) {
    sexpr y;
    afficher(car(e));
    
    y = cdr(e);
    
    if (y == NULL) {
        return; 
    } else if (cons_p(y)) {
        printf(" ");
        afficher_liste(y);
    } else {
        printf(" . ");
        afficher(y);
    }
}

sexpr new_primitive(char *nom, primitive p){
    sexpr res = valisp_malloc(sizeof(struct valisp_object));
    res->type = prim;
    res->data.PRIMITIVE.nom = chaine_vers_memoire(nom);
    res->data.PRIMITIVE.f = p;
    return res;
}

sexpr new_speciale(char *nom, primitive p) {
    sexpr res = (sexpr)valisp_malloc(sizeof(struct valisp_object));
    res->type = spec;
    res->data.PRIMITIVE.nom = chaine_vers_memoire(nom);
    res->data.PRIMITIVE.f = p;
    return res;
}

bool prim_p(sexpr val) {
    if (val == NULL) return false;
    return val->type == prim;
}

bool spec_p(sexpr val) {
    if (val == NULL) return false;
    return val->type == spec;
}

char* get_name(sexpr p) {
    if (!prim_p(p) && !spec_p(p)) {
        ERREUR_FATALE("Le type n'est ni une primitive ni une forme speciale");
    }
    return p->data.PRIMITIVE.nom;
}

primitive get_prim(sexpr p) {
    if (!prim_p(p) && !spec_p(p)) {
        ERREUR_FATALE("Le type est ni une primitive ni une forme speciale");
    }
    return p->data.PRIMITIVE.f;
}

sexpr run_prim(sexpr p, sexpr liste, sexpr env) {
    primitive func = get_prim(p);
    return func(liste, env);
}

bool sexpr_equal(sexpr e1, sexpr e2){
    if (e1 == NULL && e2 == NULL) {
        return true;
    }
    if (e1 == NULL || e2 == NULL){
        return false;
    }
    if (e1->type != e2->type){
        return false;
    }

    switch (e1->type) {
        case entier:
            return e1->data.INTEGER == e2->data.INTEGER;
        case chaine:
        case symbole:
            return strcmp(e1->data.STRING, e2->data.STRING) == 0;
        case couple:
            return sexpr_equal(car(e1), car(e2)) && sexpr_equal(cdr(e1), cdr(e2));
        case prim:
        case spec:
            return e1->data.PRIMITIVE.f == e2->data.PRIMITIVE.f;
        default:
            return false;
    }