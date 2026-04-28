#include "player.h"
#include "raylib.h"

void moverEsquerdaDireita(Player* player) {
    if (IsKeyDown(KEY_LEFT)) {
        player->posicaoX -= player->velocidade;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player->posicaoX += player->velocidade;
    }
    if (player->posicaoX < 20) player->posicaoX = 20;
    if (player->posicaoX > 780) player->posicaoX = 780;
}

void drawPlayer(Player* player) {
    DrawCircle(player->posicaoX, 500, 20, WHITE);
}