#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <stdbool.h>
#include "raylib.h"
#include "../player/player.h"
#include "../boss/boss.h"

// --- Configurações da Estrela ---
#define ESTRELA_INTERVALO    10.0f   // segundos entre aparições
#define ESTRELA_VELOCIDADE   150.0f  // pixels por segundo (queda)
#define ESTRELA_RAIO         14      // raio de colisão/desenho
#define ESTRELA_PONTAS       5       // número de pontas da estrela

typedef struct {
    float x;
    float y;
    bool  ativa;          // true = estrela visível e caindo
    float timerAparição;  // conta até ESTRELA_INTERVALO para spawnar
} Estrela;

// Inicializa a estrela (inativa, timer zerado)
void inicializarEstrela(Estrela* estrela);

// Atualiza timer, spawna (na posição do boss) e move a estrela; retorna true se colidiu com o jogador
bool atualizarEstrela(Estrela* estrela, const Boss* boss, const Player* jogador, float deltaTime);

// Desenha a estrela na tela
void drawEstrela(const Estrela* estrela);

#endif // QUESTIONS_H
