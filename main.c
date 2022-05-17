#include "deck.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


int main() {
    srand(time(NULL));

    struct pollfd p2cIn[4];
    struct pollfd c2pIn[4];

    int p2c[2*4];
    int c2p[2*4];
    for (int i = 0; i < 4; i++) {
        pipe(&p2c[2*i]);
        p2cIn[i].fd = p2c[2*i];
        p2cIn[i].events = POLLIN;

        pipe(&c2p[2*i]);
        c2pIn[i].fd = c2p[2*i];
        c2pIn[i].events = POLLIN;
    }

    int id = -1;
    pid_t cPid;
    pid_t pid[4];
    for (int i = 0; i < 4; i++) {
        cPid = fork();
        if (cPid == 0) {
            id = i;
            printf("Init %d\n", id);
            break;
        }
        else {
            pid[i] = cPid;
        }
    }

    int skipTurn, cardNo, newColor;
    if (id == -1) {
        Deck *deck = newDeck(DECK_SIZE);
        fillDeck(deck);

        Deck *pile = newDeck(DECK_SIZE);
        push(pile, drawCard(deck));

        int cardsInHand[4];
        for (int i = 0; i < 4; i++) {
            pDealCard(p2c[2*i + 1], &p2cIn[i], deck, 7);
            cardsInHand[i] = 7;
        }

        int go = 1;
        int next = 0;
        int turnOrder = 1;
        Card lastCard;
        while (go) {
            skipTurn = 0;
            lastCard = peek(pile);
            pSendCard(p2c[2*next + 1], &p2cIn[next], &lastCard);

            /* Pre-turno *
            **************/
            switch (lastCard.type) {
            case DRAW_2:
                pDealCard(p2c[2*next + 1], &p2cIn[next], deck, 2);
                cardsInHand[next] += 2;
                skipTurn = 1;
                pile->cards[pile->top].type = 1;
                break;
            case WILD_DRAW_4:
                pDealCard(p2c[2*next + 1], &p2cIn[next], deck, 4);
                cardsInHand[next] += 4;
                skipTurn = 1;
                pile->cards[pile->top].type = 1;
                break;
            case SKIP:
                skipTurn = 1;
                pile->cards[pile->top].type = 1;
                break;
            default:
                break;
            }

            if (!skipTurn) {
                /* Pre-jugada *
                ***************/
                printf("Pre-jugada\n");

                while (read(c2p[2*next], &cardNo, sizeof(int)) < 0) {};
                if (cardNo == 0) {
                    pDealCard(p2c[2*next + 1], &p2cIn[next], deck, 1);
                    cardsInHand[next]++;
                }
                else {
                    /* Jugada *
                    ***********/
                    printf("Jugada\n");

                    while(read(c2p[2*next], &lastCard, sizeof(Card)) < 0) {};
                    push(pile, lastCard);
                    cardsInHand[next]--;

                    /* Pos-jugada *
                    ***************/
                    printf("Pos-jugada\n");

                    switch (lastCard.type) {
                    case REVERSE:
                        turnOrder = -turnOrder;
                        break;
                    case WILD:
                    case WILD_DRAW_4:
                        while (read(c2p[2*next], &newColor, sizeof(int)) < 0) {};
                        pile->cards[pile->top].color = newColor;
                        break;
                    default:
                        break;
                    }

                    /* Pos-turno *
                    **************/
                    printf("Pos-turno\n");

                    if (cardsInHand[next] == 1) {
                        go = 0;
                        for (int i = 0; i < 4; i++) {
                            kill(pid[i], SIGKILL);
                        }

                        switch (next) {
                        case 0:
                            printf("Ganaste!\n");
                            break;
                        case 1:
                        case 2:
                        case 3:
                            printf("Ganador: jugador %d\n", next);
                            break;
                        default:
                            break;
                        }

                        return 0;
                    }
                }
            }

            nextTurn(&next, turnOrder);
            printf("Next player: %d\n", next);
        }
    }
    else {
        Deck *hand = newDeck(DECK_SIZE);
        cDrawCard(p2c[2*id], hand);

        int playPos;
        Card lastCard;
        List *validCards = newList(DECK_SIZE);
        while(1) {
            skipTurn = 0;
            while (read(p2c[2*id], &lastCard, sizeof(Card)) < 0) {};

            /* Pre-turno *
            **************/
            printf("Pre-turno jugador %d\n", id);

            switch (lastCard.type) {
            case DRAW_2:
            case WILD_DRAW_4:
                cDrawCard(p2c[2*id], hand);
                skipTurn = 1;
                break;
            case SKIP:
                skipTurn = 1;
                break;
            default:
                break;
            }
            
            if (!skipTurn) {
                /* Pre-jugada *
                ***************/
                printf("Pre-jugada jugador %d\n", id);

                getPlayable(validCards, hand, lastCard);
                cardNo = validCards->top + 1;
                cWriteInt(c2p[2*id + 1], &c2pIn[id], &cardNo);
                if (cardNo == 0) {
                    cDrawCard(p2c[2*id], hand);
                }
                else {
                    /* Jugada *
                    ***********/
                    printf("Jugada jugador %d\n", id);

                    if (id == 0) {
                        printf("Escoja el numero de su carta deseada.\n");
                        for (int i = 0; i <= validCards->top; i++) {
                            printf("%d. ", i);
                            viewCard(hand->cards[i]);
                        }
                        scanf("%d", &playPos);
                    }
                    else {
                        playPos = randInt(0, validCards->top);
                    }
                    lastCard = hand->cards[playPos];
                    cPlayCard(c2p[2*id + 1], &c2pIn[id], hand, playPos);
                    clearList(validCards);

                    /* Pos-jugada *
                    ***************/
                    printf("Pos-jugada jugador %d\n", id);

                    switch (lastCard.type) {
                    case WILD:
                    case WILD_DRAW_4:
                        if (id == 0) {
                            // TODO: Menu
                            // newColor = ...
                            newColor = randInt(0, 4);
                            cWriteInt(c2p[2*id + 1], &c2pIn[id], &newColor);
                        }
                        else {
                            newColor = randInt(0, 4);
                            cWriteInt(c2p[2*id + 1], &c2pIn[id], &newColor);
                        }
                        break;
                    default:
                        break;
                    }

                    /* Pos-turno *
                    **************/
                    printf("Pos-turno jugador %d\n", id);
                    // No hay accion necesaria
                }
            }
        }
    }


    return 0;
}