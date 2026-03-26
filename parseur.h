#include "types.h"

#ifndef PARSEUR_H
#define PARSEUR_H

#define ERREUR_PARSEUR_VIDE           -1
#define ERREUR_PARSEUR_INCOMPLET      -2
#define ERREUR_PARSEUR_ENTIER         -3
#define ERREUR_PARSEUR_CHAINE         -4
#define ERREUR_PARSEUR_PARENTHESE     -5
#define ERREUR_PARSEUR_POINT          -6
#define ERREUR_PARSEUR_IMPLEMENTATION -7

int nettoyer_espaces(char * texte, int i);
int parseur(char* texte, int i,  sexpr* res);
#endif
