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

// Cartas especiales
#define REVERSE 10
#define DRAW_2 11
#define WILD_DRAW_4 12
#define WILD 13
#define SKIP 14

static const char *colorStr[] = {"Rojo", "Azul", "Amarillo", "Verde"};

/**
 * void viewCard
 * 
 * Card card
 * 
 * printea los atributos de card formateados
 */
void viewCard(Card card);

/**
 * int cardPlayable
 * 
 * Card last 
 * Card card 
 * returns: 0, 1
 * 
 * Define si card se puede jugar sobre last.
 * Esto ocurre si ambas cartas tienen el mismo color.
 */
int cardPlayable(Card last, Card card);

/**
 * void getPlayable
 * 
 * List *validCards: lista a cartas validas para jugar
 * Deck *hand: mano a analizar
 * Card lastCard: ultima carta en pila de juego
 * 
 * Guarda los indices de todas las cartas jugables en hand
 * (con respecto a lastCard), almacenandolos en validCards.
 */
void getPlayable(List *validCards, Deck *hand, Card lastCard);

/*******
* Mazo *
********/

#define DECK_SIZE 50
#define BUF_SIZE 128

/**
 * Deck *newDeck
 * 
 * int size: nº elementos maximos
 * returns: puntero a nuevo mazo, almacenado en heap
 */
Deck *newDeck(int size);

/**
 * int isFull
 * 
 * Deck *deck 
 * returns: 0, 1
 * 
 * 1 si el mazo tiene tantos elementos como deck->size permite,
 * 0 si no.
 */
int isFull(Deck *deck);

/**
 * int isEmpty
 * 
 * Deck *deck 
 * returns: 0, 1
 * 
 * 1 si el mazo no contiene cartas,
 * 0 si no.
 */
int isEmpty(Deck *deck);

/**
 * void push
 * 
 * Deck *deck
 * Card card
 * 
 * Agrega card al fondo de deck.
 * Inmediatamente despues, deck->cards[deck->top] == card.
 */
void push(Deck *deck, Card card);

/**
 * Card peek
 * 
 * Deck *deck 
 * returns: carta ubicada en la posicion superior de deck
 * (deck->top).
 */
Card peek(Deck *deck);

/**
 * Card pop
 * 
 * Deck* deck 
 * returns: carta removida de la posicion superior de deck 
 * 
 * Remueve la carta superior de deck y la retorna.
 */
Card pop(Deck *deck);

/**
 * void fillDeck
 * 
 * Deck* deck 
 * 
 * Llena el mazo con 40 cartas de color (0 a 9 para los cuatro colores),
 * más 10 cartas especiales, repartidas entre los cuatro colores.
 */
void fillDeck(Deck *deck);

/**
 * Card drawCard
 * 
 * Deck *deck 
 * returns: carta al tope de deck
 * 
 * Rellena el mazo con fillDeck() si esta vacio, luego usa pop().
 */
Card drawCard(Deck *deck);

/**********************
* Operaciones de pipe *
***********************/

/**
 * void pDealCard
 * 
 * int p2c: pipe parent->child 
 * struct pollfd *p2cIn: estructura capaz de monitorear el lado de lectura
 *  de p2c
 * Deck *deck: mazo para robar cartas
 * int count: cantidad de cartas para robar / escribir
 * 
 * Roba una carta de deck, y luego escribe esa carta a p2c.
 * Espera a que esta carta haya sido leida antes de retornar. 
 */
void pDealCard(int p2c, struct pollfd *pIn, Deck *deck, int count);

/**
 * void pSendCard
 * 
 * int p2c: pipe parent->child 
 * struct pollfd *p2cIn: estructura capaz de monitorear el lado de lectura
 *  de p2c
 * Card *card: direccion de memoria de carta a enviar
 * 
 * Escribe la carta apuntada por card a p2c.
 * Espera a que card haya sido leida antes de retornar. 
 */
void pSendCard(int p2c, struct pollfd *p2cIn, Card *card);


/**
 * void cDrawCard
 * 
 * int p2c: pipe parent->child
 * Deck *hand: mano en la que se colocaran cartas
 * 
 * Lee una cantidad arbitraria de cartas de p2c.
 * Agrega todas las cartas leidas a hand.
 */
void cDrawCard(int p2c, Deck *hand);

/**
 * void pPlayCard
 * 
 * int c2p: pipe child->parent
 * struct pollfd *p2cIn: estructura capaz de monitorear el lado de lectura
 *  de c2p
 * Deck *hand: mano que contiene cartas
 * int pos: posicion de la carta a jugar relativo a la mano
 * 
 * Escribe la carta en la posicion pos de hand a p2c.
 * Luego, espera a que esta carta haya sido leida antes de continuar.
 * Finalmente, quita esta carta de la mano, preocupandose de mantener el orden.
 */
void cPlayCard(int c2p, struct pollfd *c2pIn, Deck *hand, int pos);

/***********************
* Operaciones de juego *
************************/

/**
 * void nextTurn
 * 
 * int *next: direccion de memoria a id del prox. jugador por jugar su turno.
 * int turnOrder: -1, 1, para ciclar turnos por la izquierda o derecha
 * 
 * Actualiza next segun el orden definido por turnOrder, preocupandose que
 * el valor en next se mantenga en el rango [0, 3].
 */
void nextTurn(int *next, int turnOrder);

#endif
