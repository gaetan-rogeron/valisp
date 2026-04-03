#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "lib_memoire.h"
#include "lib_tests.h"
#include "lib_repl.h"
#include "erreurs.h"
#include "couleurs.h"
#include "parseur.h"
#include "allocateur.h"

#include <readline/readline.h>
#include <readline/history.h>




/******************/
/*                */
/*  Déclarations  */
/*                */
/******************/

/* Les déclarations ci-dessous me permettent d’utiliser du code que
   l’étudiant n’a pas encore déclaré. Cette fonctionnalité ne fait pas
   partie du standart ANSI, mais cela permet de fortement simplifier
   le TP pour les étudiants */

#pragma weak afficher_erreur
void afficher_erreur(void);

#pragma weak car_valisp
sexpr car_valisp(sexpr, sexpr);

#pragma weak cdr_valisp
sexpr cdr_valisp(sexpr, sexpr);

#pragma weak cons_valisp
sexpr cons_valisp(sexpr, sexpr);

#pragma weak add_valisp
sexpr add_valisp(sexpr, sexpr);

#pragma weak sub_valisp
sexpr sub_valisp(sexpr, sexpr);

#pragma weak mul_valisp
sexpr mul_valisp(sexpr, sexpr);

#pragma weak div_valisp
sexpr div_valisp(sexpr, sexpr);

#pragma weak mod_valisp
sexpr mod_valisp(sexpr, sexpr);

#pragma weak less_valisp
sexpr less_valisp(sexpr, sexpr);

#pragma weak equal_valisp
sexpr equal_valisp(sexpr, sexpr);

#pragma weak print_valisp
sexpr print_valisp(sexpr, sexpr);

#pragma weak type_of_valisp
sexpr type_of_valisp(sexpr, sexpr);



/********************/
/*                  */
/*  Le REPL du TP2  */
/*                  */
/********************/

void afficher_banniere_tp2(void) {
    printf("%sVaλisp TP2%s\n\n",
           couleur_vert,
           couleur_defaut);
}



int repl_tp2(void) {
    char* invite_defaut = "\001\033[1;33m\002tp2>\001\033[1;0m\002 ";
    char* invite = invite_defaut;
    char* ligne;
    char BUFFER_READ[10000];
    int POSITION = 0;
    sexpr val;
    sexpr e;
    int res;
    jmp_buf *buf  = jump_buffer();

    afficher_banniere_tp2();
    printf("Initialisation mémoire");
    initialiser_memoire_dynamique();
    printf(" [OK]\n");


    using_history();

    printf("%s@aide%s pour afficher les directives du REPL\n",
           couleur_bleu, couleur_defaut);                              /********/
    while (1) {                                                        /* LOOP */
                                                                       /********/
        ligne = readline(invite);
        if (ligne == NULL) break;
        if (ligne[0] == '\0')  continue ;


        if (!setjmp(*buf)) { /* En cas d’erreur, le code reprend à partir du else */

            /* On gère les directives */
            if (ligne[0] == '@') {
                int numero_tp = 2;
                add_history(ligne);
                lire_directive(ligne, numero_tp);
                free(ligne);
                continue;
            }

            /* La ligne est ajoutée à un buffer pour permettre des
             * expressions sur plusieurs lignes. Un expression
             * incomplète renvoie le code -2 et la prochaine ligne lue
             * sera alors ajoutée à la suite du buffer à l’indice
             * POSITION qui n’aura pas été réinitialisé */

            POSITION = ajout_buffer(BUFFER_READ, POSITION, ligne);
            free(ligne);                                               /********/
            res = valisp_read(BUFFER_READ, &val);                      /* READ */
                                                                       /********/

            /* L’expression est vide, on ignore et on continue */
            if (res == -1) {
                continue;
            }

            /* L’expression n’est pas finie, on refait un tour pour la finir */
            if (res == -2)  {
                invite = "     ";
                continue;
            }

            /* Il n'y pas d’autre code d’erreur négatif possible */
            if (res < 0) {
                char msg[100];
                sprintf(msg, "erreur du parseur invalide [%d]\n", res);
                ERREUR_FATALE(msg);
            }

            /* On sauvegarde l’entrée dans l’historique et on
             * réinitialise le tout */
            supprime_retour_ligne_finale_buffer(BUFFER_READ);
            add_history(BUFFER_READ);
            invite = invite_defaut;
            POSITION=0;

            if (res > 0) {
                erreur_parseur("Pas plus d’une sexpr par ligne");
            }


            /* On a réussi à lire l’expression, on peut l’afficher. Dans
             * le cas où l’expression est de la forme (f a b c) on        ********
             * regarde si f est une des primitives du TP2 afin de         * EVAL *
             * pouvoir la tester. Si l’expression est un symbole          ********
             * correspondant à une primitives on affiche la
             * primitive. Toutes les autres sexpr seront simplement
             * affichées */

            res = 0;
            res |= eval_simple(val, &e,     "car",     car_valisp,     "car_valisp");
            res |= eval_simple(val, &e,     "cdr",     cdr_valisp,     "cdr_valisp");
            res |= eval_simple(val, &e,    "cons",    cons_valisp,    "cons_valisp");
            res |= eval_simple(val, &e,       "+",     add_valisp,     "add_valisp");
            res |= eval_simple(val, &e,       "-",     sub_valisp,     "sub_valisp");
            res |= eval_simple(val, &e,       "*",     mul_valisp,     "mul_valisp");
            res |= eval_simple(val, &e,       "/",     div_valisp,     "div_valisp");
            res |= eval_simple(val, &e,       "%",     mod_valisp,     "mod_valisp");
            res |= eval_simple(val, &e,       "<",    less_valisp,    "less_valisp");
            res |= eval_simple(val, &e,   "print",   print_valisp,   "print_valisp");
            res |= eval_simple(val, &e, "type-of", type_of_valisp, "type_of_valisp");

            printf("%s", couleur_vert);                                  /*********/
            afficher(res ? e : val);                                     /* PRINT */
            printf("%s", couleur_defaut);                                /*********/

        } else {
            /* Si on rencontre une « exception », c’est-à-dire une
               erreur_valisp faisant appelle au longjmp */
            invite = invite_defaut;
            POSITION=0;
            if (afficher_erreur) {
                printf("%s", couleur_rouge);
                afficher_erreur();
                printf("%s", couleur_defaut);
            } else {
                printf("Il faut implémenter la fonction « afficher_erreur »");
            }
        }

        printf("\n\n");
    }

    return 0;
}


int tp2_main(int argc, char *argv[]) {
    cacher_tests();
    test_tp1_obligatoire(); /* On teste uniquement les fonctions jusqu’à malloc */
    montrer_tests();
    test_tp2();
    /* Pour ceux qui souhaite lancer uniquement les tests sans lancer
     * le REPL il suffit de donner un paramètre au programme par exemple :
     * ./valisp --test
     */
    if (argc<2) {
        repl_tp2();
    }
    return 0;
}

