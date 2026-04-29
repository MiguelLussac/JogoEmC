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

void moverBala(Bullet* bullet) {
    if (bullet->ativa) {
        bullet->posicaoY -= bullet->velocidade;
        if (bullet->posicaoY < 0) {
            bullet->ativa = false;
        }
    }
}

void drawBullet(Bullet* bullet) {
    if (bullet->ativa) {
        DrawCircle(bullet->posicaoX, bullet->posicaoY, 5, YELLOW);
    }
}

void atirar(Player* player, Bullet* bullet) {
    if (IsKeyPressed(KEY_SPACE) && !bullet->ativa) {
        bullet->posicaoX = player->posicaoX;
        bullet->posicaoY = 480; // Posição inicial da bala
        bullet->ativa = true;
    }
}
    