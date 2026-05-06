#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "player/player.h"
#include "boss/boss.h"
#include "game/questions.h"
#include <stdbool.h>

#define MAX_BULLETS 10
#define MAX_BOSS_BULLETS 10

typedef enum {
    FIM_JOGO_NENHUM,
    FIM_JOGO_BOSS_DERROTADO,
    FIM_JOGO_PLAYER_DERROTADO
} MotivoFimJogo;

// Ponto unico para encerrar a partida e, futuramente, abrir o popup de relatorio.
static void solicitarFimDeJogo(bool* jogoEncerrado, MotivoFimJogo* motivoFimJogo, MotivoFimJogo motivo) {
    if (*jogoEncerrado) return;

    *motivoFimJogo = motivo;
    *jogoEncerrado = true;
}

static const char* textoMotivoFimJogo(MotivoFimJogo motivoFimJogo) {
    switch (motivoFimJogo) {
        case FIM_JOGO_BOSS_DERROTADO: return "Boss derrotado";
        case FIM_JOGO_PLAYER_DERROTADO: return "Player derrotado";
        case FIM_JOGO_NENHUM:
        default: return "Fim de jogo";
    }
}

// Pop-up temporario do relatorio final; depois deve receber tiros, tentativas e estatisticas.
static void drawRelatorioFinal(MotivoFimJogo motivoFimJogo) {
    Rectangle modal = {
        (GetScreenWidth() - 430) / 2.0f,
        (GetScreenHeight() - 190) / 2.0f,
        430,
        190
    };

    const char* titulo = "RELATORIO - EM ATUALIZACAO";
    const char* motivo = textoMotivoFimJogo(motivoFimJogo);
    const char* sair = "Pressione ESC para sair";

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 165});
    DrawRectangleRec(modal, (Color){18, 18, 18, 245});
    DrawRectangleLinesEx(modal, 2.0f, YELLOW);
    DrawText(titulo, (int)(modal.x + (modal.width - MeasureText(titulo, 22)) / 2), (int)modal.y + 42, 22, YELLOW);
    DrawText(motivo, (int)(modal.x + (modal.width - MeasureText(motivo, 18)) / 2), (int)modal.y + 90, 18, WHITE);
    DrawText(sair, (int)(modal.x + (modal.width - MeasureText(sair, 14)) / 2), (int)modal.y + 136, 14, GRAY);
}

int main () {
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    // Create the game window
    InitWindow(800, 600, "MindDrop");
    SetTargetFPS(60);
    // Set the resources directory as the working directory
    SearchAndSetResourceDir("resources");

	// Inicialização do Jogador
	Player jogador;
	jogador.posicaoX = 400;
	jogador.posicaoY = 500;
	jogador.velocidade = 300; // pixels por segundo
	jogador.hp = PLAYER_MAX_HP;
	jogador.tempoPiscandoDano = 0.0f;

    // Prepara o pool fixo de balas do jogador para reutilizar objetos ativos/inativos.
    Bullet bala[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) {
        bala[i].posicaoX = 380;
        bala[i].posicaoY = 500;
        bala[i].velocidade = 10;
        bala[i].ativa = false;
    }

	// Inicialização do Boss
	Boss boss;
	inicializarBoss(&boss);
	BossBullet balasBoss[MAX_BOSS_BULLETS];
	inicializarBalasBoss(balasBoss, MAX_BOSS_BULLETS);

	// Inicialização da Estrela (sistema de perguntas)
	Estrela estrela;
	inicializarEstrela(&estrela);
	// O desafio fica separado da estrela para manter o modal pausado ate terminar.
	DesafioPergunta desafio;
	inicializarDesafio(&desafio);
	bool perguntaAtiva = false; // true quando o jogador pegou a estrela
	bool jogoEncerrado = false; // true quando uma condicao de fim de jogo for atingida
	MotivoFimJogo motivoFimJogo = FIM_JOGO_NENHUM;

    // Loop Principal
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        // ── UPDATE ── só roda quando o jogo não está pausado ──────────────
        if (!jogoEncerrado && !perguntaAtiva) {
            moverEsquerdaDireita(&jogador, deltaTime);
            moverBalas(bala, MAX_BULLETS, deltaTime);
            atirar(&jogador, bala, MAX_BULLETS);

            // Atualiza o boss
            moverBoss(&boss, deltaTime);
            atualizarFeedbackDanoBoss(&boss, deltaTime);
            verificarColisaoBalasComBoss(&boss, bala, MAX_BULLETS);
            // Quando o boss fica inativo por HP zerado, registra o fim da partida.
            if (!boss.ativa) {
                solicitarFimDeJogo(&jogoEncerrado, &motivoFimJogo, FIM_JOGO_BOSS_DERROTADO);
            }
            atualizarTiroBoss(&boss, balasBoss, MAX_BOSS_BULLETS, &jogador, deltaTime);
            moverBalasBoss(balasBoss, MAX_BOSS_BULLETS, deltaTime);
            verificarColisaoBalasComPlayer(&jogador, balasBoss, MAX_BOSS_BULLETS);
            // A morte do player usa o mesmo gancho que futuramente abrira o relatorio.
            if (jogador.hp <= 0) {
                solicitarFimDeJogo(&jogoEncerrado, &motivoFimJogo, FIM_JOGO_PLAYER_DERROTADO);
            }
            atualizarFeedbackDanoPlayer(&jogador, deltaTime);

            // Verifica colisão da estrela com o jogador
            if (!jogoEncerrado && atualizarEstrela(&estrela, &boss, &jogador, deltaTime)) {
                // Coletar a estrela congela o combate e abre o desafio numerico.
                iniciarDesafio(&desafio);
                perguntaAtiva = true;
            }
        } else if (!jogoEncerrado) {
            // Enquanto o modal esta ativo, so o desafio recebe update.
            perguntaAtiva = atualizarDesafio(&desafio, &jogador, deltaTime);
            if (jogador.hp <= 0) {
                solicitarFimDeJogo(&jogoEncerrado, &motivoFimJogo, FIM_JOGO_PLAYER_DERROTADO);
            }
        }

        // ── DRAW ── sempre desenha (jogo congelado, mas visível) ───────────
        drawPlayer(&jogador);
        drawBalas(bala, MAX_BULLETS);
        drawBoss(&boss);
        drawBarraVidaBoss(&boss);
        drawBalasBoss(balasBoss, MAX_BOSS_BULLETS);
        drawPlayerHP(&jogador);
        drawEstrela(&estrela);

        // ── TELA DE PERGUNTA (modal sobre o jogo congelado) ───────────────
        if (perguntaAtiva) {
            // O jogo fica visivel ao fundo; o desafio desenha apenas a camada modal.
            drawDesafio(&desafio, &jogador);
        }

        /*Debug*/
        // Mostra no canto da tela se existe alguma bala ativa no pool do jogador.
        bool algumaAtiva = false;
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bala[i].ativa) { algumaAtiva = true; break; }
        }
        DrawText(algumaAtiva ? "Balas ativas!" : "Nenhuma bala ativa",
                 50, 80, 20, algumaAtiva ? GREEN : RED);
        DrawText("Movimentacao Inicial", 50, 50, 20, WHITE);
        if (jogoEncerrado) {
            drawRelatorioFinal(motivoFimJogo);
        }

        EndDrawing();
    }
	CloseWindow();
	return 0;
}
