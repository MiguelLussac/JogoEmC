#ifndef BOSS_H
#define BOSS_H

#include <stdbool.h>
#include "player.h"

#define BOSS_BULLET_COOLDOWN 1.0f
#define BOSS_BULLET_SPEED 220.0f

typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    float largura;
    float altura;
    int hp;
    bool ativa;
    int direcao; // 0 = parado, 1 = direita, -1 = esquerda
    float tempoDesdeUltimoTiro;
} Boss;

typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidadeX;
    float velocidadeY;
    bool ativa;
} BossBullet;

void inicializarBoss(Boss* boss);
void moverBoss(Boss* boss, float deltaTime);
void drawBoss(Boss* boss);
void verificarColisaoBalasComBoss(Boss* boss, Bullet bullets[], int count);
void inicializarBalasBoss(BossBullet bullets[], int count);
void atualizarTiroBoss(Boss* boss, BossBullet bullets[], int count, const Player* player, float deltaTime);
void moverBalasBoss(BossBullet bullets[], int count, float deltaTime);
void drawBalasBoss(BossBullet bullets[], int count);

#endif
