#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "couleurs.h"
#include "lib_memoire.h"
#include "lib_tests.h"
#include "lib_repl.h"


/******************************************************************/
/*                                                                */
/*  Déclarations des fonctions potentiellements non implémentées  */
/*                                                                */
/******************************************************************/


#pragma weak valisp_stat_memoire
void valisp_stat_memoire (void);

#pragma weak allocateur_balloc
int allocateur_balloc(int);

#pragma weak allocateur_malloc
void *allocateur_malloc(size_t);

#pragma weak valisp_malloc
void *valisp_malloc(size_t);

#pragma weak allocateur_bree
int allocateur_bree(int);

#pragma weak allocateur_free
void *allocateur_free(void*);



#define TESTER_EXISTENCE(FONCTION)					  \
    if (!FONCTION) {							  \
	printf("La fonction « " #FONCTION " » n’est pas implémentée.\n"); \
	return ;							  \
    }



/********************************/
/*                              */
/*  Définitions de directives   */
/*                              */
/********************************/


void afficher_titre_aide(char *titre) {
    printf("%s== %s%s\n",
	   couleur_bleu,
	   titre,
	   couleur_defaut);
}

void afficher_aide(char *directive, char* description) {
    printf("%s%-10s%s %s\n",
	   couleur_vert,
	   directive,
	   couleur_defaut,
	   description);
}

/* @aide */
void directive_aide(int numero_tp) {
    printf("\n");
    afficher_titre_aide("Généralités");
    afficher_aide("@aide", "Affiche cette aide");
    afficher_aide("@q", "Quitte le REPL");
    printf("\n");


    afficher_titre_aide("Images");
    afficher_aide("@save", "sauvegarde une image de la mémoire");
    afficher_aide("@load", "restaure une image de la mémoire");
    printf("\n");


    afficher_titre_aide("Mémoire");
    if (numero_tp>=3) {
        afficher_aide("@stat",  "Affiche des stats sur la mémoire");
    }
    afficher_aide("@mem",  "Affiche la mémoire");
    if (numero_tp>=3) {
    afficher_aide("@env",  "Affiche l’environnement");
    }
    printf("\n");

    afficher_titre_aide("Allocateur");
    afficher_aide("@malloc <nb octets>",  "Alloue <nb octets> dans la mémoire");
    afficher_aide("@free   <pointeurs>",  "Libère le bloc correspondant au pointeur");
    afficher_aide("@balloc <nb cases> ",  "Alloue <nb cases> dans la mémoire");
    afficher_aide("@bree   <indice>   ",  "Libère le bloc correspondant à l’indice");
    if (valisp_malloc) {
        afficher_aide("@valloc <nb octets>",  "Alloue <nb octets> dans la mémoire "
                      "avec valisp_malloc (TP2)");
    }

    printf("\n");


    if (numero_tp>=4) {
        afficher_titre_aide("Ramasse-miettes");
        afficher_aide("@m", "Marque les blocs atteignables "
                      "(1ère étape du ramasse-miettes)");
        afficher_aide("@l", "Libère les blocs non marqués "
                      "(2nd  étape du ramasse-miettes)");
        afficher_aide("@rm","Appelle le ramasse-miette manuellement (@m puis @l)");
        printf("\n");
    }
}

/* @q */
void directive_quitter(void) {
    printf("\nÀ bientôt !\n");
    exit(0);
}

/* @mem */
void directive_stat_memoire(void) {
    TESTER_EXISTENCE(valisp_stat_memoire);
    valisp_stat_memoire();
    printf("\n\n");
}

/* @balloc */
void directive_balloc(int taille) {
    TESTER_EXISTENCE(allocateur_balloc);
    printf("%d",allocateur_balloc(taille));
    printf("\n\n");
}

/* @malloc */
void directive_malloc(size_t size) {
    TESTER_EXISTENCE(allocateur_malloc);
    printf("%p",allocateur_malloc(size));
    printf("\n\n");
}

/* @valloc */
void directive_valloc(size_t size) {
    TESTER_EXISTENCE(valisp_malloc);
    printf("%p",valisp_malloc(size));
    printf("\n\n");
}

/* @free */
void directive_free(void *ptr) {
    TESTER_EXISTENCE(allocateur_free);
    allocateur_free(ptr);
    printf("\n");
}

/* @bree */
void directive_bree(int indice) {
    TESTER_EXISTENCE(allocateur_bree);
    allocateur_bree(indice);
    printf("\n\n");
}



/******************************************************/
/*                                                    */
/*  Lire les directives (commandes commençant par @)  */
/*                                                    */
/******************************************************/


#define SMALL_BUFFER_SIZE  50
int parser_directive(char *ligne, char *commande, char *argument) {
    int suite = 0; /* suite vaut 0 avant d’avoir rencontré ' ' et 1 ensuite*/
    int i = 0;
    int i_com = 0;
    int i_arg = 0;
    char c;
    while ((c=ligne[i++])) {
	if ( i > SMALL_BUFFER_SIZE) return -1;

	if (c==' ' || c ==':') {
            if (suite==1) return -2;
	    suite = 1 ;
	} else if (suite) {
	    argument[i_arg++] = c;
        } else {
	    commande[i_com++] = c;
	}
    }
    commande[i_com] = '\0';
    argument[i_arg] = '\0';
    return 0;
}

int lire_entier_for_repl(char *chaine){
    char* ptr;
    long entier = strtol(chaine,&ptr,10);
    if (ptr[0] != '\0' || entier<0) {
        printf("%sErreur paramètre invalide%s\n", couleur_rouge, couleur_defaut);
        return -1;
    }
    return (int) entier;
}

void *lire_pointeur_for_repl(char *chaine){
    char* ptr;
    long entier;
    if (chaine[0] == '0' && chaine[1] == 'x') {
        chaine++;
        chaine++;
    }

    entier= strtol(chaine,&ptr,16);
    if (ptr[0] != '\0' || entier<0) {
        printf("%sErreur paramètre invalide%s\n", couleur_rouge, couleur_defaut);

        printf("ptr=%s\n",ptr);
        printf("[debug] ptr[0]=%d\n", ptr[0]);
        printf("[debug] entier=%ld\n", entier);
        return NULL;
    }
    return (void*) entier;
}

void lire_directive(char *ligne, int numero_tp) {
    char commande[SMALL_BUFFER_SIZE];
    char argument[SMALL_BUFFER_SIZE];
    char *fichier;
    int err = parser_directive(ligne, commande, argument);
    int entier;
    void *ptr;

    if (err==-1) {
	printf("Directive trop longue, j’abandonne\n\n");
	return;
    }

    if (err==-2) {
	printf("Pas d’espaces autorisés dans les paramètres de la directive\n\n");
	return;
    }

    if (strcmp(commande, "@aide")  == 0) {
	directive_aide(numero_tp);
	return;
    }

    if (strcmp(commande, "@q") == 0) {
	directive_quitter();
        return;
    }

    if (strcmp(commande, "@load") == 0) {
        fichier = (argument[0] == '\0') ? "image.mem" :  argument;
        if (numero_tp == 1) {
            if (-1 == charger_image_simple(fichier)) {
                printf("%sErreur de lecture [%s]%s\n\n",
                       couleur_rouge, strerror(errno), couleur_defaut);
            } else {
                printf("image restaurée depuis le fichier « %s »\n\n", fichier);
            }
        } else {
            if (-1 == charger_image(fichier)) {
                printf("%sErreur de lecture [%s]%s\n\n",
                       couleur_rouge, strerror(errno), couleur_defaut);
            } else {
                printf("image restaurée depuis le fichier « %s »\n\n", fichier);
            }
        }
        return;
    }

    if (strcmp(commande, "@save") == 0) {
        fichier = (argument[0] == '\0') ? "image.mem" :  argument;
	sauver_image(fichier);
        printf("image sauvegardée dans le fichier « %s »\n\n", fichier);
	return;
    }

    if (strcmp(commande, "@stat") == 0) {
	directive_stat_memoire();
	return;
    }

    if (strcmp(commande, "@mem")  == 0) {
        if (numero_tp==1) {
            afficher_memoire_simple();
        } else {
            afficher_memoire();
        }
	return;
    }

    if (strcmp(commande, "@malloc")  == 0) {
        if ((entier= lire_entier_for_repl(argument)) != -1) directive_malloc(entier);
        else printf("Impossible de lire l’entier en argument (trop grand ?)");
	return;
    }

    if (strcmp(commande, "@balloc")  == 0) {
        if ((entier= lire_entier_for_repl(argument)) != -1) directive_balloc(entier);
	return;
    }

    if (strcmp(commande, "@valloc")  == 0) {
        if ((entier= lire_entier_for_repl(argument)) != -1) directive_valloc(entier);
        else printf("Impossible de lire l’entier en argument (trop grand ?)");
	return;
    }

    if (strcmp(commande, "@free")  == 0) {
        if ((ptr= lire_pointeur_for_repl(argument)) != NULL) {
            allocateur_free(ptr);
        }
	return;
    }

    if (strcmp(commande, "@bree")  == 0) {
        if ((entier= lire_entier_for_repl(argument)) != -1) {
            allocateur_bree(entier);
        }
	return;
    }

    printf("Je ne connais pas cette directive\n\n");
}



