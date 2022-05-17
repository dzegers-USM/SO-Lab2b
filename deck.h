#ifndef UNODECK_H
#define UNODECK_H

// Referencias:
// https://www.geeksforgeeks.org/stack-data-structure-introduction-program/

#include "util.h"

typedef struct {
    int type;
    int color;
} Card;

typedef struct {
    int top;
    int size;
    Card *cards;
} Deck;

/*********
* Cartas *
**********/

#define REVERSE 10
#define DRAW_2 11
#define WILD_DRAW_4 12
#define WILD 13
#define SKIP 14

static const char *colorStr[] = {"Rojo", "Azul", "Amarillo", "Verde"};

void viewCard(Card card);
int cardPlayable(Card last, Card card);
void getPlayable(List *validCards, Deck *hand, Card lastCard);

/*******
* Mazo *
********/

#define DECK_SIZE 50
#define BUF_SIZE 128

Deck *newDeck(int size);
int isFull(Deck *deck);
int isEmpty(Deck *deck);
void push(Deck *deck, Card card);
Card peek(Deck *deck);
Card pop(Deck *deck);
void fillDeck(Deck *deck);
Card drawCard(Deck *deck);

/**********************
* Operaciones de pipe *
***********************/

void pDealCard(int p2c, struct pollfd *pIn, Deck *deck, int count);
void pSendCard(int p2c, struct pollfd *p2cIn, Card *card);

void cDrawCard(int p2c, Deck *hand);
void cPlayCard(int c2p, struct pollfd *c2pIn, Deck *hand, int pos);

/***********************
* Operaciones de juego *
************************/

void nextTurn(int *next, int turnOrder);

#endif
