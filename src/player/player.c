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
    int framePiscando = (int)(player->tempoPiscandoDano * 20.0f);
    Color corJogador = (player->tempoPiscandoDano > 0.0f && framePiscando % 2 == 0) ? WHITE : RED;
    DrawCircle((int)player->posicaoX, (int)player->posicaoY, PLAYER_RADIUS, corJogador);
}

void drawPlayerHP(const Player* player) {
    float percentualVida = player->hp > 0 ? (float)player->hp / (float)PLAYER_MAX_HP : 0.0f;
    int larguraVida = (int)(PLAYER_HEALTH_BAR_WIDTH * percentualVida);

    DrawRectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, DARKGRAY);
    DrawRectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, larguraVida, PLAYER_HEALTH_BAR_HEIGHT, RED);
    DrawRectangleLines(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, WHITE);
    DrawText(TextFormat("PLAYER HP: %d/%d", player->hp, PLAYER_MAX_HP), PLAYER_HEALTH_BAR_X + 8, PLAYER_HEALTH_BAR_Y + 22, 16, WHITE);
}

void aplicarDanoPlayer(Player* player, int dano) {
    player->hp -= dano;
    if (player->hp < 0) player->hp = 0;
    player->tempoPiscandoDano = PLAYER_DAMAGE_FLASH_DURATION;
}

void atualizarFeedbackDanoPlayer(Player* player, float deltaTime) {
    if (player->tempoPiscandoDano <= 0.0f) return;
    player->tempoPiscandoDano -= deltaTime;
    if (player->tempoPiscandoDano < 0.0f) player->tempoPiscandoDano = 0.0f;
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
