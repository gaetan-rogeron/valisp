#include "allocateur.h"
#include "erreurs.h"

void *valisp_malloc(size_t size){
    void *res = allocateur_malloc(size);
    if (res == NULL){
        ERREUR_FATALE("ERREUR ALLOCATION MALLOC");
    }
    return res;
}