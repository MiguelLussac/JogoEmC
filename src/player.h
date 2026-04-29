#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct {
    int posicaoX;
    int velocidade;
} Player;

void moverEsquerdaDireita(Player* player);
void drawPlayer(Player* player);

typedef struct {
    int posicaoX;
    int posicaoY;
    int velocidade;
    bool ativa;
} Bullet;

void moverBalas(Bullet bullets[], int count);
void drawBalas(Bullet bullets[], int count);
void atirar(Player* player, Bullet bullets[], int count);

#endif // PLAYER_H