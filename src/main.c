#include "raylib.h"
#include "resource_dir.h"	// cabeçalho utilitário para SearchAndSetResourceDir
#include "player.h"

int main ()
{
	// Diz à janela para usar vsync e funcionar em displays de alta DPI
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Cria a Janela do Jogo 
	InitWindow(800, 600, "Galaga: Show do Milhão");
	// Define o FPS Maximo que o jogo pode chegar
	SetTargetFPS(60);

	// Função utilitária de resource_dir.h para encontrar a pasta de recursos e defini-la como diretório de trabalho atual para carregar dela
	SearchAndSetResourceDir("resources");

	Player jogador;
	jogador.posicaoX = 400;
	jogador.velocidade = 5;

	Bullet bala;
	bala.posicaoX = 380;
	bala.posicaoY = 500;
	bala.velocidade = 10;
	bala.ativa = false;

	// Carrega uma textura do diretório de recursos
	Texture wabbit = LoadTexture("heart.png");
	
	// loop do jogo
	while (!WindowShouldClose())
	{
		// 1. Tratamento de Eventos

		//2. Atualizando Posições
		moverEsquerdaDireita(&jogador);

		moverBala(&bala);
		
		atirar(&jogador, &bala);


		
		// 3. Desenho
		BeginDrawing();
		ClearBackground(BLACK);
		int rows = 60;
		int cols = 80;
		int cellSize = 10;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				// Desenha linhas para cada célula
				DrawRectangleLines(j * cellSize, i * cellSize, cellSize, cellSize, LIGHTGRAY);
			}
		}
		/* Inicio do Desenho */
		drawPlayer(&jogador);
		drawBullet(&bala);

		// Debug: mostra se a bala está ativa
		if (bala.ativa) {
			DrawText("Bala ativa!", 50, 80, 20, GREEN);
		} else {
			DrawText("Bala inativa", 50, 80, 20, RED);
		}

		DrawText("Movimentação Inicial", 50,50,20,WHITE);
		
		DrawTriangle((Vector2){ 380.0f, 520.0f }, // Final do Triangulo
				   (Vector2){ 400.0f, 500.0f }, 
				   (Vector2){ 420.0f, 500.0f }, VIOLET);

		EndDrawing();
	}

	// limpeza
	// descarrega nossa textura para que possa ser limpa
	// UnloadTexture(wabbit);

	// destrói a janela e limpa o contexto OpenGL
	CloseWindow();
	return 0;
}
