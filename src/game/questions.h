#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <stdbool.h>
#include "raylib.h"
#include "../player/player.h"
#include "../boss/boss.h"

#define NUMEROMIN 1
#define NUMEROMAX 100
#define DISTANCIA_MUITO_PERTO 5
#define DISTANCIA_PERTO 15
#define DISTANCIA_LONGE 80
#define ESTRATEGIA_MIN_DEZENAS 3

// --- Configurações da Estrela ---
#define ESTRELA_INTERVALO    10.0f   // segundos entre aparições
#define ESTRELA_VELOCIDADE   150.0f  // pixels por segundo (queda)
#define ESTRELA_RAIO         14      // raio de colisão/desenho
#define ESTRELA_PONTAS       5       // número de pontas da estrela
#define DESAFIO_MAX_TENTATIVAS 5
#define DESAFIO_TAMANHO_ENTRADA 4

typedef enum {
    passouLonge,
    muitoPerto,
    perto,
    estrategiaDezena,
    muitoAlto,
    correto,
    muitoBaixo
} StatusChute;

// Guarda o numero secreto e o resultado do ultimo chute avaliado.
typedef struct {
    int numeroSecreto;
    StatusChute status;
} Questao;

typedef struct {
    float x;
    float y;
    bool  ativa;          // true = estrela visível e caindo
    float timerAparição;  // conta até ESTRELA_INTERVALO para spawnar
} Estrela;

typedef enum {
    DESAFIO_INATIVO,
    DESAFIO_CHUTANDO,
    DESAFIO_CONFIRMAR_ULTIMA,
    DESAFIO_RESULTADO,
    DESAFIO_CONTAGEM
} EstadoDesafio;

// Estado completo do pop-up de desafio, incluindo entrada, tentativas e timers.
typedef struct {
    EstadoDesafio estado;
    Questao questao;
    int tentativasRestantes;
    int ultimoPalpite;
    int chutesTerminadosEmZero;
    char entrada[DESAFIO_TAMANHO_ENTRADA];
    int tamanhoEntrada;
    const char* dica;
    bool acertou;
    bool usouUltimaChance;
    float timerResultado;
    float timerContagem;
} DesafioPergunta;

int gerarNumeroSecreto(void);
void executaQuestao(Questao *questao, int numeroSecreto, int chute);

// Inicializa a estrela (inativa, timer zerado)
void inicializarEstrela(Estrela* estrela);

// Atualiza timer, spawna (na posição do boss) e move a estrela; retorna true se colidiu com o jogador
bool atualizarEstrela(Estrela* estrela, const Boss* boss, const Player* jogador, float deltaTime);

// Desenha a estrela na tela
void drawEstrela(const Estrela* estrela);

void inicializarDesafio(DesafioPergunta* desafio);
void iniciarDesafio(DesafioPergunta* desafio);
bool atualizarDesafio(DesafioPergunta* desafio, Player* jogador, float deltaTime);
void drawDesafio(const DesafioPergunta* desafio, const Player* jogador);

#endif // QUESTIONS_H
