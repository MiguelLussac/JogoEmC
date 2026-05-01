#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "./player/player.h"
#include "boss.h"
#include <stdbool.h>

#define MAX_BULLETS 10

int main(void)
{
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    // Create the game window
    InitWindow(800, 600, "MindDrop");
    SetTargetFPS(60);

    // Set the resources directory as the working directory
    SearchAndSetResourceDir("resources");

    Player jogador;
    jogador.posicaoX = 400;
    jogador.velocidade = 300; // pixels per second

    Bullet bala[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) {
        bala[i].posicaoX = 380;
        bala[i].posicaoY = 500;
        bala[i].velocidade = 10;
        bala[i].ativa = false;
    }

    Boss boss;
    inicializarBoss(&boss);

    Texture wabbit = LoadTexture("heart.png");

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        moverEsquerdaDireita(&jogador, deltaTime);
        moverBalas(bala, MAX_BULLETS, deltaTime);
        atirar(&jogador, bala, MAX_BULLETS);
        moverBoss(&boss, deltaTime);

        BeginDrawing();
        ClearBackground(BLACK);

        int rows = 60;
        int cols = 80;
        int cellSize = 10;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                DrawRectangleLines(j * cellSize, i * cellSize, cellSize, cellSize, LIGHTGRAY);
            }
        }

        drawPlayer(&jogador);
        drawBalas(bala, MAX_BULLETS);
        drawBoss(&boss);

        bool algumaAtiva = false;
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

    UnloadTexture(wabbit);
    CloseWindow();
    return 0;
}
