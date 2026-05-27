#ifndef BOSS_H
#define BOSS_H

#include <stdbool.h>
#include "../player/player.h"

#define BOSS_MAX_HP 60
#define BOSS_BULLET_COOLDOWN 1.05f
#define BOSS_BULLET_SPEED 190.0f
#define BOSS_DAMAGE_FLASH_DURATION 0.35f

typedef enum {
    BOSS_ATAQUE_PADRAO = 0,
    BOSS_ATAQUE_LASER,
    BOSS_ATAQUE_ONDAS,
    BOSS_ATAQUE_PAREDE,
    BOSS_ATAQUE_ESPIRAL
} BossAtaqueId;

// Estado completo do boss: posicao, vida, movimento, tiro e feedback visual.
typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidade;
    float largura;
    float altura;
    int hp;
    int hpMaximo;
    bool ativa;
    int direcao; // 0 = parado, 1 = direita, -1 = esquerda
    float tempoDesdeUltimoTiro;
    float tempoPiscandoDano;
    int fase;
    int padraoMovimento;
    int padraoAtaque;
    float tempoIA;
    float tempoMudancaMovimento;
    float tempoDash;
    float duracaoDash;
    float direcaoDash;
    int tirosRajadaRestantes;
    float tempoEntreRajadas;
    BossAtaqueId ataqueAtivo;
    float tempoAtaqueEspecial;
    int passoAtaqueEspecial;
    float laserX;
    float laserVelX;
    float laserLargura;
    float anguloEspiral;
} Boss;

// Projetil do boss com velocidade vetorial para perseguir a direcao calculada.
typedef struct {
    float posicaoX;
    float posicaoY;
    float velocidadeX;
    float velocidadeY;
    bool ativa;
} BossBullet;

void inicializarBoss(Boss* boss);
void moverBoss(Boss* boss, const Player* player, float deltaTime);
void drawBoss(Boss* boss);
void atualizarFeedbackDanoBoss(Boss* boss, float deltaTime);
void verificarColisaoBalasComBoss(Boss* boss, Bullet bullets[], int count);
void verificarColisaoBalasComPlayer(Player* player, BossBullet bullets[], int count);
void drawBarraVidaBoss(const Boss* boss);
void drawBarraVidaBossEm(const Boss* boss, int posicaoY);
void inicializarBalasBoss(BossBullet bullets[], int count);
void atualizarTiroBoss(Boss* boss, BossBullet bullets[], int count, const Player* player, float deltaTime);
void moverBalasBoss(BossBullet bullets[], int count, float deltaTime);
void drawBalasBoss(BossBullet bullets[], int count);
void drawAtaquesEspeciaisBoss(const Boss* boss);
void verificarColisaoAtaquesEspeciaisComPlayer(const Boss* boss, Player* player);

#endif
