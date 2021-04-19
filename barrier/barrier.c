#include <stdio.h>
#include <stdlib.h>

#include "barrier.h"

int barrier_init(barrier_t *barr, unsigned int count) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    barr = malloc(sizeof(barrier_t));

    if (!barr)
        return 1;

    barr->count = count;
    barr->waiting = 0;
    barr->mutex = mutex;
    barr->cond = cond;

    return 0;
}

int barrier_wait(barrier_t *barr) {
    int falla;
    falla = pthread_mutex_lock(&(barr->mutex));

    if (!falla) {
        if (barr->waiting < barr->count) {
            ++barr->waiting;
            falla = pthread_cond_wait(&(barr->cond), &(barr->mutex));
        }
        else {
            falla = pthread_cond_broadcast(&(barr->cond));
            falla = falla || pthread_mutex_unlock(&(barr->mutex));
            barr->waiting = 0;
        }
    }

    return falla;
}

int barried_destroy(barrier_t *barr) {
    int falla;
    falla = pthread_cond_destroy(&(barr->cond));
    falla = falla || pthread_mutex_destroy(&(barr->mutex));

    free(barr);
    
    return falla;
}
