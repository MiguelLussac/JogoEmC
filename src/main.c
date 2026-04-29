#include "raylib.h"
#include "resource_dir.h"	// cabeçalho utilitário para SearchAndSetResourceDir
#include "player.h"
#include "boss.h"
#include <stdbool.h>

#define MAX_BULLETS 10
int main ()
{
	// Diz à janela para usar vsync e funcionar em displays de alta DPI
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Cria a Janela do Jogo 
	InitWindow(800, 600, "MindDrop");
	// Define o FPS Maximo que o jogo pode chegar
	SetTargetFPS(60);

	// Função utilitária de resource_dir.h para encontrar a pasta de recursos e defini-la como diretório de trabalho atual para carregar dela
	SearchAndSetResourceDir("resources");

	// Inicialização do Jogador
	Player jogador;
	jogador.posicaoX = 400;
	jogador.velocidade = 300; // pixels por segundo

	// Inicialização das Balas
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

	// Carrega uma textura do diretório de recursos
	Texture wabbit = LoadTexture("heart.png");
	
	// loop do jogo
	while (!WindowShouldClose())
	{
		// 1. Tratamento de Eventos e Delta Time
		float deltaTime = GetFrameTime(); // Captura o tempo do frame
		//2. Atualizando Posições
		
		moverEsquerdaDireita(&jogador, deltaTime);

		// Atualiza a posição das balas
		moverBalas(bala, MAX_BULLETS, deltaTime);

		// Verifica se o jogador atirou
		atirar(&jogador, bala, MAX_BULLETS);

		// Atualiza a posição do boss
		moverBoss(&boss, deltaTime);

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

		// Desenha o jogador
		drawPlayer(&jogador);

		// Desenha as balas
		drawBalas(bala, MAX_BULLETS);

		// Desenha o boss
		drawBoss(&boss);
		
		/*Debug*/
		
		// Mostra se ao menos uma bala está ativa
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

		// Desenha o texto "Movimentação Inicial" no canto superior esquerdo da tela
		DrawText("Movimentação Inicial", 50,50,20,WHITE);

		//
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
