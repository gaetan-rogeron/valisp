#ifndef LIB_MEMOIRE_H
#define LIB_MEMOIRE_H
void afficher_stat_memoire(void);
void afficher_stat_memoire_bref(void);
void afficher_memoire(void);
void afficher_memoire_simple(void);

void choisir_repertoire_image_memoire(char* repertoire);
void sauver_image(char *nom_fichier);
int charger_image(char *nom_fichier);
int charger_image_simple(char *nom_fichier);

int comparer_image_et_memoire(char *nom_fichier);
void mem_detail(int indice);
#endif
