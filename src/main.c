#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "player/player.h"
#include "boss/boss.h"
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

    // Loop Principal
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        moverEsquerdaDireita(&jogador, deltaTime);
        moverBalas(bala, MAX_BULLETS, deltaTime);
        atirar(&jogador, bala, MAX_BULLETS);
        moverBoss(&boss, deltaTime);

		// Atualiza a posição do boss
		moverBoss(&boss, deltaTime);
		atualizarFeedbackDanoBoss(&boss, deltaTime);
		verificarColisaoBalasComBoss(&boss, bala, MAX_BULLETS);
		atualizarTiroBoss(&boss, balasBoss, MAX_BOSS_BULLETS, &jogador, deltaTime);
		moverBalasBoss(balasBoss, MAX_BOSS_BULLETS, deltaTime);
		verificarColisaoBalasComPlayer(&jogador, balasBoss, MAX_BOSS_BULLETS);
		atualizarFeedbackDanoPlayer(&jogador, deltaTime);

        drawPlayer(&jogador);
        drawBalas(bala, MAX_BULLETS);
        drawBoss(&boss);
        drawPlayerHP(&jogador);

        bool algumaAtiva = false;
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bala[i].ativa) {
                algumaAtiva = true;
                break;
            }
        }

		// Desenha o boss
		drawBoss(&boss);
		drawBarraVidaBoss(&boss);
		drawBalasBoss(balasBoss, MAX_BOSS_BULLETS);
		
		/*Debug*/
		
		// Mostra se ao menos uma bala está ativa
		algumaAtiva = false;
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bala[i].ativa) {
				algumaAtiva = true;
				break;
			}
		}
		if (algumaAtiva) {
			DrawText("Balas ativas!", 50, 80, 20, GREEN);
		} else {
			DrawText("Nenhuma bala ativa", 50, 80, 20, RED);
		}

        DrawText("Movimentacao Inicial", 50, 50, 20, WHITE);
        DrawTriangle(
            (Vector2){380.0f, 520.0f},
            (Vector2){400.0f, 500.0f},
            (Vector2){420.0f, 500.0f},
            VIOLET
        );

        EndDrawing();
    }
	CloseWindow();
	return 0;
}
