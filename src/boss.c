#include "boss.h"
#include "raylib.h"

void inicializarBoss(Boss* boss) {
    boss->posicaoX = 400.0f;
    boss->posicaoY = 100.0f;
    boss->velocidade = 150.0f;
    boss->largura = 80.0f;
    boss->altura = 80.0f;
    boss->hp = 10;
    boss->ativa = true;
    boss->direcao = 1;
}

void moverBoss(Boss* boss, float deltaTime) {
    if (!boss->ativa) return;

    boss->posicaoX += boss->velocidade * boss->direcao * deltaTime;

    if (boss->posicaoX < 20.0f) {
        boss->posicaoX = 20.0f;
        boss->direcao = 1;
    }
    if (boss->posicaoX > 800.0f - boss->largura) {
        boss->posicaoX = 800.0f - boss->largura;
        boss->direcao = -1;
    }
}

void drawBoss(Boss* boss) {
    if (boss->ativa) {
        DrawRectangle(
            (int)boss->posicaoX - (int)boss->largura / 2,
            (int)boss->posicaoY - (int)boss->altura / 2,
            (int)boss->largura,
            (int)boss->altura,
            RED
        );
    }
}
