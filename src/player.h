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

void moverBala(Bullet* bullet);
void drawBullet(Bullet* bullet);
void atirar(Player* player, Bullet* bullet);

#endif // PLAYER_H