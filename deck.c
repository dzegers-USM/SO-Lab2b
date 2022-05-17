#include "deck.h"
#include "util.h"
#include <stdio.h>
#include <unistd.h>


// Referencias:
// https://www.geeksforgeeks.org/stack-data-structure-introduction-program/

/*********
* Cartas *
**********/

void viewCard(Card card) {
    printf("Tipo: %d, Color: %s\n", card.type, colorStr[card.color]);
}

int cardPlayable(Card last, Card card) {
    return (card.color == last.color);
}

void getPlayable(List *validCards, Deck *hand, Card lastCard) {
    for (int i = 0; i <= hand->top; i++) {
        if (hand->cards[i].color == lastCard.color) {
            append(validCards, i);
        }
    }
    return;
}

/*******
* Mazo *
********/

Deck *newDeck(int size) {
    Deck *deck = malloc(sizeof(Deck));
    deck->size = size;
    deck->top = -1;
    deck->cards = malloc(deck->size * sizeof(Card));
}

int isFull(Deck *deck) {
    return deck->top == deck->size - 1;
}

int isEmpty(Deck *deck) {
    return deck->top == -1;
}

void push(Deck *deck, Card card) {
    deck->cards[++deck->top] = card;
    return;
}

Card peek(Deck *deck) {
    return deck->cards[deck->top];
}

Card pop(Deck *deck) {
    return deck->cards[deck->top--];
}

void fillDeck(Deck *deck) {
    for (int color = 0; color < 4; color++) {
        for (int type = 0; type < 10; type++) {
            push(deck, (Card) {type, color});
        }
    }

    int color;
    for (color = 0; color < 2; color++) {
        for (int type = 10; type < 15; type += 2) {
            push(deck, (Card) { type, color });
        }
    }

    for (color; color < 4; color++) {
        for (int type = 11; type < 15; type += 2) {
            push(deck, (Card) { type, color });
        }
    }

    shuffle(deck->cards, 50, sizeof(Card));
    return;
}

Card drawCard(Deck *deck) {
    if (isEmpty(deck)) {
        fillDeck(deck);
    }
    return pop(deck);
}

/**********************
* Operaciones de pipe *
***********************/

void pDealCard(int p2c, struct pollfd *p2cIn, Deck *deck, int count) {
    size_t size = count*sizeof(Card);
    Card *buf = malloc(size);
    for (int i = 0; i < count; i++) {
        buf[i] = drawCard(deck);
    }
    while (write(p2c, buf, size) < 0) {};
    free(buf);
    while (poll(p2cIn, 1, 32) != 0) {};
    return;
}

void pSendCard(int p2c, struct pollfd *p2cIn, Card *card) {
    while (write(p2c, card, sizeof(Card)) < 0) {};
    while (poll(p2cIn, 1, 32) != 0) {};
    return;
}


void cDrawCard(int p2c, Deck *hand) {
    Card *buf = malloc(BUF_SIZE);
    int n = -1;
    while (n < 0) {
        n = read(p2c, buf, BUF_SIZE);
    }
    n = n / sizeof(Card);
    for (int i = 0; i < n; i++) {
        push(hand, buf[i]);
    }
    free(buf);
    return;
}

void cPlayCard(int c2p, struct pollfd *c2pIn, Deck *hand, int pos) {
    Card *buf = malloc(sizeof(Card));
    *buf = hand->cards[pos];
    while (write(c2p, buf, sizeof(Card)) < 0) {};
    free(buf);
    while (poll(c2pIn, 1, 32) != 0) {};

    Deck *temp = newDeck(DECK_SIZE);
    while (hand->top > -1) {
        if (hand->top == pos) {
            pop(hand);
        }
        else {
            push(temp, pop(hand));
        }
    }

    while (temp->top > -1) {
        push(hand, pop(temp));
    }
    return;
}

/***********************
* Operaciones de juego *
************************/

void nextTurn(int *next, int turnOrder) {
    *next += turnOrder;
    switch (*next) {
    case 4:
        *next = 0;
        break;
    case -1:
        *next = 3;
        break;
    default:
        break;
    }
}

