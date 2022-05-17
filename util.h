#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <poll.h>

// Referencias:
// https://stackoverflow.com/questions/6127503/shuffle-array-in-c

int randCmp(const void *a, const void *b);
void shuffle(void *base, size_t nmemb, size_t size);
int randInt(int min, int max);

typedef struct {
    int *arr;
    int size;
    int top;
} List;

List *newList(int size);
void append(List *list, int num);
void clearList(List *list);

void cWriteInt(int c2p, struct pollfd *c2pIn, int *iptr);

#endif
