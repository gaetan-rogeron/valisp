#include "allocateur.h"
#include <stdio.h>

bloc MEMOIRE_DYNAMIQUE[TAILLE_MEMOIRE_DYNAMIQUE];

void initialiser_memoire_dynamique() {
    MEMOIRE_DYNAMIQUE[0] = 32767;
    MEMOIRE_DYNAMIQUE[TAILLE_MEMOIRE_DYNAMIQUE - 1] = 65535;
}

bloc cons_bloc(int rm, int precedant, int libre, int suivant){
    return (rm << 31) | (precedant << 16) | (libre << 15) | suivant;
}

int bloc_suivant(int indice) {
    return MEMOIRE_DYNAMIQUE[indice] & 0x7FFF;
}

int usage_bloc(int indice) {
    return (MEMOIRE_DYNAMIQUE[indice] >> 15) & 1;
}

int bloc_precedant(int indice) {
    return (MEMOIRE_DYNAMIQUE[indice] >> 16) & 0x7FFF;
}

int rm_bloc(int indice) {
    return (MEMOIRE_DYNAMIQUE[indice] >> 31) & 1;
}

int taille_bloc(int indice) {
    if (indice == TAILLE_MEMOIRE_DYNAMIQUE - 1) {
        return 0;
    }
    return bloc_suivant(indice) - indice - 1;
}

int rechercher_bloc_libre(int nombre_blocs) {
    int courant = 0;
    int dernier = TAILLE_MEMOIRE_DYNAMIQUE - 1;

    while (courant != dernier) {
        if (usage_bloc(courant) == 0 && taille_bloc(courant) >= nombre_blocs) {
            return courant; 
        }
        courant = bloc_suivant(courant);
    }
    
    return -1;
}

int allocateur_balloc(int nombre_blocs) {

    int indice = rechercher_bloc_libre(nombre_blocs);

    int taille_dispo = taille_bloc(indice);
    int precedent = bloc_precedant(indice);
    int suivant = bloc_suivant(indice);

    if (indice == -1) return -1;

    if (taille_dispo > nombre_blocs) {
        int nouvel_indice = indice + nombre_blocs + 1;
        
        MEMOIRE_DYNAMIQUE[nouvel_indice] = cons_bloc(0, indice, 0, suivant);
        
        MEMOIRE_DYNAMIQUE[indice] = cons_bloc(0, precedent, 1, nouvel_indice);
        
        MEMOIRE_DYNAMIQUE[suivant] = cons_bloc(rm_bloc(suivant), nouvel_indice, usage_bloc(suivant), bloc_suivant(suivant));
    } else {
        MEMOIRE_DYNAMIQUE[indice] = cons_bloc(0, precedent, 1, suivant);
    }

    return indice;
}

int octets_vers_blocs(size_t size) {
    return (size + 3) / 4;
}

void *allocateur_malloc(size_t size) {
    int blocs_necessaires = octets_vers_blocs(size);
    int indice = allocateur_balloc(blocs_necessaires);
    
    if (indice == -1) {
        return NULL;
    }
    
    return (void *)(&MEMOIRE_DYNAMIQUE[indice + 1]);
}

void allocateur_bree(int i) {
    int precedent = bloc_precedant(i);
    int suivant = bloc_suivant(i);

    MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), precedent, 0, suivant);

    if (suivant != TAILLE_MEMOIRE_DYNAMIQUE - 1 && usage_bloc(suivant) == 0) {
        int suivant_du_suivant = bloc_suivant(suivant);
        
        MEMOIRE_DYNAMIQUE[i] = cons_bloc(rm_bloc(i), precedent, 0, suivant_du_suivant);
        
        MEMOIRE_DYNAMIQUE[suivant_du_suivant] = cons_bloc(rm_bloc(suivant_du_suivant), i, usage_bloc(suivant_du_suivant), bloc_suivant(suivant_du_suivant));
        
        suivant = suivant_du_suivant;
    }

    if (i != 0 && usage_bloc(precedent) == 0) {
        MEMOIRE_DYNAMIQUE[precedent] = cons_bloc(rm_bloc(precedent), bloc_precedant(precedent), 0, suivant);
        
        MEMOIRE_DYNAMIQUE[suivant] = cons_bloc(rm_bloc(suivant), precedent, usage_bloc(suivant), bloc_suivant(suivant));
    }
}

void allocateur_free(void *ptr) {
    
    bloc *case_donnees = (bloc *)ptr;
    
    int indice_donnees = case_donnees - MEMOIRE_DYNAMIQUE;
    
    int indice_entete = indice_donnees - 1;

    if (ptr == NULL) {
        return; 
    }
    allocateur_bree(indice_entete);
}