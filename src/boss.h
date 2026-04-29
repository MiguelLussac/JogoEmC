#ifndef BOSS_H
#define BOSS_H

#include <stdbool.h>

typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    float largura;
    float altura;
    int hp;
    bool ativa;
    int direcao; // 0 = parado, 1 = direita, -1 = esquerda
} Boss;

void inicializarBoss(Boss* boss);
void moverBoss(Boss* boss, float deltaTime);
void drawBoss(Boss* boss);

#endif
