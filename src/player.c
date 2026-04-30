#include "player.h"
#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_RADIUS 20

void moverEsquerdaDireita(Player* player, float deltaTime) {
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->posicaoX -= player->velocidade * deltaTime;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->posicaoX += player->velocidade * deltaTime;
    }
    if (player->posicaoX < PLAYER_RADIUS) player->posicaoX = PLAYER_RADIUS;
    if (player->posicaoX > SCREEN_WIDTH - PLAYER_RADIUS) player->posicaoX = SCREEN_WIDTH - PLAYER_RADIUS;
}

void drawPlayer(Player* player) {
    DrawCircle((int)player->posicaoX, (int)player->posicaoY, PLAYER_RADIUS, WHITE);
}

void moverBalas(Bullet bullets[], int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            bullets[i].posicaoY -= bullets[i].velocidade * deltaTime;
            if (bullets[i].posicaoY < 0) {
                bullets[i].ativa = false;
            }
        }
    }
}

void drawBalas(Bullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            DrawCircle((int)bullets[i].posicaoX, (int)bullets[i].posicaoY, PLAYER_BULLET_RADIUS, YELLOW);
        }
    }
}

void atirar(Player* player, Bullet bullets[], int count) {
    if (IsKeyPressed(KEY_SPACE)) {
        for (int i = 0; i < count; i++) {
            if (!bullets[i].ativa) {
                bullets[i].posicaoX = player->posicaoX;
                bullets[i].posicaoY = player->posicaoY - PLAYER_RADIUS; // Posição inicial da bala
                bullets[i].velocidade = 500.0f; // Velocidade aumentada para balas mais rápidas
                bullets[i].ativa = true;
                break;
            }
        }
    }
}
