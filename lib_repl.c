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
#include "parseur.h"
#include "types.h"
#include "erreurs.h"


/******************************************************************/
/*                                                                */
/*  Déclarations des fonctions potentiellements non implémentées  */
/*                                                                */
/******************************************************************/


#pragma weak valisp_stat_memoire
void valisp_stat_memoire (void);

#pragma weak afficher_env
void afficher_env(sexpr env);

#pragma weak environnement_global
sexpr environnement_global(void);

#pragma weak ramasse_miettes_parcourir_et_marquer
void ramasse_miettes_parcourir_et_marquer (sexpr env);

#pragma weak ramasse_miettes_liberer
void ramasse_miettes_liberer (sexpr env);

#pragma weak valisp_ramasse_miettes
void valisp_ramasse_miettes (sexpr env);

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


    if (numero_tp>=3) {
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

/* @env */
void directive_afficher_environnement(void) {
    TESTER_EXISTENCE(environnement_global);
    TESTER_EXISTENCE(afficher_env);
    afficher_env(environnement_global());
    printf("\n\n");
}

/* @rm */
void directive_ramasse_miette(void) {
    TESTER_EXISTENCE(environnement_global);
    TESTER_EXISTENCE(valisp_ramasse_miettes);
    TESTER_EXISTENCE(valisp_stat_memoire);
    valisp_ramasse_miettes(environnement_global());
    valisp_stat_memoire();
    printf("\n\n");
}

/* @m */
void directive_ramasse_miettes_marquer(void) {
    TESTER_EXISTENCE(environnement_global);
    TESTER_EXISTENCE(ramasse_miettes_parcourir_et_marquer);
    ramasse_miettes_parcourir_et_marquer(environnement_global());
    afficher_memoire();
}

/* @l */
void directive_ramasse_miettes_liberer(void) {
    TESTER_EXISTENCE(environnement_global);
    TESTER_EXISTENCE(ramasse_miettes_liberer);
    ramasse_miettes_liberer(environnement_global());
    afficher_memoire();
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

    if (strcmp(commande, "@env") == 0) {
        directive_afficher_environnement();
	return;
    }

    if (strcmp(commande, "@rm") == 0) {
        directive_ramasse_miette();
        return;
    }

    if (strcmp(commande, "@m")  == 0) {
        directive_ramasse_miettes_marquer();
        return;
    }

    if (strcmp(commande, "@l")  == 0) {
        directive_ramasse_miettes_liberer();
        return;
    }
    printf("Je ne connais pas cette directive\n\n");
}



/******************/
/*                */
/*  Utilitaires   */
/*                */
/******************/

int valisp_read(char * texte, sexpr* res) {
    int i;
    char *message_erreur = "";
    *res = NULL;
    i = parseur(texte, 0, res);
    i = nettoyer_espaces(texte,i);
    if (i>0 && texte[i] == '\0') return 0;
    if (i>=0) return i;

    switch (i) {
    case ERREUR_PARSEUR_VIDE:
    case ERREUR_PARSEUR_INCOMPLET:
	return i;

    case ERREUR_PARSEUR_ENTIER:
	message_erreur = "entier invalide";
	break;

    case ERREUR_PARSEUR_CHAINE:
	message_erreur = "chaîne invalide";
	break;

    case ERREUR_PARSEUR_PARENTHESE:
	message_erreur = "parenthèse fermante orpheline";
	break;

    case ERREUR_PARSEUR_POINT:
	message_erreur = "séparateur de cons «.» en position non finale";
	break;

    case ERREUR_PARSEUR_IMPLEMENTATION:
	message_erreur = "fonction non implémentée";
	break;

    default:
	fprintf(stderr, ">>>>> %d\n", i);
	ERREUR_FATALE("Code d’erreur du parseur invalide");
    }
    erreur_parseur(message_erreur);
    return 0; /* Pour faire plaisir au compilateur*/
}


int ajout_buffer(char* buffer, int position, char * chaine) {
  int i;
  for (i=0; chaine[i] != '\0'; i++) {
    buffer[position+i] = chaine[i];
  }
  buffer[position+i] = '\n';
  buffer[position+i+1] = '\0';
  return position+i+1;
}

void supprime_retour_ligne_finale_buffer(char * buffer) {
  int i;
  for (i=0; buffer[i] != '\0'; i++);
  if (buffer[i-1] == '\n') buffer[i-1] = '\0';
}



#pragma weak cons_p
int cons_p(sexpr);

#pragma weak car
sexpr car(sexpr);

#pragma weak cdr
sexpr cdr(sexpr);

#pragma weak run_prim
sexpr run_prim(sexpr, sexpr, sexpr);

#pragma weak symbol_match_p
bool symbol_match_p(sexpr, const char*);

#pragma weak symbol_p
bool symbol_p(sexpr);

#pragma weak new_primitive
sexpr new_primitive(char*, primitive);


int eval_simple(sexpr e, sexpr *res, char *ch, primitive p, char *nom) {
    if (cons_p == NULL) return 0;
    if (car == NULL) return 0;
    if (cdr == NULL) return 0;
    if (run_prim == NULL) return 0;
    if (symbol_match_p == NULL) return 0;
    if (symbol_p == NULL) return 0;
    if (symbol_p(e) && symbol_match_p(e, ch)) {
        if (p) {
            sexpr pr = new_primitive(nom, p);
            *res = pr;
            return 1;
        } else {
            printf("%sFonction «%s» non implémentée\n%s",
                   couleur_rouge, nom, couleur_defaut);
            return 0;
        }

    }
    else if (cons_p(e) && symbol_p(car(e)) && symbol_match_p(car(e), ch)) {
        if (p) {
            sexpr pr = new_primitive(nom, p);
            *res = run_prim(pr, cdr(e), NULL);
            return 1;
        } else {
            printf("%sFonction «%s» non implémentée\n%s",
                   couleur_rouge, nom, couleur_defaut);
            return 0;
        }

    } else {
        return 0;
    }
}
