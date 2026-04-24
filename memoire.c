#include "allocateur.h"
#include "erreurs.h"
#include "memoire.h"

void *valisp_malloc(size_t size){
    void *ptr = allocateur_malloc(size);
    
    if (ptr == NULL) {
        erreur(MEMOIRE, "@valloc", "plus de mémoire disponible", NULL);
    }
    
    return ptr;
}