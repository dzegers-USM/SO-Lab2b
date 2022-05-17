#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <poll.h>

// Referencias:
// https://stackoverflow.com/questions/6127503/shuffle-array-in-c

/**
 * int randCmp
 * 
 * void *a
 * void* b
 * returns: 1, -1 aleatoriamente
 * 
 * Funcion compañera a shuffle()
 */
int randCmp(const void *a, const void *b);

/**
 * void shuffle
 * 
 * void *base: arreglo
 * size_t nmemb: nº elementos en base
 * size_t size: memoria utilizada por arreglo 
 * 
 * Sortea aleatoriamente un arreglo
 * Utilizado para randomizar el mazo de juego
 */
void shuffle(void *base, size_t nmemb, size_t size);

/**
 * int randInt
 * 
 * int min: minimo rango
 * int max: maximo rango
 * returns: numero entero en rango [min, max]
 */
int randInt(int min, int max);

// Stack en disfraz :^)
typedef struct {
    int *arr;
    int size;
    int top;
} List;

/**
 * List *newList
 * 
 * int size: nº elementos maximos
 * returns: puntero a nueva lista, almacenada en heap
 */
List *newList(int size);

/**
 * void append
 * 
 * List *list
 * int num
 * 
 * Agrega elemento al fondo de una lista
 * Analogo a funcion push() de stack
 */
void append(List *list, int num);

/**
 * void clearList
 * 
 * List *list
 * 
 * "Elimina" los elementos de una lista
 * list->top = -1;
 */
void clearList(List *list);

/**
 * void destroyList
 * 
 * List *list
 * 
 * Desaloca la memoria heap dedicada a list
 */
void destroyList(List* list);

/**
 * void cWriteInt
 * 
 * int c2p: pipe child->parent
 * struct pollfd *c2pIn: estructura capaz de monitorear el lado de lectura
 *  de c2p
 * int iptr: direccion de memoria de int a escribir
 * 
 * Escribe un int a c2p.
 * Espera a que este haya sido leido antes de retornar.
 */
void cWriteInt(int c2p, struct pollfd *c2pIn, int *iptr);

#endif
