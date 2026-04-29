#include "player.h"
#include "raylib.h"

void moverEsquerdaDireita(Player* player) {
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->posicaoX -= player->velocidade;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->posicaoX += player->velocidade;
    }
    if (player->posicaoX < 20) player->posicaoX = 20;
    if (player->posicaoX > 780) player->posicaoX = 780;
}

void drawPlayer(Player* player) {
    DrawCircle(player->posicaoX, 500, 20, WHITE);
}

void moverBalas(Bullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            bullets[i].posicaoY -= bullets[i].velocidade;
            if (bullets[i].posicaoY < 0) {
                bullets[i].ativa = false;
            }
        }
    }
}

void drawBalas(Bullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            DrawCircle(bullets[i].posicaoX, bullets[i].posicaoY, 5, YELLOW);
        }
    }
}

void atirar(Player* player, Bullet bullets[], int count) {
    if (IsKeyPressed(KEY_SPACE)) {
        for (int i = 0; i < count; i++) {
            if (!bullets[i].ativa) {
                bullets[i].posicaoX = player->posicaoX;
                bullets[i].posicaoY = 480; // Posição inicial da bala
                bullets[i].velocidade = 10;
                bullets[i].ativa = true;
                break;
            }
        }
    }
}
    