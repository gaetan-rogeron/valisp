#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "couleurs.h"




/************************************************/
/*                                              */
/*  Déclaration des fonctions que l’on utilise  */
/*                                              */
/************************************************/

typedef void* (*fonction_binaire) (void*, void*);


#pragma weak taille_bloc
int taille_bloc(int i);

#pragma weak bloc_suivant
int bloc_suivant(int i);

#pragma weak bloc_precedant
int bloc_precedant(int i);

#pragma weak usage_bloc
int usage_bloc(int i);

#pragma weak rm_bloc
int rm_bloc(int i);


extern uint32_t MEMOIRE_DYNAMIQUE[];


#define TAILLE_MEMOIRE_DYNAMIQUE (1<<15)



typedef void* sexpr;


#pragma weak integer_p
int integer_p(sexpr);

#pragma weak get_integer
int32_t get_integer(sexpr);

#pragma weak string_p
int string_p(sexpr val);

#pragma weak symbol_p
int symbol_p(sexpr val);

#pragma weak cons_p
int cons_p (sexpr val);

#pragma weak list_p
int list_p(sexpr val);

#pragma weak closure_p
int closure_p(sexpr val);

#pragma weak prim_p
int prim_p(sexpr val);

#pragma weak spec_p
int spec_p(sexpr val);


#pragma weak get_integer
int32_t get_integer(sexpr  val);

#pragma weak get_string
char * get_string(sexpr  val);

#pragma weak get_symbol
char * get_symbol(sexpr  val);

#pragma weak get_name
char* get_name(sexpr e) ;

#pragma weak get_prim
fonction_binaire get_prim(sexpr e) ;

#pragma weak car
sexpr car(sexpr  val);

#pragma weak cdr
sexpr cdr(sexpr e) ;




/**************************************************************/
/*                                                            */
/*  Trouver le type d’une valeur dans une case de la mémoire  */
/*                                                            */
/**************************************************************/

typedef enum {
  mem_entier ,
  mem_chaine ,
  mem_symbole,
  mem_couple ,
  mem_prim   ,
  mem_spec   ,
  mem_inconnu,
  mem_string
} mem_type;


mem_type trouver_type(int i) {
    unsigned char* hack = (unsigned char*) &MEMOIRE_DYNAMIQUE[i+1] ;
    if (!taille_bloc) return mem_inconnu;

    if (*hack==0 && taille_bloc(i) == 1) return mem_string;

    if (integer_p && integer_p(hack)) return mem_entier;
    if (string_p  && string_p(hack))  return mem_chaine;
    if (symbol_p  && symbol_p(hack))  return mem_symbole;
    if (cons_p    && cons_p(hack))    return mem_couple;
    if (prim_p    && prim_p(hack))    return mem_prim;
    if (spec_p    && spec_p(hack))    return mem_spec;

    if (*hack>8) return mem_string;

    return mem_inconnu;
}





/**********************************************/
/*                                            */
/*  Affichage mémoire : fourni aux étudiants  */
/*                                            */
/**********************************************/


void print_debut_chaine(char* chaine) {
    int i;
    printf("«");
    for (i=0; i<30; i++) {
        if (chaine[i] == '\0') break;
        printf("%c", chaine[i]);
    }
    if (chaine[i] != '\0') printf("…");
    printf("»");
}


void affiche_contenu_bloc(int i) {
    char* hack = (char*) &MEMOIRE_DYNAMIQUE[i+1] ;
    uint32_t *e = (uint32_t*) hack;
    int indice, j;
    uint32_t *p;
    uint32_t *q;

    switch (trouver_type(i)) {

    case mem_entier:
        printf("ENTIER");
        if (get_integer) printf(" = %s%d%s",
                                couleur_vert,
                                get_integer(e),
                                couleur_defaut);
        break;

    case mem_chaine:
        printf("CHAINE");
        if (get_string) {
            p = (uint32_t*) get_string(e);
            indice = (int) (p - MEMOIRE_DYNAMIQUE - 1);
            printf(" → [%d]", indice);
        }
        break;

    case mem_symbole:
        printf("SYMBOLE");
        if (get_symbol) {
            p = (uint32_t*) get_string(e);
            indice = (int) (p - MEMOIRE_DYNAMIQUE - 1);
            printf(" → [%d]", indice);
        }
        break;

    case mem_couple:
        printf("CONS");
        if (car && cdr) {
            p = (uint32_t*) car(e);
            q = (uint32_t*) cdr(e);
            i = (int) (p - MEMOIRE_DYNAMIQUE - 1);
            j = (int) (q - MEMOIRE_DYNAMIQUE - 1);
            printf(" (");
            if (p) printf("%d",i); else printf("NULL");
            printf(" . ");
            if (q) printf("%d",j); else printf("NULL");
            printf(")");
        }
        break;
    case mem_prim:
        printf("PRIMITIVE");
        if (get_name) {
            p = (uint32_t*) get_name(e);
            indice = (int) (p - MEMOIRE_DYNAMIQUE - 1);
            printf(" : %s#p<%d>%s",
                   couleur_bleu,
                   indice,
                   couleur_defaut);
        }
        break;

    case mem_spec:
        printf("SPECIALE");
        if (get_name) {
            p = (uint32_t*) get_name(e);
            indice = (int) (p - MEMOIRE_DYNAMIQUE - 1);
            printf(" : %s#s<%d>%s",
                   couleur_bleu,
                   indice,
                   couleur_defaut);
        }
        break;


    case mem_inconnu:

        printf("%sÉtrange %hu %s",
               couleur_rouge,
               *hack,
               couleur_defaut);
        printf("%s", couleur_jaune);
        print_debut_chaine(hack);
        printf("%s", couleur_defaut);
        break;

    case mem_string:
        printf("%s", couleur_jaune);
        print_debut_chaine(hack);
        printf("%s", couleur_defaut);
        break;

    }
}




void info_bloc(int i) {
    int t = taille_bloc(i);
    int p = bloc_precedant(i);
    int s = bloc_suivant(i);
    char * u = usage_bloc(i) ? "[x]" : "[ ]";
    char * r = rm_bloc(i) ? "*" : " ";

    printf("%5d → [%5d] → %-5d ", p, i, s);
    printf("%s%s", u, r);
    printf(" %p [%5d]", (void *) &MEMOIRE_DYNAMIQUE[i], i);
    printf(" (%u) ", t);
    if (usage_bloc(i) && i!=s) affiche_contenu_bloc(i);
    printf("\n");
}



void info_bloc_simple(int i) {
    int t = taille_bloc(i);
    int p = bloc_precedant(i);
    int s = bloc_suivant(i);
    char * u = usage_bloc(i) ? "[x]" : "[ ]";
    char * r = rm_bloc(i) ? "*" : " ";

    printf("%5d → [%5d] → %-5d ", p, i, s);
    printf("%s%s", u, r);
    printf(" %p [%5d]", (void *) &MEMOIRE_DYNAMIQUE[i], i);
    printf(" (%u) ", t);
    printf("\n");
}


void afficher_memoire(void) {
    int i;
    for (i=0; (MEMOIRE_DYNAMIQUE[i] & 0x7FFF) != i; i = (MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
	info_bloc(i);
    }
    info_bloc(i);
}


void afficher_memoire_simple(void) {
    int i;
    if (!taille_bloc)    goto erreur;
    if (!bloc_precedant) goto erreur;
    if (!bloc_suivant)   goto erreur;
    if (!usage_bloc)     goto erreur;
    if (!rm_bloc)        goto erreur;

    for (i=0; bloc_suivant(i) != i; i = bloc_suivant(i)) {
	info_bloc_simple(i);
    }
    info_bloc_simple(i);
    return;

 erreur:
    printf("Exercice 2 non fini\n");
}


void afficher_stat_memoire(void) {
    size_t i = 0;
    int t = TAILLE_MEMOIRE_DYNAMIQUE;
    int n = 0;
    int b = 0;
    int a = 0;
    for (i=0; (MEMOIRE_DYNAMIQUE[i] & 0x7FFF) != i; i = (MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
        n+=1;
        if (usage_bloc(i)) {
            a += taille_bloc(i) + 1;
            b += 1;
        }
    }
    printf("%d/%d (%.2f %%) %d/%d blocs alloués", a, t, (100.*a)/t, b,n);
}

void afficher_stat_memoire_bref(void) {
    size_t i = 0;
    int t = TAILLE_MEMOIRE_DYNAMIQUE;
    int a = 0;
    for (i=0; (MEMOIRE_DYNAMIQUE[i] & 0x7FFF) != i; i = (MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
        if (usage_bloc(i)) {
            a += taille_bloc(i) + 1;
        }
    }
    printf("%.2f%%", (100.*a)/t);
}




/************************/
/*                      */
/*  Gestion des images  */
/*                      */
/************************/

#define TAILLE_MAX_REPERTOIRE_IMAGE  1024
char REPERTOIRE_IMAGE[TAILLE_MAX_REPERTOIRE_IMAGE] = "images_test/";

void choisir_repertoire_image_memoire(char* repertoire) {
    int i;
    int n = strlen(repertoire);
    if (n > TAILLE_MAX_REPERTOIRE_IMAGE-2) {
        fprintf(stderr, "%sErreur%s Répertoire «%s» trop long\n",
                couleur_rouge, couleur_defaut, repertoire);
    }
    for (i=0; i<n; i++) {
        REPERTOIRE_IMAGE[i] = repertoire[i];
    }
    if (repertoire[n-1] != '/') {
        REPERTOIRE_IMAGE[i++] = '/';
    }
    REPERTOIRE_IMAGE[i] = '\0';
}

char* chemin(char* fichier) {
    static char buffer[1024];
    char * repertoire = REPERTOIRE_IMAGE;
    int i = 0;
    while (*repertoire) buffer[i++] = *(repertoire++);
    while (*fichier) buffer[i++] = *(fichier++);
    buffer[i] = '\0';
    return buffer;
}


typedef enum {entjero,cheno,simbolo,paro,praa,speciala} balisp_type;
typedef struct {uint32_t* unua;uint32_t* dua;} balisp_cons;
typedef struct {char* nomo; void* (*funkcio) (void*,void*);} balisp_prim;
typedef union {int ENTJERO;char* CHENO;balisp_prim PRAA;balisp_cons PARO;} balisp_data;
struct balisp_object {balisp_type tipo;balisp_data datumo;};

#define MAX_PRIMITIVE_NUMBER  64
#define MAX_NAME_PRIMITIVE_LENGTH  64


typedef struct info_primitive {
    fonction_binaire fonction;
    char nom[MAX_NAME_PRIMITIVE_LENGTH+1];
} info_primitive;

info_primitive sauvegarde_primitive(struct balisp_object* prafunkcio) {
    int i;
    info_primitive sauv;
    char *nom = prafunkcio->datumo.PRAA.nomo;
    sauv.fonction = prafunkcio->datumo.PRAA.funkcio;

    for (i=0; nom[i] != '\0'; i++) {
        if (i==MAX_NAME_PRIMITIVE_LENGTH) {
            printf("%s%s%s trop long pour être sauvegardé%s\n",
                   couleur_jaune, nom, couleur_rouge, couleur_defaut);
            return sauv;
        }
        sauv.nom[i] = nom[i];
    }
    sauv.nom[i] = '\0';
    return sauv;
}


uint32_t MEMOIRE_TEMP[TAILLE_MEMOIRE_DYNAMIQUE];
uint32_t MEMOIRE_COPIE[TAILLE_MEMOIRE_DYNAMIQUE];
info_primitive PILE_PRIM[MAX_PRIMITIVE_NUMBER];
int HAUT_PILE_PRIM = 0;


info_primitive sauvegarde_primitive_manuelle(char *nom, fonction_binaire f) {
    int i;
    info_primitive sauv;

    for (i=0; nom[i] != '\0'; i++) {
        if (i==MAX_NAME_PRIMITIVE_LENGTH) {
            printf("%s%s%s trop long pour être sauvegardé%s\n",
                   couleur_jaune, nom, couleur_rouge, couleur_defaut);
            return sauv;
        }
        sauv.nom[i] = nom[i];
    }
    sauv.nom[i] = '\0';
    sauv.fonction = f;
    return sauv;
}

void sauvegarder_une_primitive_manuellement(char *nom, fonction_binaire f) {
    PILE_PRIM[HAUT_PILE_PRIM] = sauvegarde_primitive_manuelle(nom, f);
    HAUT_PILE_PRIM++;
}

void sauvegarder_primitives(void) {
    int i;
    struct balisp_object* e;

    /* Inutile de refaire une sauvegarde si elle a déjà été faîte */
    if (HAUT_PILE_PRIM) return;

    for (i=0; (MEMOIRE_DYNAMIQUE[i] & 0x7FFF) != i; i = (MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
        e = (struct balisp_object*) &MEMOIRE_DYNAMIQUE[i+1];
        if (e->tipo == praa || e->tipo == speciala) {
            PILE_PRIM[HAUT_PILE_PRIM] = sauvegarde_primitive(e);
            HAUT_PILE_PRIM++;
        }
    }
}

fonction_binaire retrouver_primitive(char *nom) {
    int i;
    info_primitive p;

    for (i=0; i<HAUT_PILE_PRIM; i++) {
	p = PILE_PRIM[i];
	if (strcmp(nom, p.nom) == 0) {
	    return p.fonction;
	}
    }
    printf("WARNING, primitive %s non trouvée\n", nom);
    return NULL;
}

size_t sauver_pointeur(void *ptr, int i, char * erreur) {
    uint32_t *p = (uint32_t*) ptr;
    size_t indice = (size_t) (p - MEMOIRE_DYNAMIQUE - 1);
    if (ptr==NULL) return -1;
    if ( 0 <= indice && indice < 1<<15) {
        return indice;
    }
    else {
        fprintf(stderr, "Erreur *ptr %p\n", ptr);
        fprintf(stderr, "       %s [%d]\n",erreur,i);
        return -1;
    }
}

void *restaurer_pointeur(void *p) {
    size_t indice = (size_t) p;
    if (indice == -1) return NULL;
    return (void*) &MEMOIRE_DYNAMIQUE[indice+1];
}

void archiver_contenu_bloc(int i) {
    char* hack = (char*) &MEMOIRE_COPIE[i+1] ;
    struct balisp_object* e = (struct balisp_object*) hack;
    switch (*hack) {

    case entjero:
        break;

    case cheno:
    case simbolo:
	e->datumo.CHENO = (char*) sauver_pointeur(e->datumo.CHENO,i,"CHA");
        break;

    case paro:
	e->datumo.PARO.unua = (uint32_t*) sauver_pointeur(e->datumo.PARO.unua,i,"CAR");
	e->datumo.PARO.dua  = (uint32_t*) sauver_pointeur(e->datumo.PARO.dua,i, "CDR");
	break;

    case praa:
    case speciala:
	e->datumo.PRAA.nomo = (char*) sauver_pointeur(e->datumo.PRAA.nomo,i,"PRA");
	e->datumo.PRAA.funkcio = NULL;
        break;

    case 6:
    case 7:
    case 8:
        printf("%s??? [%i]  %u%s ",couleur_rouge,i, *hack,couleur_defaut);
	printf("%s", couleur_jaune);
        print_debut_chaine(hack);
	printf("%s", couleur_defaut);
	break;

    default:
        break;
    }
}

void restaurer_contenu_bloc(int i) {
    char* hack = (char*) &MEMOIRE_DYNAMIQUE[i+1] ;
    struct balisp_object* e = (struct balisp_object*) hack;

    switch (*hack) {

    case entjero:
        break;

    case cheno:
    case simbolo:
	e->datumo.CHENO = (char*) restaurer_pointeur(e->datumo.CHENO) ;
        break;

    case paro:
	e->datumo.PARO.unua = (uint32_t*) restaurer_pointeur(e->datumo.PARO.unua);
	e->datumo.PARO.dua  = (uint32_t*) restaurer_pointeur(e->datumo.PARO.dua);
	break;

    case praa:
    case speciala:
	e->datumo.PRAA.nomo = (char*) restaurer_pointeur(e->datumo.PRAA.nomo);
	e->datumo.PRAA.funkcio = retrouver_primitive(e->datumo.PRAA.nomo);
        break;

    case 6:
    case 7:
    case 8:
        printf("%sdÉtrange %d%s",
	       couleur_rouge,
	       *hack,
	       couleur_defaut);
	printf("%s", couleur_jaune);
        print_debut_chaine(hack);
	printf("%s", couleur_defaut);
	break;

    default:
        break;
    }
}


void restaurer_contenu_bloc_simple(int i) {
    char* hack = (char*) &MEMOIRE_DYNAMIQUE[i+1] ;
    struct balisp_object* e = (struct balisp_object*) hack;

    switch (*hack) {

    case entjero:
        break;

    case cheno:
    case simbolo:
	e->datumo.CHENO = (char*) restaurer_pointeur(e->datumo.CHENO) ;
        break;

    case paro:
	e->datumo.PARO.unua = (uint32_t*) restaurer_pointeur(e->datumo.PARO.unua);
	e->datumo.PARO.dua  = (uint32_t*) restaurer_pointeur(e->datumo.PARO.dua);
	break;

    case praa:
    case speciala:
	e->datumo.PRAA.nomo = (char*) restaurer_pointeur(e->datumo.PRAA.nomo);
	/* e->datumo.PRAA.funkcio = retrouver_primitive(e->datumo.PRAA.nomo); */
        break;

    case 6:
    case 7:
    case 8:
        printf("%sdÉtrange %d%s",
	       couleur_rouge,
	       *hack,
	       couleur_defaut);
	printf("%s", couleur_jaune);
        print_debut_chaine(hack);
	printf("%s", couleur_defaut);
	break;

    default:
        break;
    }
}





void sauver_image(char *nom_fichier) {
    FILE *fichier;
    int i;
    sauvegarder_primitives();
    memcpy(MEMOIRE_COPIE, MEMOIRE_DYNAMIQUE, sizeof(MEMOIRE_COPIE));
    for (i=0; i != (MEMOIRE_DYNAMIQUE[i] & 0x7FFF); i=(MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
        if (MEMOIRE_COPIE[i] != MEMOIRE_DYNAMIQUE[i]) printf("Warning %d\n",i);
        MEMOIRE_COPIE[i] = MEMOIRE_DYNAMIQUE[i];
        if ((MEMOIRE_DYNAMIQUE[i]>>15) & 1) {
            archiver_contenu_bloc(i);
        }

    }
    fichier = fopen(chemin(nom_fichier), "w");
    fwrite(MEMOIRE_COPIE, sizeof(uint32_t), TAILLE_MEMOIRE_DYNAMIQUE, fichier);
    fclose(fichier);
    printf("Image « %s » sauvegardée\n", chemin(nom_fichier));
}

int charger_image(char *nom_fichier) {
    int i;
    FILE *fichier;
    sauvegarder_primitives();
    fichier =  fopen(chemin(nom_fichier), "r");
    if (fichier == NULL) return -1;
    fread(MEMOIRE_DYNAMIQUE, sizeof(uint32_t), TAILLE_MEMOIRE_DYNAMIQUE, fichier);
    fclose(fichier);
    for (i=0; i != (MEMOIRE_DYNAMIQUE[i]&0x7FFF); i = (MEMOIRE_DYNAMIQUE[i]&0x7FFF)) {
        /* Il ne faut pas restaurer le contenu d’un bloc non
           utilisé. En effet, cela risque de poser soucis lorsque le
           bloc est vide. */
        if ((MEMOIRE_DYNAMIQUE[i]>>15) & 1) {
            restaurer_contenu_bloc(i);
        }
    }
    return 0;
}

int charger_image_simple(char *nom_fichier) {
    int i;
    FILE *fichier;
    sauvegarder_primitives();
    fichier =  fopen(chemin(nom_fichier), "r");
    if (fichier == NULL) return -1;
    fread( MEMOIRE_DYNAMIQUE, sizeof(uint32_t), TAILLE_MEMOIRE_DYNAMIQUE, fichier);
    fclose(fichier);
    for (i=0; i != (MEMOIRE_DYNAMIQUE[i]&0x7FFF); i=(MEMOIRE_DYNAMIQUE[i] & 0x7FFF)) {
     restaurer_contenu_bloc_simple(i);
    }
    return 0;
}


int charger_image_test(char *nom_fichier) {
    FILE *fichier;
    size_t taille =  sizeof(uint32_t)*TAILLE_MEMOIRE_DYNAMIQUE;
    fichier =  fopen(chemin(nom_fichier), "r");
    if (fichier == NULL) return -1;
    memcpy(MEMOIRE_TEMP, MEMOIRE_DYNAMIQUE, taille);
    charger_image(nom_fichier);
    memcpy(MEMOIRE_COPIE, MEMOIRE_DYNAMIQUE, taille);
    memcpy(MEMOIRE_DYNAMIQUE, MEMOIRE_TEMP, taille);
    return 0;
}


/** Renvoie -i lorsque le premier indice où les deux mémoires
    diffèrent est l’indice i et 1 si les deux mémoires sont
    égales */
int comparer_image(void) {
    int i;
    for (i=0; i<TAILLE_MEMOIRE_DYNAMIQUE; i++) {
        if (MEMOIRE_COPIE[i] != MEMOIRE_DYNAMIQUE[i]) {
            return -i;
        }
    }
    return 1;
}

int comparer_image_et_memoire(char *nom_fichier) {
    int i = charger_image_test(nom_fichier);
    if (i==-1) {
        printf("Erreur : Fichier test «%s» inexistant\n", nom_fichier);
        exit(1);
    }
    return comparer_image();
}

