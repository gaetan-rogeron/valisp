#include <setjmp.h>

#ifndef VALISP_ERREUR_H
#define VALISP_ERREUR_H


void erreur_fatale(char  *fichier, int ligne, char* causes);

void erreur_parseur(char *explication);

jmp_buf *jump_buffer(void);

#define ERREUR_FATALE(CAUSE) erreur_fatale(__FILE__, __LINE__, CAUSE)

#endif
