#include <setjmp.h>
#include "types.h"

#ifndef VALISP_ERREUR_H
#define VALISP_ERREUR_H


void erreur_fatale(char  *fichier, int ligne, char* causes);

void erreur_parseur(char *explication);

jmp_buf *jump_buffer(void);

enum erreurs {
    TYPAGE,
    ARITE,
    NOM,
    MEMOIRE,
    DIVISION_PAR_ZERO,
    SYNTAXE,
    MEMOIRE_PARSEUR,
    RUNTIME
};

extern sexpr SEXPR_ERREUR;
extern char *FONCTION_ERREUR;
extern char *MESSAGE_ERREUR;
extern enum erreurs TYPE_ERREUR;

void afficher_erreur(void);
void erreur(enum erreurs type, char *fonction, char *explication, sexpr s);

#define ERREUR_FATALE(CAUSE) erreur_fatale(__FILE__, __LINE__, CAUSE)

#endif
