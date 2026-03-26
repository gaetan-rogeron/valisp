#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include "couleurs.h"
#include "lib_memoire.h"





/**********************/
/*                    */
/*  Niveau de debug   */
/*                    */
/**********************/

enum debug_level {HIDE, SHOW};
enum debug_level DEBUG_LEVEL = HIDE;

int cacher_tests(void) {
    int  sauv = DEBUG_LEVEL;
    DEBUG_LEVEL = HIDE;
    return sauv;
}


void restaurer_tests(int i) {
    DEBUG_LEVEL = i;
}


void montrer_tests(void) {
    DEBUG_LEVEL = SHOW;
}



void printf_test(const char *fmt, ...) {
    int n;
    va_list fmtargs;
    char buffer[1024];

    /* Determine required size. */

    va_start(fmtargs, fmt);
    n = vsprintf(buffer, fmt, fmtargs);
    va_end(fmtargs);

    if (n < 0)
        return;

    if (n>1000) {
        fprintf(stderr, "Taille nécessaire : %d\n", n);
        fprintf(stderr, "Taille réelle     : %d\n", 1024);
        fprintf(stderr, "Chaîne trop longue : merci d’appeler M. Baldellon\n.");
        exit(1);
    }

    if (DEBUG_LEVEL==SHOW) {
        printf("%s", buffer);
        fflush(stdout);
    }
}



/**************************/
/*                        */
/*  Affichage des titres  */
/*                        */
/**************************/

/* strlen mais avec gestions à l’arrache des accents */
int longueur(char *chaine) {
    int i = 0;
    int n = 0;
    unsigned char c;
    while ( (c=chaine[i]) != '\0') {
        if (c>127) i++; /* Non ascii => accent => lettre sur 2 octets*/
        n++;
        i++;
    }
    return n;
}

void afficher_titre(char *titre) {
    int n = longueur(titre);
    int i;
    printf_test("\n   +");
    for (i=0; i<n+2; i++) printf("-");
    printf_test("+\n");
    printf_test("   | %s |\n", titre);
    printf_test("   +");
    for (i=0; i<n+2; i++) printf("-");
    printf_test("+\n\n");
}




/******************************/
/*                            */
/*  Pour enchaîner les tests  */
/*                            */
/******************************/

char* NOM_FONCTION_TESTEE;


int run_test(int boolean, char *description, char* fichier, int line) {
    if (!boolean) {
	if (DEBUG_LEVEL==SHOW) {
	    printf("%s[ÉCHEC]%s\n", couleur_rouge, couleur_defaut);
	    printf("\n%s%s%s ", couleur_jaune, NOM_FONCTION_TESTEE, couleur_defaut);
	    printf("%s ligne %d\n", fichier, line);
	    printf("%s\n", description);
	} else {
            printf("%s[ÉCHEC]%s\n", couleur_rouge, couleur_defaut);
	    printf("\n%s%s%s ", couleur_jaune, NOM_FONCTION_TESTEE, couleur_defaut);
	    printf("%s ligne %d\n", fichier, line);
	    printf("%s\n", description);
	}
    }
    return boolean;
}


#define RUN_TEST(BOOL) run_test(BOOL, #BOOL, __FILE__, __LINE__)


int test_une_fonction(char *nom_fonc, int exist_p, int (*test_fonc)(void)) {
    NOM_FONCTION_TESTEE = nom_fonc;
    printf_test("%s %*s ", nom_fonc,  30 - (int) strlen(nom_fonc), "");
    if (exist_p) {
	if (test_fonc()) {
	    printf_test("%s[OK]%s\n", couleur_vert, couleur_defaut);
            return 1;
        } else {
            return 0;
        }
    } else {
	printf("%s[ÉCHEC]%s\n%s%s%s Fonction inconnue\n\n",
               couleur_rouge, couleur_defaut,
               couleur_jaune, nom_fonc, couleur_defaut);
	return 0;
    }
}

#define TEST_FONCTION(FONC) test_une_fonction(#FONC, FONC ? 1 : 0, run_test_ ## FONC)






/****************************************/
/*                                      */
/*  Pour la gestion des images mémoire  */
/*                                      */
/****************************************/

extern uint32_t MEMOIRE_DYNAMIQUE[];
extern uint32_t MEMOIRE_COPIE[];


void reset_memoire(void) {
    int i;
    int n = 1<<15;
    for (i=0 ; i<n ; i++) {
        MEMOIRE_DYNAMIQUE[i] = 0;
    }
}

#pragma weak initialiser_memoire_dynamique
void initialiser_memoire_dynamique(void);

#pragma weak allocateur_balloc
int  allocateur_balloc(int taille);

#pragma weak allocateur_malloc
void  *allocateur_malloc(size_t size);


#pragma weak allocateur_bree
void allocateur_bree(int indice);

#pragma weak allocateur_free
void allocateur_free(void *ptr);




struct scenario_quelconque {
    int n;
    int indices[10];
    char *chaines[10];
    char *fichier_mem_initial;
    char *fichier_mem_sauvegarde;
};

struct scenario_allocation {
    int n;
    int tailles[10];
    int indices[10];
    char *fichier_mem_initial;
    char *fichier_mem_sauvegarde;

};

struct scenario_liberation {
    int n;
    int indices[10];
    char *fichier_mem_initial;
    char *fichier_mem_sauvegarde;
};

struct scenario_bits_du_rm {
    int n;
    int indices[10];
    char *fichier_mem_initial;
    char *fichier_mem_sauvegarde;
};



struct scenario_quelconque test_1_3_b(void) {
    char *tab[] = {
        "1" "000000000000000" "0" "000000000000000",
        "0" "000000000000001" "0" "000000000000000",
        "0" "100000000000001" "0" "000000000000000",
        "0" "000000000000000" "1" "000000000000000",
        "0" "000000000000000" "0" "000000000000001",
        "0" "000000000000000" "0" "100000000000001"};
    int ind[] = { 200,205, 300,405,999,1000};
    struct scenario_quelconque res;
    int i;
    res.n = sizeof(tab)/sizeof(char *);
    assert (res.n == sizeof(ind)/sizeof(int));
    res.fichier_mem_initial = NULL;
    res.fichier_mem_sauvegarde = "test_4_0.img";
    for (i=0; i<res.n; i++) {
        res.chaines[i] = tab[i];
        res.indices[i] = ind[i];
    }
    return res;
}

struct scenario_allocation test_1_4_a(void) {
    /* Pour trouver les indices à mettre dans ind[], il faut sauver le
       scenario avec la fonction « sauver_scenario_allocation » qui
       affiche les indices. */
    int tab[] = {10, 5, 14, 1, 1, 60, 12,   1,   7,  7};
    int ind[] = { 0,11, 17,32, 34,36, 97, 110, 112, 120};
    struct scenario_allocation res;
    int i;
    res.n = 10;
    res.fichier_mem_initial = NULL;
    res.fichier_mem_sauvegarde = "test_4_1.img";
    for (i=0; i<res.n; i++) {
        res.tailles[i] = tab[i];
        res.indices[i] = ind[i];
    }
    return res;
}

struct scenario_liberation test_1_4_b(void) {
    int tab[] = {17, 32, 36, 112};
    struct scenario_liberation res;
    int i;
    res.n = 4;
    res.fichier_mem_initial    = "test_4_1.img";
    res.fichier_mem_sauvegarde = "test_4_2.img";
    for (i=0; i<res.n; i++) {
        res.indices[i] = tab[i];
    }
    return res;
}

struct scenario_liberation test_exemple(void) {
    int tab[] = {17, 32, 36, 112};
    struct scenario_liberation res;
    int i;
    res.n = 4;
    res.fichier_mem_initial    = "test_4_1.img";
    res.fichier_mem_sauvegarde = "exemple.img";
    for (i=0; i<res.n; i++) {
        res.indices[i] = tab[i];
    }
    return res;
}


struct scenario_allocation test_1_4_c(void) {
    int tab[] = {30,  7,  1,  30,  5};
    int ind[] = {36, 17, 25, 128, 27};
    struct scenario_allocation res;
    int i;
    res.n = 5;
    res.fichier_mem_initial    = "test_4_2.img";
    res.fichier_mem_sauvegarde = "test_4_3.img";
    for (i=0; i<res.n; i++) {
        res.tailles[i] = tab[i];
        res.indices[i] = ind[i];
    }
    return res;
}

struct scenario_bits_du_rm test_1_3_a(void) {
    int ind[] = {11, 27, 33, 97, 120};
    struct scenario_bits_du_rm res;
    int i;
    res.n = 5;
    res.fichier_mem_initial    = "test_4_3.img";
    res.fichier_mem_sauvegarde = "test_3_1.img";
    for (i=0; i<res.n; i++) {
        res.indices[i] = ind[i];
    }
    return res;
}


void prof_afficher_bloc(int select, char* repr) {
    int i;
    printf("[");

    printf("%s", (select == 1) ? couleur_vert : couleur_defaut);
    printf("%c ", repr[0]);

    printf("%s",  (select == 2) ? couleur_vert : couleur_defaut);
    for (i=1; i<16; i++) printf("%c", repr[i]);

    printf(" %s",  (select == 3) ? couleur_vert : couleur_defaut);
    printf("%c ", repr[16]);

    printf("%s",  (select == 4) ? couleur_vert : couleur_defaut);
    for (i=17; i<32; i++) printf("%c", repr[i]);
    printf("%s]\n", couleur_defaut);
}

int prof_binaire_vers_entier(char* repr) {
    uint32_t res = 0;
    while (*repr) res = (res<<1) + *(repr++) - 48;
    return res;
}

int prof_binaire_vers_rm(char* repr) {
    return *repr - 48;
}

int prof_binaire_vers_prec(char* repr) {
    int res = 0;
    int i = 0;
    repr++;
    for (i=0; i<15; i++) res = (res<<1) + *(repr++) - 48;
    return res;
}

int prof_binaire_vers_usage(char* repr) {
    int res;
    int i = 0;
    repr++;
    for (i=0; i<15; i++) res = (res<<1) + *(repr++) - 48;
    return *repr - 48;
}

int prof_binaire_vers_suivant(char* repr) {
    int res = 0;
    int i = 0;
    repr++;
    for (i=0; i<15; i++) res = (res<<1) + *(repr++) - 48;
    repr++;
    res = 0;
    for (i=0; i<15; i++) res = (res<<1) + *(repr++) - 48;
    return res;
}



void sauver_scenario_quelconque(struct scenario_quelconque scenar) {
    int i;
    int indice;
    uint32_t binaire;

    printf("SAUVEGARDE TEST QUELCONQUE %s\n", scenar.fichier_mem_sauvegarde);


    /* On initialise la mémoire */
    if (scenar.fichier_mem_initial == NULL) {
        reset_memoire();
        initialiser_memoire_dynamique();
    } else {
        charger_image_simple(scenar.fichier_mem_initial);
    }

    /* On remplit le tableau*/
    for (i=0; i<scenar.n; i++) {
        indice = scenar.indices[i];
        binaire =  prof_binaire_vers_entier(scenar.chaines[i]);

        MEMOIRE_DYNAMIQUE[indice] = binaire;
    }

    sauver_image(scenar.fichier_mem_sauvegarde);

}

void sauver_scenario_allocation(struct scenario_allocation scenar) {
    int i;
    void *ptr;

    printf("SAUVEGARDE TEST ALLOCATION %s\n", scenar.fichier_mem_sauvegarde);


    /* On initialise la mémoire */
    if (scenar.fichier_mem_initial == NULL) {
        reset_memoire();
        initialiser_memoire_dynamique();
    } else {
        charger_image_simple(scenar.fichier_mem_initial);
    }

    /* On enchaîne les malloc*/
    for (i=0; i<scenar.n; i++) {
        ptr = allocateur_malloc(scenar.tailles[i] * sizeof(uint32_t));
        printf("%d : %i (%ld)\n",
               i,
               scenar.indices[i],
               (uint32_t *) ptr - MEMOIRE_DYNAMIQUE - 1);
    }

    sauver_image(scenar.fichier_mem_sauvegarde);

}

void sauver_scenario_liberation(struct scenario_liberation scenar) {
    int i;
    int indice;

    printf("SAUVEGARDE TEST LIBÉRATION %s\n", scenar.fichier_mem_sauvegarde);


    /* On initialise la mémoire */
    charger_image_simple(scenar.fichier_mem_initial);

    /* On enchaîne les free*/
    for (i=0; i<scenar.n; i++) {
        indice = scenar.indices[i];
        allocateur_free(&MEMOIRE_DYNAMIQUE[indice+1]);
    }

    sauver_image(scenar.fichier_mem_sauvegarde);

}

void sauver_scenario_bits_du_rm(struct scenario_bits_du_rm scenar) {
    int i;
    int indice;

    printf("SAUVEGARDE TEST BIT DU RM %s\n", scenar.fichier_mem_sauvegarde);


    /* On initialise la mémoire */
    charger_image_simple(scenar.fichier_mem_initial);

    /* On ajoute la marque du rm*/
    for (i=0; i<scenar.n; i++) {
        indice = scenar.indices[i];
        MEMOIRE_DYNAMIQUE[indice] |= 1<<31;
    }

    sauver_image(scenar.fichier_mem_sauvegarde);

}


void generer_images_tp1(void) {
    reset_memoire();
    initialiser_memoire_dynamique();
    sauver_image("init.img");
    sauver_scenario_allocation(test_1_4_a());
    sauver_scenario_liberation(test_1_4_b());
    sauver_scenario_allocation(test_1_4_c());
    sauver_scenario_liberation(test_exemple());
    sauver_scenario_bits_du_rm(test_1_3_a());
    sauver_scenario_quelconque(test_1_3_b());
}



/**********************************/
/*                                */
/*  Outils pour les test du TP1   */
/*                                */
/**********************************/


void prof_afficher_bit_couleur(char a_afficher, char reference, char* couleur) {
    char *couleur_bit = a_afficher == reference ? couleur_defaut : couleur;
    printf("%s%c%s",couleur_bit, a_afficher, couleur_defaut);
}

void prof_afficher_comparaison(uint32_t correct, uint32_t errone, uint32_t taille) {
    char cor[33];
    char err[33];
    int i;
    char* vert  = couleur_vert;
    char* rouge = couleur_rouge;

    int taille_correcte, taille_erronee;

    taille_correcte = 0;
    taille_erronee = 0;
    for (i=31 ; i>=0; i--) {
     cor[i] = correct%2 ? '1' : '0';
     err[i] = errone%2  ? '1' : '0';
     if (cor[i]=='1') taille_correcte = 32-i;
     if (err[i]=='1') taille_erronee  = 32-i;
     correct >>=1;
     errone  >>=1;
    }

    cor[32] = 0;
    err[32] = 0;

    if (taille_correcte > taille) {
     fprintf(stderr, "ERREUR DU PROF %u>%u afficher_comparaison\n",
             taille_correcte,
             taille);
     exit(1);
    }


    taille_correcte = taille_correcte > taille ? taille_correcte : taille ;
    taille_erronee = taille_erronee > taille ? taille_erronee : taille ;

    for (i=0; i<32-taille_correcte; i++)  cor[i] = ' ';
    for (i=0; i<32-taille_erronee; i++)   err[i] = ' ';

    printf("Valeur attendue : [");
    for (i=0;  i<1;  i++) prof_afficher_bit_couleur(cor[i], err[i], vert);
    printf(" ");
    for (i=1;  i<16; i++) prof_afficher_bit_couleur(cor[i], err[i], vert);
    printf(" ");
    for (i=16; i<17; i++) prof_afficher_bit_couleur(cor[i], err[i], vert);
    printf(" ");
    for (i=17; i<32; i++) prof_afficher_bit_couleur(cor[i], err[i], vert);
    printf("]\n");

    printf("Valeur obtenue  : [");
    for (i=0;  i<1;  i++) prof_afficher_bit_couleur(err[i], cor[i],rouge);
    printf(" ");
    for (i=1;  i<16; i++) prof_afficher_bit_couleur(err[i], cor[i],rouge);
    printf(" ");
    for (i=16; i<17; i++) prof_afficher_bit_couleur(err[i], cor[i],rouge);
    printf(" ");
    for (i=17; i<32; i++) prof_afficher_bit_couleur(err[i], cor[i],rouge);
    printf("]\n");

}


void prof_expliquer_erreur_simple (int indice_fautif, char *image_mem) {
    int i = indice_fautif;
    printf("%sDans la case d’indice MEMOIRE_DYNAMIQUE[%d]%s\n",
           couleur_rouge,i,couleur_defaut);
    prof_afficher_comparaison(MEMOIRE_COPIE[i],MEMOIRE_DYNAMIQUE[i], 32);

    printf("\n%sMémoire lue%s\n", couleur_jaune, couleur_defaut);
    afficher_memoire_simple();

    charger_image(image_mem);
    printf("\n%sMémoire souhaitée%s\n", couleur_jaune, couleur_defaut);
    afficher_memoire_simple();
    printf("\n\n");
}

void prof_expliquer_erreur (int indice_fautif, char *image_mem) {
    int i = indice_fautif;
    printf("%sDans la case d’indice MEMOIRE_DYNAMIQUE[%d]%s\n",
           couleur_rouge,i,couleur_defaut);
    prof_afficher_comparaison(MEMOIRE_COPIE[i],MEMOIRE_DYNAMIQUE[i], 32);

    printf("\n%sMémoire lue%s\n", couleur_jaune, couleur_defaut);
    afficher_memoire();

    charger_image(image_mem);
    printf("\n%sMémoire souhaitée%s\n", couleur_jaune, couleur_defaut);
    afficher_memoire();
    printf("\n\n");
}

int tester_scenario_ballocation(struct scenario_allocation scenar) {
    int memoire_correcte;
    int b = 1;
    int i;
    int err;
    int ptr;
    int pointeurs[10];
    assert(scenar.n <= 10);

    /* On initialise la mémoire */
    if (scenar.fichier_mem_initial == NULL) {
        reset_memoire();
        initialiser_memoire_dynamique();
    } else {
        charger_image_simple(scenar.fichier_mem_initial);
    }

    /* On enchaîne les malloc*/
    for (i=0; i<scenar.n; i++) {
        pointeurs[i] = allocateur_balloc(scenar.tailles[i]);
    }

    /* On vérifie si la mémoire est correcte */
    err = comparer_image_et_memoire(scenar.fichier_mem_sauvegarde);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur_simple(-err, scenar.fichier_mem_sauvegarde);
        return b;
    }


    /* On vérifie si les pointeurs renvoyés sont les bons */
    for (i=0; i<scenar.n; i++) {
        ptr = scenar.indices[i];
        b = b && RUN_TEST(pointeurs[i] == ptr);
        if (!b) {
            printf("Indice %d au lieu de %d attendu\n", ptr, pointeurs[i]);
            printf("\n%sMémoire souhaitée%s\n", couleur_jaune, couleur_defaut);
            afficher_memoire_simple();
            printf("\n\n");
            return b;
        }
    }

    return b;


}


int tester_scenario_allocation(struct scenario_allocation scenar) {
    int memoire_correcte;
    int b = 1;
    int i;
    int err;
    void * ptr;
    void * pointeurs[10];
    assert(scenar.n <= 10);

    /* On initialise la mémoire */
    if (scenar.fichier_mem_initial == NULL) {
        reset_memoire();
        initialiser_memoire_dynamique();
    } else {
        charger_image_simple(scenar.fichier_mem_initial);
    }

    /* On enchaîne les malloc*/
    for (i=0; i<scenar.n; i++) {
        pointeurs[i] = allocateur_malloc(scenar.tailles[i] * sizeof(uint32_t));
    }

    /* On vérifie si la mémoire est correcte */
    err = comparer_image_et_memoire(scenar.fichier_mem_sauvegarde);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur_simple(-err, scenar.fichier_mem_sauvegarde);
        return b;
    }


    /* On vérifie si les pointeurs renvoyés sont les bons */
    for (i=0; i<scenar.n; i++) {
        ptr = &MEMOIRE_DYNAMIQUE[scenar.indices[i]]+1;
        b = b && RUN_TEST(pointeurs[i] == ptr);
        if (!b) {
            printf("Pointeur %p au lieu de %p attendu\n", ptr, pointeurs[i]);
            printf("\n%sMémoire souhaitée%s\n", couleur_jaune, couleur_defaut);
            afficher_memoire_simple();
            printf("\n\n");
            return b;
        }
    }

    return b;


}

int tester_scenario_bliberation(struct scenario_liberation scenar) {
    int b = 1;
    int i, err;
    int indice, memoire_correcte;
    assert(scenar.n <= 10);

    /* On initialise la mémoire */
    charger_image_simple(scenar.fichier_mem_initial);

    /* On enchaîne les free*/
    for (i=0; i<scenar.n; i++) {
        indice = scenar.indices[i];
        allocateur_bree(indice);
    }

    /* On vérifie si la mémoire est correcte */
    err = comparer_image_et_memoire(scenar.fichier_mem_sauvegarde);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur_simple(-err, scenar.fichier_mem_sauvegarde);
    }

    return b;
}

int tester_scenario_liberation(struct scenario_liberation scenar) {
    int b = 1;
    int i, err;
    int indice, memoire_correcte;
    assert(scenar.n <= 10);

    /* On initialise la mémoire */
    charger_image_simple(scenar.fichier_mem_initial);

    /* On enchaîne les free*/
    for (i=0; i<scenar.n; i++) {
        indice = scenar.indices[i];
        allocateur_free(&MEMOIRE_DYNAMIQUE[indice+1]);
    }

    /* On vérifie si la mémoire est correcte */
    err = comparer_image_et_memoire(scenar.fichier_mem_sauvegarde);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur_simple(-err, scenar.fichier_mem_sauvegarde);
    }

    return b;
}




/**************************************/
/*                                    */
/*  Tests pour les fonctions du TP1   */
/*                                    */
/**************************************/




/** Exercice 2 **/
#pragma weak initialiser_memoire_dynamique
void initialiser_memoire_dynamique(void);
int run_test_initialiser_memoire_dynamique(void) {
    int b = 1;
    int memoire_identique;
    reset_memoire();
    initialiser_memoire_dynamique();
    memoire_identique=comparer_image_et_memoire("init.img");
    b = RUN_TEST(1==memoire_identique) && b;

    if (!b) {
        prof_expliquer_erreur_simple(-memoire_identique, "init.img");
    }


    return b;

}

/** Exercice 3 **/
#pragma weak cons_bloc
uint32_t cons_bloc(int, int, int, int);
int run_test_cons_bloc(void) {
    struct scenario_quelconque scenar = test_1_3_b();
    int b = 1;
    char *binaire;
    int i;
    int marque, prec, usage, suivant;
    int voulu, obtenue;

    for (i=0; i<scenar.n; i++){
        binaire = scenar.chaines[i];
        marque  = prof_binaire_vers_rm(binaire);
        prec    = prof_binaire_vers_prec(binaire);
        usage   = prof_binaire_vers_usage(binaire);
        suivant = prof_binaire_vers_suivant(binaire);

        voulu   = prof_binaire_vers_entier(binaire);
	obtenue = cons_bloc(marque, prec, usage, suivant) ;

	b = RUN_TEST(voulu == obtenue);

	if (!b) {
	    prof_afficher_comparaison(voulu, obtenue, 32);
	    printf("\n");
	    return 0;
	}
    }
    return b;
}

#pragma weak bloc_suivant
int bloc_suivant(int i);
int run_test_bloc_suivant(void) {
    struct scenario_quelconque scenar = test_1_3_b();
    int b = 1;
    char *binaire;
    int i;
    int indice, voulu, obtenue;

    charger_image(scenar.fichier_mem_sauvegarde);

    for (i=0; i<scenar.n; i++){
        indice  = scenar.indices[i];
        binaire = scenar.chaines[i];
        voulu   = prof_binaire_vers_suivant(binaire);
	obtenue = bloc_suivant(indice) ;

	b = RUN_TEST(voulu == obtenue);

	if (!b) {
            prof_afficher_bloc(4, binaire);
	    prof_afficher_comparaison(voulu, obtenue, 15);
	    printf("\n");
	    return 0;
	}
    }
    return b;
}

#pragma weak bloc_precedant
int bloc_precedant(int i);
int run_test_bloc_precedant(void) {
    struct scenario_quelconque scenar = test_1_3_b();
    int b = 1;
    char *binaire;
    int i;
    int indice, voulu, obtenue;

    charger_image(scenar.fichier_mem_sauvegarde);

    for (i=0; i<scenar.n; i++){
        indice  = scenar.indices[i];
        binaire = scenar.chaines[i];
        voulu   = prof_binaire_vers_prec(binaire);
	obtenue = bloc_precedant(indice) ;

	b = RUN_TEST(voulu == obtenue );

	if (!b) {
            prof_afficher_bloc(2, binaire);
	    prof_afficher_comparaison(voulu, obtenue, 15);
	    printf("\n");
	    return 0;
	}
    }
    return b;
}

#pragma weak usage_bloc
int usage_bloc(int i);
int run_test_usage_bloc(void) {
    struct scenario_quelconque scenar = test_1_3_b();
    int b = 1;
    char *binaire;
    int i;
    int indice, voulu, obtenue;

    charger_image(scenar.fichier_mem_sauvegarde);

    for (i=0; i<scenar.n; i++){
        indice  = scenar.indices[i];
        binaire = scenar.chaines[i];
        voulu   = prof_binaire_vers_usage(binaire);
	obtenue = usage_bloc(indice) ;

	b = RUN_TEST(voulu == obtenue );

	if (!b) {
            prof_afficher_bloc(3, binaire);
	    prof_afficher_comparaison(voulu, obtenue, 1);
	    printf("\n");
	    return 0;
	}
    }
    return b;
}

#pragma weak rm_bloc
int rm_bloc(int i);
int run_test_rm_bloc(void) {
    struct scenario_quelconque scenar = test_1_3_b();
    int b = 1;
    char *binaire;
    int i;
    int indice, voulu, obtenue;

    charger_image(scenar.fichier_mem_sauvegarde);

    for (i=0; i<scenar.n; i++){
        indice  = scenar.indices[i];
        binaire = scenar.chaines[i];
        voulu   = prof_binaire_vers_rm(binaire);
	obtenue = rm_bloc(indice) ;

	b = RUN_TEST(voulu == obtenue);

	if (!b) {
            prof_afficher_bloc(1, binaire);
	    prof_afficher_comparaison(voulu, obtenue, 1);
	    printf("\n");
	    return 0;
	}
    }
    return b;
}

#pragma weak taille_bloc
int taille_bloc(int i);
int run_test_taille_bloc(void) {
    int ind[] = { 0,11,17,25,27,33,34,36,67,97,110,112,120,128,  159,32767};
    int tai[] = {10, 5, 7, 1, 5, 0, 1,30,29,12,  1,  7,  7, 30,32607,    0};
    int n=16;
    int b = 1;
    int i;
    assert (n*sizeof(int)==sizeof(ind)  && sizeof(ind)==sizeof(tai));
    charger_image("test_3_1.img");
    for (i=0; i<n; i++) {
        if (taille_bloc(ind[i]) != tai[i]) {
            if (b==1) printf("%s[KO]%s\n\n", couleur_rouge, couleur_defaut);
            b=0;
            printf("taille[%d] == %d et non %d\n", ind[i], tai[i], taille_bloc(ind[i]));
        }
    }
    if (!b) afficher_memoire_simple();
    return b;
}

/** Exercice 4 **/

#pragma weak octets_vers_blocs
int octets_vers_blocs(size_t size);
int run_test_octets_vers_blocs(void) {
    int b=1;

    b = b && octets_vers_blocs(0)  == 0;
    b = b && octets_vers_blocs(4)  == 1;
    b = b && octets_vers_blocs(8)  == 2;
    b = b && octets_vers_blocs(12) == 3;

    b = b && octets_vers_blocs(1)  == 1;
    b = b && octets_vers_blocs(5)  == 2;
    b = b && octets_vers_blocs(9)  == 3;
    b = b && octets_vers_blocs(13) == 4;

    b = b && octets_vers_blocs(2)  == 1;
    b = b && octets_vers_blocs(6)  == 2;
    b = b && octets_vers_blocs(10) == 3;
    b = b && octets_vers_blocs(14) == 4;

    b = b && octets_vers_blocs(3)  == 1;
    b = b && octets_vers_blocs(7)  == 2;
    b = b && octets_vers_blocs(11) == 3;
    b = b && octets_vers_blocs(15) == 4;

    return b;
}

#pragma weak rechercher_bloc_libre
int rechercher_bloc_libre(int taille);
int run_test_rechercher_bloc_libre(void) {
    int taille[] = { 1, 16, 17, 59,  60,  61, 32637,32638,32639};
    int indice[] = {17, 17, 36, 36,  36, 128, 128,    128, -1};
    int n = 9;
    int i;
    int b = 1;
    charger_image("test_4_2.img");
    for (i=0; i<n; i++) {
        b = b && rechercher_bloc_libre(taille[i]) == indice[i];
        if (!b) break;
    }
    if (!b) {
        printf("%s[KO]%s\n\n", couleur_rouge, couleur_defaut);
        printf("rechercher_bloc_libre(%d) == %d et non %d\n",
               taille[i],
               indice[i],
               rechercher_bloc_libre(taille[i]));

        afficher_memoire_simple();
    }
    return b;
}


#pragma weak allocateur_balloc
int allocateur_balloc(int taille);
int run_test_allocateur_balloc(void) {
    int b = 1;
    b = b && tester_scenario_ballocation(test_1_4_a());
    b = b && tester_scenario_ballocation(test_1_4_c());
    b = b && RUN_TEST(allocateur_balloc(1<<20) == -1);
    return b;
}


#pragma weak allocateur_malloc
void  *allocateur_malloc(size_t size);
int run_test_allocateur_malloc(void) {
    int b = 1;
    b = b && tester_scenario_allocation(test_1_4_a());
    b = b && tester_scenario_allocation(test_1_4_c());
    b = b && RUN_TEST(allocateur_malloc(1<<20) == NULL);
    return b;
}

#pragma weak allocateur_bree
void allocateur_bree(int indice);
int run_test_allocateur_bree(void) {
    int b = 1;
    b = b && tester_scenario_bliberation(test_1_4_b());
    return b;
}


#pragma weak allocateur_free
void allocateur_free(void *ptr);
int run_test_allocateur_free(void) {
    int b = 1;
    b = b && tester_scenario_liberation(test_1_4_b());
    return b;
}











/**************************************/
/*                                    */
/*  Tests pour les fonctions du TP2   */
/*                                    */
/**************************************/
typedef void* sexpr;


#pragma weak new_integer
sexpr new_integer(int);
#pragma weak integer_p
int integer_p(sexpr);
#pragma weak get_integer
int get_integer(sexpr);

#pragma weak new_string
sexpr new_string(char*);
#pragma weak new_symbol
sexpr new_symbol(char*);

#pragma weak get_string
char* get_string(sexpr);
#pragma weak get_symbol
char* get_symbol(sexpr);


#pragma weak string_p
int string_p(sexpr);

#pragma weak symbol_p
int symbol_p(sexpr);

#pragma weak symbol_match_p
int symbol_match_p(sexpr, char*);


#pragma weak cons
sexpr cons(sexpr, sexpr);
#pragma weak car
sexpr car(sexpr);
#pragma weak cdr
sexpr cdr(sexpr);
#pragma weak set_car
void set_car(sexpr, sexpr);
#pragma weak set_cdr
void set_cdr(sexpr, sexpr);
#pragma weak cons_p
int cons_p(sexpr);


#pragma weak new_primitive
sexpr new_primitive(char* nom, sexpr (*p) (sexpr, sexpr));
#pragma weak new_speciale
sexpr new_speciale(char* nom, sexpr (*p) (sexpr, sexpr));
#pragma weak prim_p
int prim_p(sexpr);
#pragma weak spec_p
int spec_p(sexpr);
#pragma weak get_name
char *get_name(sexpr);
#pragma weak get_prim
sexpr (*get_prim(sexpr)) (sexpr, sexpr);
#pragma weak run_prim
sexpr run_prim(sexpr, sexpr, sexpr);




void sauvegarder_une_primitive_manuellement(char *nom, sexpr (*f) (sexpr, sexpr));

char* IMAGE_TEST_TYPES    = "types.img";
char* IMAGE_TEST_ENTIERS  = "entiers.img";
char* IMAGE_TEST_CHAINES  = "chaines.img";
char* IMAGE_TEST_SYMBOLES = "symboles.img";
char* IMAGE_TEST_LISTES   = "listes.img";
char* IMAGE_TEST_LIST_P   = "list-p.img";
char* IMAGE_TEST_PRIMS    = "primitives.img";
char* IMAGE_TEST_SPECS    = "speciales.img";

int  ENTIERS_POUR_TEST[]     = {12, 65, -11,   0,   1,   2,  48,   0,  -9,  10};
int  INDICES_ENTIERS_TEST[]  = { 0,  7,  14,  21,  28,  35,  42,  49,  56,  63};
int  INDICES_ENTIERS_TYPES[] = { 0, 33,  75, 116, 160, 192, 206, 220, 234, 248};

char *CHAINES_POUR_TEST[]    = {"Salut", "à", "toi", "camarade", "nil"};
int  INDICES_CHAINE_TEST[]   = {      0,  10,   19,          28,    39};
int  INDICES_CHAINE_TYPES[]  = {      7,  40,   82,         123,   167};

char *SYMBOLES_POUR_TEST[]   = {"+", "defun", "=", "print", "nil"};
int  INDICES_SYMBOLE_TEST[] =  {  0,      9,   19,      28,  39};
int  INDICES_SYMBOLE_TYPES[] = { 24,      56,  98,     141};

int  INDICES_CONS_TYPES[] = { 17, 49, 91, 134, 176, 199, 213, 227, 241, 255};

int  INDICES_CONS_TEST1[] = {7, 21, 35, 49, 63, 77, 91, 105, 119, 133};
int  INDICES_CAR_TEST1[]  = {0, 14, 28, 42, 56, 70, 84,  98, 112, 126};
int  INDICES_CONS_TEST2[] = {150, 166, 182, 200, 216};
int  INDICES_CAR_TEST2[]  = {140, 157, 173, 189, 207};
int  INDICES_CONS_TEST3[]  = {42, 49, 56, 63, 70, 77};
int  RESULTAT_CONS_TEST3[] = { 0,  1,  1,  1,  1,  0};

int  INDICES_PRIM_TYPES[] = {  66, 151 };
int  INDICES_SPEC_TYPES[] = { 107, 183 };


sexpr fonction1_pour_test(sexpr e1, sexpr e2) {
    return e1;
}

sexpr fonction2_pour_test(sexpr e1, sexpr e2) {
    return new_integer(1789);
}

sexpr speciale1_pour_test(sexpr e1, sexpr e2) {
    return e2;
}

sexpr speciale2_pour_test(sexpr e1, sexpr e2) {
    return cons(e1,e2);
}



void prof_sauvegarde_primitives(void) {
    static int pas_encore_sauvees = 1;
    if (pas_encore_sauvees) {
        sauvegarder_une_primitive_manuellement("f1", fonction1_pour_test);
        sauvegarder_une_primitive_manuellement("s1", speciale1_pour_test);
        sauvegarder_une_primitive_manuellement("f2", fonction2_pour_test);
        sauvegarder_une_primitive_manuellement("s2", speciale2_pour_test);

        pas_encore_sauvees = 0;
    }
}


int tester_un_type(int* indices, int n, int(*type_p)(sexpr), char* type, int res) {
    int b;
    int i;
    int indice;
    sexpr e;
    for (i=0; i<n; i++) {
        indice = indices[i];
        e = &MEMOIRE_DYNAMIQUE[indice + 1];
        b = RUN_TEST(res==type_p(e));
        if (!b) {
            if (res) {
                printf("La sexpr d’indice %d non reconnu comme %s\n", indice, type);
            } else {
                printf("La sexpr d’indice %d reconnue à tort comme %s\n",
                       indice, type);
            }
            printf("@load %s\n", IMAGE_TEST_TYPES);
            return b;
        }
      }
    return 1;
    }


#define TESTER(INDICES,TYPE_P,TYPE,RES) \
    tester_un_type(INDICES, sizeof(INDICES)/sizeof(int), TYPE_P, TYPE, RES)



/* Entiers */

int run_test_new_integer(void) {
    int i;
    int err;
    int memoire_correcte;
    int b = 1;
    void *ptr;
    int indice;
    int n = sizeof(ENTIERS_POUR_TEST)/sizeof(int);
    reset_memoire();
    initialiser_memoire_dynamique();
    for (i=0; i<n; i++) {
        indice = INDICES_ENTIERS_TEST[i];
        ptr = &MEMOIRE_DYNAMIQUE[indice+1];
        b = b && RUN_TEST(ptr == new_integer(ENTIERS_POUR_TEST[i]));
    }

    err = comparer_image_et_memoire(IMAGE_TEST_ENTIERS);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_ENTIERS);
        return b;
    }
    return b;

}

int run_test_get_integer(void) {
    int i;
    int b = 1;
    int j;
    int n = sizeof(ENTIERS_POUR_TEST)/sizeof(int);

    reset_memoire();
    initialiser_memoire_dynamique();

    for (i=0; i<n; i++) {
        j = ENTIERS_POUR_TEST[i];
        b = b && RUN_TEST(j == get_integer(new_integer(j)));
    }
    return b;

}

int run_test_integer_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(0==integer_p(NULL)); /* Le symbole nil */
    if (!b) printf("NULL n’est pas un entier\n");
    b = b && TESTER(INDICES_ENTIERS_TYPES, integer_p, "entier", 1);
    b = b && TESTER(INDICES_CHAINE_TYPES,  integer_p, "entier", 0);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, integer_p, "entier", 0);
    b = b && TESTER(INDICES_CONS_TYPES,    integer_p, "entier", 0);
    b = b && TESTER(INDICES_PRIM_TYPES,    integer_p, "entier", 0);
    b = b && TESTER(INDICES_SPEC_TYPES,    integer_p, "entier", 0);

    return b;

}


/* Chaînes */

int run_test_new_string(void) {
    int i;
    int err;
    int memoire_correcte;
    int b = 1;
    void *ptr;
    int indice;
    int n = sizeof(CHAINES_POUR_TEST)/sizeof(char*);
    reset_memoire();
    initialiser_memoire_dynamique();
    for (i=0; i<n; i++) {
        indice = INDICES_CHAINE_TEST[i];
        ptr = &MEMOIRE_DYNAMIQUE[indice+1];
        b = b && RUN_TEST(ptr == new_string(CHAINES_POUR_TEST[i]));
        if (!b) {
            printf("%s\n",CHAINES_POUR_TEST[i]);
        }
    }

    err = comparer_image_et_memoire(IMAGE_TEST_CHAINES);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_CHAINES);
        return b;
    }
    return b;

}

int run_test_get_string(void) {
    int i;
    int b = 1;
    char* j;
    int n = sizeof(CHAINES_POUR_TEST)/sizeof(char*);

    reset_memoire();
    initialiser_memoire_dynamique();

    for (i=0; i<n; i++) {
        j = CHAINES_POUR_TEST[i];
        b = b && RUN_TEST(strcmp(j, get_string(new_string(j))) == 0);
    }
    return b;

}

int run_test_string_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(0==string_p(NULL)); /* Le symbole nil */
    if (!b) {
        printf("NULL n’est pas une chaîne\n");    }

    b = b && TESTER(INDICES_ENTIERS_TYPES, string_p, "Chaîne", 0);
    b = b && TESTER(INDICES_CHAINE_TYPES,  string_p, "Chaîne", 1);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, string_p, "Chaîne", 0);
    b = b && TESTER(INDICES_CONS_TYPES,    string_p, "Chaîne", 0);
    b = b && TESTER(INDICES_PRIM_TYPES,    string_p, "Chaîne", 0);
    b = b && TESTER(INDICES_SPEC_TYPES,    string_p, "Chaîne", 0);

    return b;

}


/* Symboles */

int run_test_new_symbol(void) {
    int i;
    int err;
    int memoire_correcte;
    int b = 1;
    void *ptr;
    int indice;
    int n = sizeof(SYMBOLES_POUR_TEST)/sizeof(char*);
    reset_memoire();
    initialiser_memoire_dynamique();
    /* On s’arrête un cran avant car nil n'apparait pas en mémoire*/
    for (i=0; i<n-1; i++) {
        indice = INDICES_SYMBOLE_TEST[i];
        ptr = &MEMOIRE_DYNAMIQUE[indice+1];
        if (indice==-1) ptr=NULL;
        b = b && RUN_TEST(ptr == new_symbol(SYMBOLES_POUR_TEST[i]));
    }

    err = comparer_image_et_memoire(IMAGE_TEST_SYMBOLES);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_SYMBOLES);
        return b;
    }
    return b;
}

int run_test_get_symbol(void) {
    int i;
    int b = 1;
    char* j;
    int n = sizeof(SYMBOLES_POUR_TEST)/sizeof(char*);

    reset_memoire();
    initialiser_memoire_dynamique();
    for (i=0; i<n; i++) {
        j = CHAINES_POUR_TEST[i];
        b = b && RUN_TEST(strcmp(j, get_symbol(new_symbol(j))) == 0);

    }
    return b;

}

int run_test_symbol_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(1==symbol_p(NULL)); /* Le symbole nil est traité à part*/
    if (!b) {
        printf("nil (=NULL) est un symbole\n");    }

    b = b && TESTER(INDICES_ENTIERS_TYPES, symbol_p, "symbole", 0);
    b = b && TESTER(INDICES_CHAINE_TYPES,  symbol_p, "symbole", 0);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, symbol_p, "symbole", 1);
    b = b && TESTER(INDICES_CONS_TYPES,    symbol_p, "symbole", 0);
    b = b && TESTER(INDICES_PRIM_TYPES,    symbol_p, "symbole", 0);
    b = b && TESTER(INDICES_SPEC_TYPES,    symbol_p, "symbole", 0);

    return b;

}

int run_test_symbol_match_p(void) {
    int i,j;
    int b = 1;
    char* chi;
    char* chj;
    sexpr e;
    int n = sizeof(SYMBOLES_POUR_TEST)/sizeof(char*);
    int res;
    reset_memoire();
    initialiser_memoire_dynamique();
    for (i=0; i<n; i++) {
            chi = CHAINES_POUR_TEST[i];
            e = new_symbol(chi);
            for (j=0; j<n; j++) {
                chj = CHAINES_POUR_TEST[j];
                res = (i==j);
                b = b && RUN_TEST(res == symbol_match_p(e,chj));
            }
    }
    return b;

}


/* Listes */

int run_test_cons(void) {
    int i;
    sexpr liste;
    sexpr e;
    int err;
    int memoire_correcte;
    int b = 1;
    void *ptr;
    int indice;
    int n = sizeof(INDICES_CONS_TEST1)/sizeof(int);
    reset_memoire();
    initialiser_memoire_dynamique();
    liste=NULL;
    for (i=0; i<n; i++) {
        indice = INDICES_CONS_TEST1[i];
        ptr = &MEMOIRE_DYNAMIQUE[indice+1];
        e = new_integer(ENTIERS_POUR_TEST[i]);
        liste = cons(e, liste);
        b = b && RUN_TEST(ptr == liste);
    }
    if (!b) return b;

    liste=NULL;
    n = sizeof(INDICES_CONS_TEST2)/sizeof(int);
    for (i=0; i<n; i++) {
        indice = INDICES_CONS_TEST2[i];
        ptr = &MEMOIRE_DYNAMIQUE[indice+1];
        e = new_string(CHAINES_POUR_TEST[i]);
        liste = cons(e, liste);
        b = b && RUN_TEST(ptr == liste);
    }
    if (!b) return b;

    err = comparer_image_et_memoire(IMAGE_TEST_LISTES);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_LISTES);
        return b;
    }
    return b;
}

int run_test_cons_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(0==cons_p(NULL)); /* Le symbole nil n’est pas un cons */
    if (!b) {
        printf("nil (=NULL) est un symbole\n");    }

    b = b && TESTER(INDICES_ENTIERS_TYPES, cons_p, "cons", 0);
    b = b && TESTER(INDICES_CHAINE_TYPES,  cons_p, "cons", 0);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, cons_p, "cons", 0);
    b = b && TESTER(INDICES_CONS_TYPES,    cons_p, "cons", 1);
    b = b && TESTER(INDICES_PRIM_TYPES,    cons_p, "cons", 0);
    b = b && TESTER(INDICES_SPEC_TYPES,    cons_p, "cons", 0);

    return b;

}

int run_test_car(void) {
    int i;
    sexpr liste;
    sexpr e;
    int b = 1;
    int n = sizeof(INDICES_CONS_TEST1)/sizeof(int);
    reset_memoire();
    initialiser_memoire_dynamique();

    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i]);
        liste = cons(e, liste);
        b = b && RUN_TEST(e == car(liste));
    }

    n = sizeof(INDICES_CONS_TEST2)/sizeof(int);
    for (i=0; i<n; i++) {
        e = new_string(CHAINES_POUR_TEST[i]);
        liste = cons(e, liste);
        b = b && RUN_TEST(e == car(liste));
    }

    return b;
}

int run_test_cdr(void) {
    int i;
    sexpr liste;
    sexpr coudeur;
    sexpr e;
    int b = 1;
    int n = sizeof(INDICES_CONS_TEST1)/sizeof(int);
    reset_memoire();
    initialiser_memoire_dynamique();

    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i]);
        coudeur = liste;
        liste = cons(e, liste);
        b = b && RUN_TEST( coudeur == cdr(liste));
    }

    n = sizeof(INDICES_CONS_TEST2)/sizeof(int);
    for (i=0; i<n; i++) {
        e = new_string(CHAINES_POUR_TEST[i]);
        coudeur = liste;
        liste = cons(e, liste);
        b = b && RUN_TEST( coudeur == cdr(liste));
    }

    return b;
}

int run_test_set_car(void) {
    int i;
    sexpr liste;
    sexpr e;
    int b = 1;
    int n = sizeof(INDICES_CONS_TEST1)/sizeof(int);
    sexpr TABCONS[sizeof(INDICES_CONS_TEST1)/sizeof(int)] ;

    reset_memoire();
    initialiser_memoire_dynamique();
    liste=NULL;
    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i]);
        liste = cons(e, liste);
        TABCONS[i] = liste;
    }


    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i] * 2);
        liste = TABCONS[i];
        set_car(liste, e) ;
        b = b && RUN_TEST(e == car(liste));
    }

    return b;
}

int run_test_set_cdr(void) {
    int i;
    sexpr liste;
    sexpr e;
    int b = 1;
    int n = sizeof(INDICES_CONS_TEST1)/sizeof(int);
    sexpr TABCONS[sizeof(INDICES_CONS_TEST1)/sizeof(int)] ;

    reset_memoire();
    initialiser_memoire_dynamique();
    liste=NULL;
    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i]);
        liste = cons(e, liste);
        TABCONS[i] = liste;
    }


    for (i=0; i<n; i++) {
        e = new_integer(ENTIERS_POUR_TEST[i] * 2);
        liste = TABCONS[i];
        set_cdr(liste, e) ;
        b = b && RUN_TEST(e == cdr(liste));
    }

    return b;
}


/* Primitives */

int run_test_new_primitive(void) {
    int b=1;
    int err;
    int memoire_correcte;
    reset_memoire();
    initialiser_memoire_dynamique();
    new_primitive("f1",fonction1_pour_test);
    new_primitive("f2",fonction2_pour_test);
    err = comparer_image_et_memoire(IMAGE_TEST_PRIMS);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_PRIMS);
        return b;
    }
    return b;
}

int run_test_new_speciale(void) {
    int b=1;
    int err;
    int memoire_correcte;
    reset_memoire();
    initialiser_memoire_dynamique();
    new_primitive("f1",fonction1_pour_test);
    new_speciale("s1",speciale1_pour_test);
    new_primitive("f2",fonction2_pour_test);
    new_speciale("s2",speciale2_pour_test);
    err = comparer_image_et_memoire(IMAGE_TEST_SPECS);
    memoire_correcte = (err==1);
    b = b && RUN_TEST(memoire_correcte);
    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_SPECS);
        return b;
    }
    return b;

    return 1;

}


int run_test_prim_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(0==prim_p(NULL)); /* Le symbole nil n’est pas une primitive */
    if (!b) {
        printf("nil (=NULL) est un symbole\n");    }

    b = b && TESTER(INDICES_ENTIERS_TYPES, prim_p, "primitive", 0);
    b = b && TESTER(INDICES_CHAINE_TYPES,  prim_p, "primitive", 0);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, prim_p, "primitive", 0);
    b = b && TESTER(INDICES_CONS_TYPES,    prim_p, "primitive", 0);
    b = b && TESTER(INDICES_PRIM_TYPES,    prim_p, "primitive", 1);
    b = b && TESTER(INDICES_SPEC_TYPES,    prim_p, "primitive", 0);

    return b;

}

int run_test_spec_p(void) {
    int b = 1;
    prof_sauvegarde_primitives();
    charger_image(IMAGE_TEST_TYPES);

    b = b && RUN_TEST(0==spec_p(NULL)); /* nil n’est pas une forme spéciale */
    if (!b) {
        printf("nil (=NULL) est un symbole\n");    }

    b = b && TESTER(INDICES_ENTIERS_TYPES, spec_p, "forme spéciale", 0);
    b = b && TESTER(INDICES_CHAINE_TYPES,  spec_p, "forme spéciale", 0);
    b = b && TESTER(INDICES_SYMBOLE_TYPES, spec_p, "forme spéciale", 0);
    b = b && TESTER(INDICES_CONS_TYPES,    spec_p, "forme spéciale", 0);
    b = b && TESTER(INDICES_PRIM_TYPES,    spec_p, "forme spéciale", 0);
    b = b && TESTER(INDICES_SPEC_TYPES,    spec_p, "forme spéciale", 1);

    return b;

}

int run_test_get_name(void) {
    int b = 1;
    sexpr e;
    reset_memoire();
    initialiser_memoire_dynamique();
    e = new_primitive("f1",fonction1_pour_test);
    b = b && RUN_TEST(strcmp(get_name(e), "f1") == 0);
    e = new_speciale("s1",speciale1_pour_test);
    b = b && RUN_TEST(strcmp(get_name(e), "s1") == 0);
    e = new_primitive("f2",fonction2_pour_test);
    b = b && RUN_TEST(strcmp(get_name(e), "f2") == 0);
    e = new_speciale("s2",speciale2_pour_test);
    b = b && RUN_TEST(strcmp(get_name(e), "s2") == 0);
    return b;
}

int run_test_get_prim(void) {
    int b = 1;
    sexpr e;
    reset_memoire();
    initialiser_memoire_dynamique();
    e = new_primitive("f1",fonction1_pour_test);
    b = b && RUN_TEST(get_prim(e) == fonction1_pour_test);
    e = new_speciale("s1",speciale1_pour_test);
    b = b && RUN_TEST(get_prim(e) == &speciale1_pour_test);
    e = new_primitive("f2",fonction2_pour_test);
    b = b && RUN_TEST(get_prim(e) == &fonction2_pour_test);
    e = new_speciale("s2",speciale2_pour_test);
    b = b && RUN_TEST(get_prim(e) == &speciale2_pour_test);

    return b;
}

int run_test_run_prim(void) {
    int b = 1;
    sexpr e1, e2, e, f;
    reset_memoire();
    initialiser_memoire_dynamique();
    e1 = new_integer(1887);
    e2 = new_string("Zamenhof");
    f = new_primitive("f1",fonction1_pour_test);
    b = b && RUN_TEST(e1 == run_prim(f, e1, e2));
    f = new_speciale("s1",speciale1_pour_test);
    b = b && RUN_TEST(e2 == run_prim(f, e1, e2));
    f = new_primitive("f2",fonction2_pour_test);
    b = b && RUN_TEST(1789 == get_integer(run_prim(f, e1, e2)));
    f = new_speciale("s2",speciale2_pour_test);
    e = run_prim(f, e1, e2);
    b = b && RUN_TEST(e1 == car(e));
    b = b && RUN_TEST(e2 == cdr(e));

    return b;
}





/**************************************/
/*                                    */
/*  Tests pour les fonctions du TP3   */
/*                                    */
/**************************************/

#pragma weak ENV
extern void* ENV;

#pragma weak environnement_global
sexpr environnement_global(void);

#pragma weak initialiser_memoire
void initialiser_memoire(void);

#pragma weak longueur_env
int longueur_env(sexpr);

#pragma weak trouver_variable
int trouver_variable(sexpr, sexpr, sexpr*);

#pragma weak modifier_variable
int modifier_variable(sexpr, sexpr, sexpr);

#pragma weak definir_variable_globale
int definir_variable_globale(sexpr, sexpr);


char* IMAGE_TEST_INIT_MEM = "init-mem.img";
char* IMAGE_TEST_ENV_1 = "env1.img";
char* IMAGE_TEST_ENV_2 = "env2.img";


int run_test_environnement_global(void) {
    return environnement_global() == ENV;
}


int run_test_initialiser_memoire(void) {
    int err, memoire_correcte, b;
    reset_memoire();
    initialiser_memoire();
    err= comparer_image_et_memoire(IMAGE_TEST_INIT_MEM);
    memoire_correcte = (err==1);
    b = RUN_TEST(memoire_correcte);

    if (!b) {
        prof_expliquer_erreur(-err,IMAGE_TEST_INIT_MEM);
        return b;
    }
    return b;
}

int run_test_longueur_env(void) {
    int b;
    reset_memoire();
    initialiser_memoire();
    b  = 1;
    b &= RUN_TEST(longueur_env(NULL) == 0);
    b &= RUN_TEST(longueur_env(ENV) == 1);
    charger_image(IMAGE_TEST_ENV_1);
    b &= RUN_TEST(longueur_env(ENV) == 4);
    b &= RUN_TEST(longueur_env(cdr(ENV)) == 3);
    charger_image(IMAGE_TEST_ENV_2);
    b &= RUN_TEST(longueur_env(ENV) == 5);
    b &= RUN_TEST(longueur_env(cdr(ENV)) == 4);
    return b;
}

int test_recherche_variable(sexpr env, char* ch, int indice) {
    sexpr s = new_symbol(ch);
    sexpr e;
    int err;
    int i;
    err = trouver_variable(env, s, &e);
    i = (err == -1) ? -1 : (uint32_t *) e - MEMOIRE_DYNAMIQUE - 1;
    return i == indice;
}

int run_test_trouver_variable(void) {
    int b = 1;
    sexpr ev, e;
    int indice;
    charger_image(IMAGE_TEST_ENV_1);
    e = new_string("Baudelaire");
    indice = (uint32_t*) e - MEMOIRE_DYNAMIQUE -1;
    ev = cons(cons(new_symbol("Charles"), e),ENV);
    b = b && RUN_TEST(test_recherche_variable(ENV, "Bobby", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "Charles", -1));
    b = b && RUN_TEST(test_recherche_variable(ev , "Charles", indice));
    b = b && RUN_TEST(test_recherche_variable(ENV, "t", 0));
    b = b && RUN_TEST(test_recherche_variable(ENV, "x", 23));
    b = b && RUN_TEST(test_recherche_variable(ENV, "c", 53));
    b = b && RUN_TEST(test_recherche_variable(ev, "t", 0));
    b = b && RUN_TEST(test_recherche_variable(ev, "x", 23));
    b = b && RUN_TEST(test_recherche_variable(ev, "c", 53));
    b = b && RUN_TEST(test_recherche_variable(NULL, "t", -1));
    b = b && RUN_TEST(test_recherche_variable(NULL, "x", -1));
    b = b && RUN_TEST(test_recherche_variable(NULL, "c", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "révolution", 88));
    b = b && RUN_TEST(test_recherche_variable(ENV, "var", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "choucroute", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "chanson", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "son", -1));
    charger_image(IMAGE_TEST_ENV_2);
    e = new_string("Baudelaire");
    indice = (uint32_t*) e - MEMOIRE_DYNAMIQUE -1;
    ev = cons(cons(new_symbol("Charles"), e),ENV);
    b = b && RUN_TEST(test_recherche_variable(ENV, "Bobby", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "Charles", -1));
    b = b && RUN_TEST(test_recherche_variable(ev , "Charles", indice));
    b = b && RUN_TEST(test_recherche_variable(ENV, "t", 0));
    b = b && RUN_TEST(test_recherche_variable(ENV, "x", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "c", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "révolution", -1));
    b = b && RUN_TEST(test_recherche_variable(ENV, "var", 23));
    b = b && RUN_TEST(test_recherche_variable(ENV, "choucroute", 30));
    b = b && RUN_TEST(test_recherche_variable(ENV, "chanson", 101));
    b = b && RUN_TEST(test_recherche_variable(ENV, "son", 85));
    b = b && RUN_TEST(test_recherche_variable(ev, "var", 23));
    b = b && RUN_TEST(test_recherche_variable(ev, "choucroute", 30));
    b = b && RUN_TEST(test_recherche_variable(ev, "chanson", 101));
    b = b && RUN_TEST(test_recherche_variable(ev, "son", 85));
    b = b && RUN_TEST(test_recherche_variable(NULL, "var", -1));
    b = b && RUN_TEST(test_recherche_variable(NULL, "choucroute", -1));
    b = b && RUN_TEST(test_recherche_variable(NULL, "chanson", -1));
    b = b && RUN_TEST(test_recherche_variable(NULL, "son", -1));
    return b;
}


void afficher(sexpr env);
int test_modifier_une_variable(char* ch, int indice, int exist_p) {
    sexpr s = new_symbol(ch);
    sexpr var;
    sexpr e = &MEMOIRE_DYNAMIQUE[indice+1];
    int n = longueur_env(ENV);
    int err = modifier_variable(ENV, s, e);
    if (err == -1) return !exist_p;
    if (-1 == trouver_variable(ENV, s, &var)) return 0;
    return (e == var) && (n == longueur_env(ENV));
}


int run_test_modifier_variable(void) {
    int b = 1;
    charger_image(IMAGE_TEST_ENV_1);
    b = b && RUN_TEST(test_modifier_une_variable("Bobby", 23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("t", 23, 1));         /* 0  */
    b = b && RUN_TEST(test_modifier_une_variable("x", 53, 1));         /* 23 */
    b = b && RUN_TEST(test_modifier_une_variable("c", 88, 1));         /* 53 */
    b = b && RUN_TEST(test_modifier_une_variable("révolution", 0, 1)); /* 88 */
    b = b && RUN_TEST(test_modifier_une_variable("var",       23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("choucroute",23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("chanson",   23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("son",       23, 0));
    charger_image(IMAGE_TEST_ENV_2);
    b = b && RUN_TEST(test_modifier_une_variable("Bobby",      23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("t",          23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("x",          23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("c",          23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("révolution", 23, 0));
    b = b && RUN_TEST(test_modifier_une_variable("var", 101, 1));        /* 23 */
    b = b && RUN_TEST(test_modifier_une_variable("choucroute", 85, 1));  /* 30 */
    b = b && RUN_TEST(test_modifier_une_variable("chanson", 23, 1));     /* 101 */
    b = b && RUN_TEST(test_modifier_une_variable("son", 30, 1));         /* 85 */
    return b;
}

int test_definir_une_variable_globale(char* ch, int valeur, int exist_p) {
    sexpr nouv = new_integer(valeur);
    sexpr var  = new_symbol(ch);
    sexpr e;
    int n = longueur_env(ENV) + (exist_p ? 0 : 1);
    definir_variable_globale(var, nouv);
    if (n != longueur_env(ENV)) return 0;
    if (-1 == trouver_variable(ENV, var, &e)) return 0;
    return e == nouv;
}

int run_test_definir_variable_globale(void) {
    int b = 1;
    charger_image(IMAGE_TEST_ENV_1);
    b = b && RUN_TEST(test_definir_une_variable_globale("Bobby", 12, 0));
    b = b && RUN_TEST(test_definir_une_variable_globale("toto", 14, 0));
    b = b && RUN_TEST(test_definir_une_variable_globale("prouf", 144, 0));
    b = b && RUN_TEST(test_definir_une_variable_globale("Bobby", 12, 1));
    b = b && RUN_TEST(test_definir_une_variable_globale("toto", 14, 1));
    b = b && RUN_TEST(test_definir_une_variable_globale("prouf", 144, 1));
    b = b && RUN_TEST(test_definir_une_variable_globale("t", 13, 1));
    b = b && RUN_TEST(test_definir_une_variable_globale("x", 14, 1));
    b = b && RUN_TEST(test_definir_une_variable_globale("c", 144, 1));
    return b;
}



/************************/
/*                      */
/*  API pour les test   */
/*                      */
/************************/


int test_tp1(void) {
    int b = 1;
    int nombre_test_valides = 0;
    b = b && TEST_FONCTION(initialiser_memoire_dynamique); nombre_test_valides += b;
    b = b && TEST_FONCTION(cons_bloc)                    ; nombre_test_valides += b;
    b = b && TEST_FONCTION(bloc_suivant)                 ; nombre_test_valides += b;
    b = b && TEST_FONCTION(bloc_precedant)               ; nombre_test_valides += b;
    b = b && TEST_FONCTION(usage_bloc)                   ; nombre_test_valides += b;
    b = b && TEST_FONCTION(rm_bloc)                      ; nombre_test_valides += b;
    b = b && TEST_FONCTION(taille_bloc)                  ; nombre_test_valides += b;
    b = b && TEST_FONCTION(rechercher_bloc_libre)        ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_balloc)            ; nombre_test_valides += b;
    b = b && TEST_FONCTION(octets_vers_blocs)            ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_malloc)            ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_bree)              ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_free)              ; nombre_test_valides += b;
    printf_test("\n");
    return nombre_test_valides;
}


int test_tp1_obligatoire(void) {
    int b = 1;
    int nombre_test_valides = 0;
    b = b && TEST_FONCTION(initialiser_memoire_dynamique); nombre_test_valides += b;
    b = b && TEST_FONCTION(cons_bloc)                    ; nombre_test_valides += b;
    b = b && TEST_FONCTION(bloc_suivant)                 ; nombre_test_valides += b;
    b = b && TEST_FONCTION(bloc_precedant)               ; nombre_test_valides += b;
    b = b && TEST_FONCTION(usage_bloc)                   ; nombre_test_valides += b;
    b = b && TEST_FONCTION(rm_bloc)                      ; nombre_test_valides += b;
    b = b && TEST_FONCTION(taille_bloc)                  ; nombre_test_valides += b;
    b = b && TEST_FONCTION(rechercher_bloc_libre)        ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_balloc)            ; nombre_test_valides += b;
    b = b && TEST_FONCTION(octets_vers_blocs)            ; nombre_test_valides += b;
    b = b && TEST_FONCTION(allocateur_malloc)            ; nombre_test_valides += b;
    printf_test("\n");
    return nombre_test_valides;
}


void demarrer_test(char *message) {
    printf_test("%s %*s: ", message,  10 - longueur(message), "");
}


void finir_test(int b) {
    if (b) printf_test("%s[OK]%s\n", couleur_vert, couleur_defaut);
}


int test_tp2(void) {
    int b = 1;
    int nombre_test_valides = 0;
    int debug;

    if (!new_integer) return 1;


    demarrer_test("Entiers");
    debug = cacher_tests();
    b = b && TEST_FONCTION(new_integer); nombre_test_valides += b;
    b = b && TEST_FONCTION(integer_p);   nombre_test_valides += b;
    b = b && TEST_FONCTION(get_integer); nombre_test_valides += b;
    restaurer_tests(debug);
    finir_test(b);
    if (!b) return nombre_test_valides;

    demarrer_test("Chaînes");
    debug = cacher_tests();
    b = b && TEST_FONCTION(new_string); nombre_test_valides += b;
    b = b && TEST_FONCTION(string_p);   nombre_test_valides += b;
    b = b && TEST_FONCTION(get_string); nombre_test_valides += b;
    restaurer_tests(debug);
    finir_test(b);
    if (!b) return nombre_test_valides;

    demarrer_test("Symboles");
    debug = cacher_tests();
    b = b && TEST_FONCTION(new_symbol);     nombre_test_valides += b;
    b = b && TEST_FONCTION(symbol_p);       nombre_test_valides += b;
    b = b && TEST_FONCTION(get_symbol);     nombre_test_valides += b;
    b = b && TEST_FONCTION(symbol_match_p); nombre_test_valides += b;
    restaurer_tests(debug);
    finir_test(b);
    if (!b) return nombre_test_valides;

    demarrer_test("Listes");
    debug = cacher_tests();
    b = b && TEST_FONCTION(cons);    nombre_test_valides += b;
    b = b && TEST_FONCTION(cons_p);  nombre_test_valides += b;
    b = b && TEST_FONCTION(car);     nombre_test_valides += b;
    b = b && TEST_FONCTION(cdr);     nombre_test_valides += b;
    b = b && TEST_FONCTION(set_car); nombre_test_valides += b;
    b = b && TEST_FONCTION(set_cdr); nombre_test_valides += b;
    restaurer_tests(debug);
    finir_test(b);
    if (!b) return nombre_test_valides;

    demarrer_test("Primitives");
    debug = cacher_tests();
    b = b && TEST_FONCTION(new_primitive); nombre_test_valides += b;
    b = b && TEST_FONCTION(new_speciale);  nombre_test_valides += b;
    b = b && TEST_FONCTION(prim_p);        nombre_test_valides += b;
    b = b && TEST_FONCTION(spec_p);        nombre_test_valides += b;
    b = b && TEST_FONCTION(get_name);      nombre_test_valides += b;
    b = b && TEST_FONCTION(get_prim);      nombre_test_valides += b;
    b = b && TEST_FONCTION(run_prim);      nombre_test_valides += b;
    restaurer_tests(debug);
    finir_test(b);
    if (!b) return nombre_test_valides;
    printf("\n");


    return nombre_test_valides;
}


int test_tp3(void) {
    int b = 1;
    int nombre_test_valides = 0;

    b = b && TEST_FONCTION(environnement_global);     nombre_test_valides += b;
    b = b && TEST_FONCTION(initialiser_memoire);      nombre_test_valides += b;
    b = b && TEST_FONCTION(longueur_env);             nombre_test_valides += b;
    b = b && TEST_FONCTION(trouver_variable);         nombre_test_valides += b;
    b = b && TEST_FONCTION(modifier_variable);        nombre_test_valides += b;
    b = b && TEST_FONCTION(definir_variable_globale); nombre_test_valides += b;
    printf_test("\n");
    return nombre_test_valides;
}



