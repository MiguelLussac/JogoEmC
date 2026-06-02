#ifndef PARTIDA_H
#define PARTIDA_H

#include <stdbool.h>
#include "../player/player.h"
#include "../boss/boss.h"
#include "questions.h"

#define MAX_BULLETS 10
#define MAX_BOSS_BULLETS 96

typedef enum {
    FIM_JOGO_NENHUM,
    FIM_JOGO_BOSS_DERROTADO,
    FIM_JOGO_PLAYER_DERROTADO
} MotivoFimJogo;

typedef enum {
    TELA_MENU,
    TELA_SELECAO_MODO,
    TELA_HISTORICO,
    TELA_RELATORIO_ANALITICO,
    TELA_JOGO,
    TELA_RELATORIO_FINAL
} TelaJogo;

typedef enum {
    MODO_ARCADE,
    MODO_LOGICO
} ModoJogo;

typedef struct {
    ModoJogo modo;
    int tirosAtirados;
    int acertosNoBoss;
    int desafiosIniciados;
    int desafiosVencidos;
    float tempoPartida;
    int logicAcertos;
    int logicErros;
    int logicComboMax;
    int logicPowerUps;
    int logicBuffs;
    int arcadeBuffs;
} EstatisticasPartida;

void inicializarPartida(Player* jogador, Bullet balasJogador[], Boss* boss, BossBullet balasBoss[],
                        Estrela* estrela, DesafioPergunta* desafio, bool* perguntaAtiva,
                        bool* jogoEncerrado, MotivoFimJogo* motivoFimJogo, EstatisticasPartida* stats);

const char* textoMotivoFimJogo(MotivoFimJogo motivoFimJogo);

#endif
