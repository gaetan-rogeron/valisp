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


/* Cette directive #pragma permet de compiler même si la fonction
   n’est pas encore écrite. Sans cette directive, le fichier tp1.c ne
   pourrait pas compiler avant que l’étudiant n’ait terminé l’exercice
   2 et la fonction initialiser_memoire_dynamique() qui est utilisé
   dans repl_tp1().

   Par contre, si on tente d’exécuter repl_tp1 (et donc
   initialiser_memoire_dynamique) avant, le programme terminera avec
   une erreur de segmentation. */

#pragma weak initialiser_memoire_dynamique
void initialiser_memoire_dynamique(void);


void afficher_banniere_tp1(void) {
    printf("%sVALISP TP1%s\n\n", couleur_vert, couleur_defaut);
}

int repl_tp1(void) {
  char* invite_defaut = "\001\033[1;33m\002tp1>\001\033[1;0m\002 ";
  char* invite = invite_defaut;
  char* ligne;

  afficher_banniere_tp1();
  printf("Initialisation mémoire");
  initialiser_memoire_dynamique();
  printf(" [OK]\n");

  using_history();

  printf("%s@aide%s pour afficher les directives du REPL\n",
         couleur_bleu, couleur_defaut);
  while (1) { /* REPL */
      ligne = readline(invite);

      /* On gère les directives */
      if (ligne == NULL) break;
      if (ligne[0] == '\0')  continue ;

      if (ligne[0] == '@') {
          add_history(ligne);
          lire_directive(ligne, 1); /* Nous sommes au TP 1 */
          free(ligne);
          continue;
      }

      printf("Dans le TP1 seules les directives sont autorisées\n");
      /* Ce n’est pas une directive, c’est donc du vaλisp, mais c’est
         trop tôt pour le tp1. */

      printf("\n");
  }

  return 0;
}


int tp1_main(int argc, char *argv[]) {
    montrer_tests();
    test_tp1();
    repl_tp1();
    return 0;
}
