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
#define PLAYER_BASE_BULLET_DAMAGE 1
#define PLAYER_DAMAGE_BOOST_MULTIPLIER 2
#define PLAYER_DAMAGE_BOOST_DURATION 10.0f

// Estado principal do jogador usado por movimento, vida e feedback visual.
typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    int hp;
    float tempoPiscandoDano;
    int danoTiro;
    float tempoBoostDano;
} Player;

void moverEsquerdaDireita(Player* player, float deltaTime);
void drawPlayer(Player* player);
void drawPlayerHP(const Player* player);
void aplicarDanoPlayer(Player* player, int dano);
void atualizarFeedbackDanoPlayer(Player* player, float deltaTime);
void aplicarBoostDanoPlayer(Player* player);
void atualizarBoostDanoPlayer(Player* player, float deltaTime);

// Bala simples do jogador, reaproveitada em um pool fixo.
typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    int dano;
    bool ativa;
} Bullet;

void moverBalas(Bullet bullets[], int count, float deltaTime);
void drawBalas(Bullet bullets[], int count);
void atirar(Player* player, Bullet bullets[], int count);

#endif // PLAYER_H
