#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "player/player.h"
#include "boss/boss.h"
#include "game/questions.h"
#include <stdbool.h>

#define MAX_BULLETS 10
#define MAX_BOSS_BULLETS 10
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
	bool perguntaAtiva = false; // true quando o jogador pegou a estrela

    // Loop Principal
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        // ── UPDATE ── só roda quando o jogo não está pausado ──────────────
        if (!perguntaAtiva) {
            moverEsquerdaDireita(&jogador, deltaTime);
            moverBalas(bala, MAX_BULLETS, deltaTime);
            atirar(&jogador, bala, MAX_BULLETS);

            // Atualiza o boss
            moverBoss(&boss, deltaTime);
            atualizarFeedbackDanoBoss(&boss, deltaTime);
            verificarColisaoBalasComBoss(&boss, bala, MAX_BULLETS);
            atualizarTiroBoss(&boss, balasBoss, MAX_BOSS_BULLETS, &jogador, deltaTime);
            moverBalasBoss(balasBoss, MAX_BOSS_BULLETS, deltaTime);
            verificarColisaoBalasComPlayer(&jogador, balasBoss, MAX_BOSS_BULLETS);
            atualizarFeedbackDanoPlayer(&jogador, deltaTime);

            // Verifica colisão da estrela com o jogador
            perguntaAtiva = atualizarEstrela(&estrela, &boss, &jogador, deltaTime);
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
            // Fundo semi-transparente sobre toda a tela
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                          (Color){0, 0, 0, 150});
            // Modal
            DrawRectangleRounded((Rectangle){150, 175, 500, 250}, 0.1f, 8,
                                 (Color){20, 20, 50, 240});
            DrawRectangleRoundedLines((Rectangle){150, 175, 500, 250}, 0.1f, 8,
                                      YELLOW);
            DrawText("Voce pegou a estrela!", 218, 220, 24, YELLOW);
            DrawText("[Sistema de perguntas em breve]", 185, 275, 18, WHITE);
            DrawText("Pressione ENTER para continuar", 188, 360, 18, LIGHTGRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                perguntaAtiva = false;
            }
        }

        /*Debug*/
        bool algumaAtiva = false;
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bala[i].ativa) { algumaAtiva = true; break; }
        }
        DrawText(algumaAtiva ? "Balas ativas!" : "Nenhuma bala ativa",
                 50, 80, 20, algumaAtiva ? GREEN : RED);
        DrawText("Movimentacao Inicial", 50, 50, 20, WHITE);

        EndDrawing();
    }
	CloseWindow();
	return 0;
}
