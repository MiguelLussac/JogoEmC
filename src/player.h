#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct {
    float posicaoX;
    float velocidade;
} Player;

void moverEsquerdaDireita(Player* player, float deltaTime);
void drawPlayer(Player* player);

typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    bool ativa;
} Bullet;

void moverBalas(Bullet bullets[], int count, float deltaTime);
void drawBalas(Bullet bullets[], int count);
void atirar(Player* player, Bullet bullets[], int count);

#endif // PLAYER_H