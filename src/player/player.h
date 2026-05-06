#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define PLAYER_BULLET_RADIUS 5
#define PLAYER_RADIUS 20
#define PLAYER_MAX_HP 3
#define PLAYER_HEALTH_BAR_WIDTH 220
#define PLAYER_HEALTH_BAR_HEIGHT 18
#define PLAYER_HEALTH_BAR_X 20
#define PLAYER_HEALTH_BAR_Y 560
#define PLAYER_DAMAGE_FLASH_DURATION 0.35f

typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    int hp;
    float tempoPiscandoDano;
} Player;

void moverEsquerdaDireita(Player* player, float deltaTime);
void drawPlayer(Player* player);
void drawPlayerHP(const Player* player);
void aplicarDanoPlayer(Player* player, int dano);
void atualizarFeedbackDanoPlayer(Player* player, float deltaTime);

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
