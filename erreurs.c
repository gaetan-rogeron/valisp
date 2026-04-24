#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "couleurs.h"
#include "erreurs.h"


/**********************/
/*                    */
/*  Erreurs Fatales   */
/*                    */
/**********************/

/* Pour s’arrêter lorsque le développeur tombe sur cas qui ne devrait
   jamais arriver. Normalement, si le programme est écrit sans bugs,
   les erreurs fatales ne devrait jamais être rencontrées.

   La variable TEST_ERREUR_FATALE permet l’écriture de test pour
   intercepter l’erreur lors des tests
*/

int TEST_ERREUR_FATALE = 0;


void erreur_fatale(char  *fichier, int ligne, char *causes) {
    if (TEST_ERREUR_FATALE--) return;
    fprintf(stderr,"%s", couleur_rouge);
    fprintf(stderr,"\n   /!\\ Erreur fatale /!\\");
    fprintf(stderr,"%s", couleur_defaut);
    fprintf(stderr,"   %s ligne %d\n\n", fichier, ligne);
    fprintf(stderr,"%s\n", causes);
    exit(1);
}



/**************************/
/*                        */
/*  Gestion du long jump  */
/*                        */
/**************************/

/* Merci de lire le cours pour comprendre ce que fait le type
   jmp_buf. Rapidement, c’est une structure qui sauvegarde l’état du
   programme que l’on pourra restaurer en cas d’erreur. On l’utilisera
   lors de l’exercice 11 */

jmp_buf buf;

jmp_buf *jump_buffer(void) {
    return &buf;
}





/************************/
/*                      */
/*  Erreur du parseur   */
/*                      */
/************************/

void erreur_parseur(char *explication) {
    fprintf(stderr, "%sErreur parseur%s\n", couleur_rouge, couleur_defaut);
    exit(1);
    /*   erreur(SYNTAXE,"parseur vaλisp", explication, NULL); */

}


sexpr SEXPR_ERREUR = NULL;
char *FONCTION_ERREUR = NULL;
char *MESSAGE_ERREUR = NULL;
enum erreurs TYPE_ERREUR = RUNTIME;

const char* nom_erreurs[] = {
    "TYPAGE", "ARITE", "NOM", "MEMOIRE", 
    "DIVISION_PAR_ZERO", "SYNTAXE", "MEMOIRE_PARSEUR", "RUNTIME"
};

void afficher_erreur(void) {
    printf("\033[31m"); 
    
    printf("Erreur d'exécution [%s] : %s\n", nom_erreurs[TYPE_ERREUR], MESSAGE_ERREUR);
    
    if (FONCTION_ERREUR != NULL) {
        printf("Fonction fautive : <%s>\n", FONCTION_ERREUR);
    }
    
    if (SEXPR_ERREUR != NULL) {
        printf("Valeur fautive : <");
        afficher(SEXPR_ERREUR);
        printf(">\n");
    }
    
    printf("\033[0m");
}

void erreur(enum erreurs type, char *fonction, char *explication, sexpr s) {
    TYPE_ERREUR = type;
    FONCTION_ERREUR = fonction;
    MESSAGE_ERREUR = explication;
    SEXPR_ERREUR = s;
    
    longjmp(*jump_buffer(), 1); 
}