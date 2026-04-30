#include "boss.h"
#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BOSS_BULLET_RADIUS 6

void inicializarBoss(Boss* boss) {
    boss->posicaoX = 400.0f;
    boss->posicaoY = 100.0f;
    boss->velocidade = 150.0f;
    boss->largura = 80.0f;
    boss->altura = 80.0f;
    boss->hp = 10;
    boss->ativa = true;
    boss->direcao = 1;
    boss->tempoDesdeUltimoTiro = 0.0f;
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

void inicializarBalasBoss(BossBullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        bullets[i].posicaoX = 0.0f;
        bullets[i].posicaoY = 0.0f;
        bullets[i].velocidadeX = 0.0f;
        bullets[i].velocidadeY = 0.0f;
        bullets[i].ativa = false;
    }
}

void atualizarTiroBoss(Boss* boss, BossBullet bullets[], int count, const Player* player, float deltaTime) {
    if (!boss->ativa) return;

    boss->tempoDesdeUltimoTiro += deltaTime;
    if (boss->tempoDesdeUltimoTiro < BOSS_BULLET_COOLDOWN) return;

    for (int i = 0; i < count; i++) {
        if (!bullets[i].ativa) {
            float direcaoX = player->posicaoX - boss->posicaoX;
            float direcaoY = player->posicaoY - boss->posicaoY;
            float tamanhoDirecao = sqrtf(direcaoX * direcaoX + direcaoY * direcaoY);

            if (tamanhoDirecao <= 0.0f) return;

            bullets[i].posicaoX = boss->posicaoX;
            bullets[i].posicaoY = boss->posicaoY + (boss->altura / 2.0f);
            bullets[i].velocidadeX = (direcaoX / tamanhoDirecao) * BOSS_BULLET_SPEED;
            bullets[i].velocidadeY = (direcaoY / tamanhoDirecao) * BOSS_BULLET_SPEED;
            bullets[i].ativa = true;
            boss->tempoDesdeUltimoTiro = 0.0f;
            break;
        }
    }
}

void moverBalasBoss(BossBullet bullets[], int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            bullets[i].posicaoX += bullets[i].velocidadeX * deltaTime;
            bullets[i].posicaoY += bullets[i].velocidadeY * deltaTime;

            if (bullets[i].posicaoX < 0.0f || bullets[i].posicaoX > SCREEN_WIDTH ||
                bullets[i].posicaoY < 0.0f || bullets[i].posicaoY > SCREEN_HEIGHT) {
                bullets[i].ativa = false;
            }
        }
    }
}

void drawBalasBoss(BossBullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            DrawCircle((int)bullets[i].posicaoX, (int)bullets[i].posicaoY, BOSS_BULLET_RADIUS, ORANGE);
        }
    }
}
