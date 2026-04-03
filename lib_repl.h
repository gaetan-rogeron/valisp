#include "types.h"

#ifndef LIB_REPL_H
#define LIB_REPL_H

void lire_directive(char *ligne, int numero_tp);
int valisp_read(char * texte, sexpr* res);
int ajout_buffer(char* buffer, int position, char * chaine);
void supprime_retour_ligne_finale_buffer(char * buffer);

int eval_simple(sexpr e, sexpr *res, char *ch, primitive p, char *nom);
#endif
