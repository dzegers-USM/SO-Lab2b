#include "util.h"
#include <unistd.h>

// Referencias:
// https://stackoverflow.com/questions/6127503/shuffle-array-in-c
// https://stackoverflow.com/questions/1202687/how-do-i-get-a-specific-range-of-numbers-from-rand

int randCmp(const void *a, const void *b) {
    // Evita warnings para uso con qsort()
    (void) a; (void) b;

    return rand() % 2 ? +1 : -1;
}

void shuffle(void *base, size_t nmemb, size_t size) {
    qsort(base, nmemb, size, randCmp);
}

int randInt(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

List *newList(int size) {
    List *list = malloc(sizeof(List));
    list->size = size;
    list->top = -1;
    list->arr = malloc(list->size * sizeof(int));
}

void append(List *list, int num) {
    list->arr[++list->top] = num;
    return;
}

void clearList(List *list) {
    list->top = -1;
    return;
}

void cWriteInt(int c2p, struct pollfd *c2pIn, int *iptr) {
    while (write(c2p, iptr, sizeof(int)) < 0 ) {};
    while (poll(c2pIn, 1, 32) == 0) {};
    return;
}
