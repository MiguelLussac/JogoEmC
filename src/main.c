#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "player/player.h"
#include "boss/boss.h"
#include "game/questions.h"
#include "game/partida.h"
#include "game/history.h"
#include <stdbool.h>
#include <math.h>

// Ponto unico para encerrar a partida.
static void solicitarFimDeJogo(bool* jogoEncerrado, MotivoFimJogo* motivoFimJogo, MotivoFimJogo motivo) {
    if (*jogoEncerrado) return;

    *motivoFimJogo = motivo;
    *jogoEncerrado = true;
}

static const char* gerarDiagnostico(const EstatisticasPartida* stats) {
    float precisao = stats->tirosAtirados > 0 ? ((float)stats->acertosNoBoss * 100.0f) / (float)stats->tirosAtirados : 0.0f;
    if (precisao < 20.0f) return "DICA: ajuste o timing dos tiros para subir a precisao.";
    if (stats->desafiosIniciados > 0 && stats->desafiosVencidos == 0) return "DICA: use buscas por intervalo no desafio numerico.";
    if (stats->tempoPartida > 90.0f) return "DICA: pressione o boss com movimentacao agressiva e tiro continuo.";
    return "DICA: bom controle geral! Tente concluir em menos tempo.";
}

static void drawFundoArcadeAnimado(float tempo) {
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){8, 8, 20, 255}, (Color){24, 8, 35, 255});

    for (int y = 0; y < GetScreenHeight(); y += 4) {
        DrawLine(0, y, GetScreenWidth(), y, (Color){255, 255, 255, 8});
    }

    for (int i = 0; i < 48; i++) {
        float x = (float)((i * 173) % GetScreenWidth());
        float movimento = fmodf((tempo * (22.0f + (float)(i % 8) * 6.0f)) + (float)(i * 19), (float)GetScreenHeight());
        float y = GetScreenHeight() - movimento;
        Color cor = (i % 2 == 0) ? (Color){255, 255, 255, 130} : (Color){255, 225, 80, 130};
        DrawRectangle((int)x, (int)y, 2, 2, cor);
    }
}

static void drawBotaoPixel(Rectangle botao, const char* texto, bool selecionado) {
    Color fundo = selecionado ? (Color){245, 200, 60, 255} : (Color){40, 40, 58, 255};
    Color sombra = selecionado ? (Color){185, 135, 20, 255} : (Color){18, 18, 28, 255};
    Color textoCor = selecionado ? BLACK : RAYWHITE;
    DrawRectangle((int)botao.x + 5, (int)botao.y + 5, (int)botao.width, (int)botao.height, sombra);
    DrawRectangleRec(botao, fundo);
    DrawRectangleLinesEx(botao, 3.0f, selecionado ? WHITE : (Color){120, 120, 160, 255});
    DrawText(texto,
             (int)(botao.x + (botao.width - MeasureText(texto, 24)) / 2.0f),
             (int)botao.y + 13,
             24,
             textoCor);
}

static void drawMenuInicial(int opcaoSelecionada, float tempo) {
    drawFundoArcadeAnimado(tempo);

    const char* titulo = "MINDDROP";
    const char* subtitulo = "ARCADE REPORT EDITION";
    DrawText(titulo, (GetScreenWidth() - MeasureText(titulo, 74)) / 2, 88, 74, (Color){255, 230, 80, 255});
    DrawText(subtitulo, (GetScreenWidth() - MeasureText(subtitulo, 24)) / 2, 170, 24, (Color){255, 255, 255, 200});

    Rectangle botaoJogar = {(GetScreenWidth() - 390) / 2.0f, 250, 390, 54};
    Rectangle botaoHistorico = {(GetScreenWidth() - 390) / 2.0f, 324, 390, 54};
    drawBotaoPixel(botaoJogar, "INICIAR PARTIDA", opcaoSelecionada == 0);
    drawBotaoPixel(botaoHistorico, "VER HISTORICO", opcaoSelecionada == 1);

    Color blink = ((int)(tempo * 3.5f) % 2 == 0) ? (Color){255, 255, 255, 245} : (Color){255, 220, 90, 255};
    const char* dicas = "W/S ou SETAS | ENTER para confirmar";
    DrawText(dicas, (GetScreenWidth() - MeasureText(dicas, 18)) / 2, 410, 18, blink);
}

static void drawTelaHistorico(char linhasHistorico[][180], int totalLinhas, int scroll, float tempo) {
    drawFundoArcadeAnimado(tempo);
    const char* titulo = "HISTORICO DE RELATORIOS";
    DrawText(titulo, (GetScreenWidth() - MeasureText(titulo, 38)) / 2, 40, 38, (Color){255, 220, 75, 255});

    Rectangle painel = {56, 105, (float)GetScreenWidth() - 112.0f, 420};
    DrawRectangleRec(painel, (Color){12, 12, 20, 220});
    DrawRectangleLinesEx(painel, 3.0f, (Color){170, 170, 215, 255});

    if (totalLinhas == 0) {
        const char* vazio = "Sem relatorios salvos ainda. Jogue uma partida!";
        DrawText(vazio, (GetScreenWidth() - MeasureText(vazio, 24)) / 2, 290, 24, RAYWHITE);
    } else {
        int inicio = scroll;
        int fim = inicio + 16;
        if (fim > totalLinhas) fim = totalLinhas;
        int y = 130;
        for (int i = inicio; i < fim; i++) {
            DrawText(linhasHistorico[i], 78, y, 18, (Color){235, 235, 235, 245});
            y += 24;
        }
    }

    const char* rodape = "ESC para voltar | PgUp/PgDn para rolar";
    DrawText(rodape, (GetScreenWidth() - MeasureText(rodape, 18)) / 2, 546, 18, (Color){220, 220, 220, 210});
}

static void drawRelatorioFinal(MotivoFimJogo motivoFimJogo, const EstatisticasPartida* stats, bool salvo) {
    Rectangle modal = {
        (GetScreenWidth() - 620) / 2.0f,
        (GetScreenHeight() - 320) / 2.0f,
        620,
        320
    };

    float precisao = stats->tirosAtirados > 0 ? ((float)stats->acertosNoBoss * 100.0f) / (float)stats->tirosAtirados : 0.0f;
    float aproveitamento = stats->desafiosIniciados > 0
        ? ((float)stats->desafiosVencidos * 100.0f) / (float)stats->desafiosIniciados
        : 0.0f;

    const char* titulo = "RELATORIO FINAL";
    const char* motivo = textoMotivoFimJogo(motivoFimJogo);
    const char* salvar = salvo ? "Relatorio salvo no historico." : "Salvando no historico...";
    const char* sair = "ENTER ou ESC para voltar ao menu";
    const char* dica = gerarDiagnostico(stats);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 165});
    DrawRectangleRec(modal, (Color){18, 18, 18, 245});
    DrawRectangleLinesEx(modal, 3.0f, YELLOW);
    DrawText(titulo, (int)(modal.x + (modal.width - MeasureText(titulo, 32)) / 2), (int)modal.y + 24, 32, YELLOW);
    DrawText(motivo, (int)(modal.x + (modal.width - MeasureText(motivo, 22)) / 2), (int)modal.y + 67, 22, WHITE);

    DrawText(TextFormat("Tempo de partida: %.1fs", stats->tempoPartida), (int)modal.x + 36, (int)modal.y + 116, 20, RAYWHITE);
    DrawText(TextFormat("Tiros no boss: %d/%d (%.1f%%)", stats->acertosNoBoss, stats->tirosAtirados, precisao), (int)modal.x + 36, (int)modal.y + 146, 20, RAYWHITE);
    DrawText(TextFormat("Desafios vencidos: %d/%d (%.1f%%)", stats->desafiosVencidos, stats->desafiosIniciados, aproveitamento), (int)modal.x + 36, (int)modal.y + 176, 20, RAYWHITE);
    DrawText(dica, (int)modal.x + 36, (int)modal.y + 214, 18, (Color){190, 255, 190, 245});
    DrawText(salvar, (int)modal.x + 36, (int)modal.y + 245, 16, (Color){185, 185, 185, 250});
    DrawText(sair, (int)(modal.x + (modal.width - MeasureText(sair, 16)) / 2), (int)modal.y + 278, 16, GRAY);
}

int main () {
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    // Create the game window
    InitWindow(800, 600, "MindDrop");
    SetTargetFPS(60);
    // Set the resources directory as the working directory
    SearchAndSetResourceDir("resources");

	Player jogador;
    Bullet bala[MAX_BULLETS];
	Boss boss;
	BossBullet balasBoss[MAX_BOSS_BULLETS];
	Estrela estrela;
	DesafioPergunta desafio;
	bool perguntaAtiva = false;
	bool jogoEncerrado = false;
	MotivoFimJogo motivoFimJogo = FIM_JOGO_NENHUM;
    EstatisticasPartida stats;
    TelaJogo telaAtual = TELA_MENU;
    int opcaoMenu = 0;
    char linhasHistorico[MAX_HISTORICO_LINHAS][TAMANHO_LINHA_HISTORICO];
    int totalHistorico = 0;
    int scrollHistorico = 0;
    bool relatorioSalvo = false;

    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);

    // Loop Principal
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float tempo = (float)GetTime();

        if (telaAtual == TELA_MENU) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                opcaoMenu = (opcaoMenu + 1) % 2;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (opcaoMenu == 0) {
                    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);
                    relatorioSalvo = false;
                    telaAtual = TELA_JOGO;
                } else {
                    totalHistorico = carregarHistoricoRelatorios(linhasHistorico, MAX_HISTORICO_LINHAS);
                    scrollHistorico = 0;
                    telaAtual = TELA_HISTORICO;
                }
            }
        } else if (telaAtual == TELA_HISTORICO) {
            if (IsKeyPressed(KEY_PAGE_DOWN) && scrollHistorico + 16 < totalHistorico) scrollHistorico++;
            if (IsKeyPressed(KEY_PAGE_UP) && scrollHistorico > 0) scrollHistorico--;
            if (IsKeyPressed(KEY_ESCAPE)) telaAtual = TELA_MENU;
        } else if (telaAtual == TELA_JOGO) {
            stats.tempoPartida += deltaTime;

            int tirosAtivosAntes = 0;
            int tirosAtivosDepois = 0;
            int hpBossAntes = boss.hp;
            bool desafioAtivoAntes = perguntaAtiva;

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bala[i].ativa) tirosAtivosAntes++;
            }

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
                // A morte do player usa o mesmo gancho que abre o relatório.
                if (jogador.hp <= 0) {
                    solicitarFimDeJogo(&jogoEncerrado, &motivoFimJogo, FIM_JOGO_PLAYER_DERROTADO);
                }
                atualizarFeedbackDanoPlayer(&jogador, deltaTime);

                // Verifica colisão da estrela com o jogador
                if (!jogoEncerrado && atualizarEstrela(&estrela, &boss, &jogador, deltaTime)) {
                    // Coletar a estrela congela o combate e abre o desafio numérico.
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

            if (!desafioAtivoAntes && perguntaAtiva) stats.desafiosIniciados++;
            if (desafioAtivoAntes && !perguntaAtiva && desafio.acertou) stats.desafiosVencidos++;

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bala[i].ativa) tirosAtivosDepois++;
            }
            if (tirosAtivosDepois > tirosAtivosAntes) stats.tirosAtirados += (tirosAtivosDepois - tirosAtivosAntes);

            if (hpBossAntes > boss.hp) stats.acertosNoBoss += (hpBossAntes - boss.hp);

            if (jogoEncerrado) {
                if (!relatorioSalvo) {
                    salvarRelatorioHistorico(motivoFimJogo, &stats);
                    relatorioSalvo = true;
                }
                telaAtual = TELA_RELATORIO_FINAL;
            }
        } else if (telaAtual == TELA_RELATORIO_FINAL) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                telaAtual = TELA_MENU;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (telaAtual == TELA_MENU) {
            drawMenuInicial(opcaoMenu, tempo);
        } else if (telaAtual == TELA_HISTORICO) {
            drawTelaHistorico(linhasHistorico, totalHistorico, scrollHistorico, tempo);
        } else {
            // ── DRAW ── sempre desenha (jogo congelado, mas visível) ───────────
            drawPlayer(&jogador);
            drawBalas(bala, MAX_BULLETS);
            drawBoss(&boss);
            drawBarraVidaBoss(&boss);
            drawBalasBoss(balasBoss, MAX_BOSS_BULLETS);
            drawPlayerHP(&jogador);
            drawEstrela(&estrela);

            if (perguntaAtiva) {
                drawDesafio(&desafio, &jogador);
            }

            DrawText(TextFormat("Tempo: %.1fs", stats.tempoPartida), 20, 20, 20, WHITE);

            if (telaAtual == TELA_RELATORIO_FINAL) {
                drawRelatorioFinal(motivoFimJogo, &stats, relatorioSalvo);
            }
        }

        EndDrawing();
    }
	CloseWindow();
	return 0;
}
