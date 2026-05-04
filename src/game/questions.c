#include "questions.h"
#include "raylib.h"
#include <math.h>

// -------------------------------------------------------
// Funções auxiliares
// -------------------------------------------------------

// Desenha uma estrela de N pontas centrada em (cx, cy)
static void desenharEstrelaForma(float cx, float cy, float rExterno, float rInterno, int pontas, Color cor) {
    float angulo = -PI / 2.0f; // começa no topo
    float passo  = PI / pontas;

    for (int i = 0; i < pontas * 2; i++) {
        float r  = (i % 2 == 0) ? rExterno : rInterno;
        float x1 = cx + cosf(angulo) * r;
        float y1 = cy + sinf(angulo) * r;
        angulo += passo;

        float r2 = (( i + 1) % 2 == 0) ? rExterno : rInterno;
        float x2 = cx + cosf(angulo) * r2;
        float y2 = cy + sinf(angulo) * r2;

        DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2.5f, cor);
        DrawTriangle(
            (Vector2){cx, cy},
            (Vector2){x1, y1},
            (Vector2){x2, y2},
            cor
        );
    }
}

// -------------------------------------------------------
// API pública
// -------------------------------------------------------

void inicializarEstrela(Estrela* estrela) {
    estrela->x            = 0;
    estrela->y            = 0;
    estrela->ativa        = false;
    estrela->timerAparição = 0.0f;
}

bool atualizarEstrela(Estrela* estrela, const Boss* boss, const Player* jogador, float deltaTime) {
    if (!estrela->ativa) {
        // Conta o timer até a próxima aparição
        estrela->timerAparição += deltaTime;
        if (estrela->timerAparição >= ESTRELA_INTERVALO) {
            // Spawna no centro do boss
            estrela->x = boss->posicaoX + boss->largura / 2.0f;
            estrela->y = boss->posicaoY + boss->altura / 2.0f;
            estrela->ativa = true;
            estrela->timerAparição = 0.0f;
        }
        return false;
    }

    // Move a estrela para baixo
    estrela->y += ESTRELA_VELOCIDADE * deltaTime;

    // Saiu da tela? Desativa e reinicia o timer
    if (estrela->y - ESTRELA_RAIO > GetScreenHeight()) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return false;
    }

    // Verifica colisão com o jogador (círculo vs círculo)
    float dx  = estrela->x - jogador->posicaoX;
    float dy  = estrela->y - jogador->posicaoY;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < ESTRELA_RAIO + PLAYER_RADIUS) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return true; // colidiu!
    }

    return false;
}

void drawEstrela(const Estrela* estrela) {
    if (!estrela->ativa) return;

    // Brilho externo (halo)
    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 6, (Color){255, 230, 50, 60});
    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 3, (Color){255, 220, 80, 100});

    // Corpo da estrela
    desenharEstrelaForma(estrela->x, estrela->y,
                         ESTRELA_RAIO, ESTRELA_RAIO * 0.45f,
                         ESTRELA_PONTAS, YELLOW);

    // Centro brilhante
    DrawCircle((int)estrela->x, (int)estrela->y, 4, WHITE);
}
