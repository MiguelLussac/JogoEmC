#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "./player/player.h"

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Cria a Janela do Jogo 
	InitWindow(800, 600, "MindDrop");
	// Define o FPS Maximo que o jogo pode chegar
	SetTargetFPS(60);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	Player jogador;
	jogador.posicaoX = 400;
	jogador.velocidade = 5;

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("heart.png");
	
	// game loop
	while (!WindowShouldClose())
	{
		// 1. Event Handling

		//2. Updating Positions
		moverEsquerdaDireita(&jogador);
		
		// 3. Drawing
		BeginDrawing();
		ClearBackground(BLACK);
		int rows = 60;
		int cols = 80;
		int cellSize = 10;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				// Draw lines for each cell
				DrawRectangleLines(j * cellSize, i * cellSize, cellSize, cellSize, LIGHTGRAY);
			}
		}
		/* Inicio do Desenho */
		drawPlayer(&jogador);
		
		DrawCircle(380, 500, 5, PINK);
		DrawCircle(400, 480, 5, PINK);
		DrawText("Movimentação Inicial", 50,50,20,WHITE);
		
		DrawTriangle((Vector2){ 380.0f, 520.0f }, // Final do Triangulo
				   (Vector2){ 400.0f, 500.0f }, 
				   (Vector2){ 420.0f, 500.0f }, VIOLET);

		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	// UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
