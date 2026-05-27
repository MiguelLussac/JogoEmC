#include "raylib.h"
#include "resource_dir.h"  // Utility header for SearchAndSetResourceDir
#include "player/player.h"
#include "boss/boss.h"
#include "game/questions.h"
#include "game/partida.h"
#include "game/history.h"
#include "analysis/analise.h"
#include "logic_phase.h"
#include "game_state.h"
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define HIST_LIST_X 40
#define HIST_LIST_Y 108
#define HIST_LIST_W 330
#define HIST_LIST_H 420
#define HIST_ROW_H 42
#define HIST_VISIVEIS 10
#define HIST_DETAIL_X 390
#define HIST_DETAIL_Y HIST_LIST_Y
#define HIST_DETAIL_W 370
#define HIST_DETAIL_H 420

typedef struct {
    char dataHora[32];
    char motivo[48];
    float tempo;
    int tiros;
    int acertosBoss;
    float precisao;
    int desafiosVencidos;
    int desafiosTotal;
    float taxaDesafios;
    char linhaOriginal[TAMANHO_LINHA_HISTORICO];
    bool valido;
} HistoricoRegistro;

static int menuSelection = 0; // 0 para Clássico, 1 para Lógica

// Use shared GameState from game_state.h

/* Variáveis do módulo de análise — integradas à main para exposição à UI */
static HistoricoPartida *g_historico = NULL;
static size_t g_historico_count = 0;
static int g_historico_err = 0; /* código HistoricoErro */
static double g_media_score = 0.0;
static double g_desvio_score = 0.0;
static HistoricoPartida g_melhor_partida = {0};
static HistoricoPartida g_pior_partida = {0};
static int g_has_analise = 0; /* 1 se houver dados válidos */

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

static void trimTexto(char* texto) {
    int inicio = 0;
    int fim = (int)strlen(texto) - 1;

    while (texto[inicio] != '\0' && isspace((unsigned char)texto[inicio])) inicio++;
    while (fim >= inicio && isspace((unsigned char)texto[fim])) {
        texto[fim] = '\0';
        fim--;
    }

    if (inicio > 0) {
        memmove(texto, texto + inicio, strlen(texto + inicio) + 1);
    }
}

static HistoricoRegistro parseLinhaHistorico(const char* linha) {
    HistoricoRegistro registro = {0};
    strncpy(registro.linhaOriginal, linha, TAMANHO_LINHA_HISTORICO - 1);
    registro.linhaOriginal[TAMANHO_LINHA_HISTORICO - 1] = '\0';

    int lidos = sscanf(
        linha,
        "[%31[^]]] %47[^|]| tempo: %fs | tiros: %d | acertos boss: %d | precisao: %f%% | desafios: %d/%d (%f%%)",
        registro.dataHora,
        registro.motivo,
        &registro.tempo,
        &registro.tiros,
        &registro.acertosBoss,
        &registro.precisao,
        &registro.desafiosVencidos,
        &registro.desafiosTotal,
        &registro.taxaDesafios
    );

    if (lidos == 9) {
        trimTexto(registro.dataHora);
        trimTexto(registro.motivo);
        registro.valido = true;
    } else {
        registro.valido = false;
        strcpy(registro.dataHora, "Data nao identificada");
        strcpy(registro.motivo, "Formato de relatorio legado");
    }

    return registro;
}

static void textoComReticencias(const char* origem, char* destino, int tamDestino, int fonte, int larguraMax) {
    if (tamDestino <= 0) return;
    destino[0] = '\0';
    if (origem == NULL) return;

    if (MeasureText(origem, fonte) <= larguraMax) {
        strncpy(destino, origem, tamDestino - 1);
        destino[tamDestino - 1] = '\0';
        return;
    }

    const char* sufixo = "...";
    int len = (int)strlen(origem);
    int i = len;
    while (i > 0) {
        char tmp[220];
        snprintf(tmp, sizeof(tmp), "%.*s%s", i, origem, sufixo);
        if (MeasureText(tmp, fonte) <= larguraMax) {
            strncpy(destino, tmp, tamDestino - 1);
            destino[tamDestino - 1] = '\0';
            return;
        }
        i--;
    }

    strncpy(destino, sufixo, tamDestino - 1);
    destino[tamDestino - 1] = '\0';
}

static int quebrarTexto(const char* texto, char linhas[][120], int maxLinhas, int fonte, int larguraMax) {
    if (texto == NULL || maxLinhas <= 0) return 0;
    int total = 0;
    const char* cursor = texto;

    while (*cursor != '\0' && total < maxLinhas) {
        int len = (int)strlen(cursor);
        int corte = len;
        if (MeasureText(cursor, fonte) > larguraMax) {
            for (corte = len; corte > 0; corte--) {
                char tmp[220];
                snprintf(tmp, sizeof(tmp), "%.*s", corte, cursor);
                if (MeasureText(tmp, fonte) <= larguraMax) break;
            }
            if (corte <= 0) corte = 1;
            while (corte > 1 && cursor[corte] != '\0' && cursor[corte] != ' ') corte--;
            if (corte <= 0) corte = 1;
        }

        snprintf(linhas[total], 120, "%.*s", corte, cursor);
        trimTexto(linhas[total]);
        total++;
        cursor += corte;
        while (*cursor == ' ') cursor++;
    }

    return total;
}

static int quebrarTextoComReticencias(const char* texto, char linhas[][120], int maxLinhas, int fonte, int larguraMax) {
    int qtd = quebrarTexto(texto, linhas, maxLinhas, fonte, larguraMax);
    if (texto == NULL || qtd <= 0) return qtd;

    // Se o texto original não coube inteiro, marca a última linha com reticências.
    char reconstruido[520] = {0};
    for (int i = 0; i < qtd; i++) {
        strncat(reconstruido, linhas[i], sizeof(reconstruido) - strlen(reconstruido) - 1);
        if (i < qtd - 1) strncat(reconstruido, " ", sizeof(reconstruido) - strlen(reconstruido) - 1);
    }

    if (strlen(reconstruido) < strlen(texto) && qtd > 0) {
        char ultima[120];
        snprintf(ultima, sizeof(ultima), "%s...", linhas[qtd - 1]);
        textoComReticencias(ultima, linhas[qtd - 1], 120, fonte, larguraMax);
    }

    return qtd;
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
    // Fundo escuro elegante para a tela principal
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){15, 18, 32, 255});
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){18, 22, 40, 255}, (Color){8, 12, 24, 255});

    for (int i = 0; i < 18; i++) {
        float x = fmodf(tempo * (40.0f + i * 5.0f) + i * 120.0f, GetScreenWidth() + 60.0f) - 30.0f;
        float y = 80.0f + sinf(tempo * 1.1f + i * 0.9f) * 18.0f + (i % 2) * 16.0f;
        DrawCircle((int)x, (int)y, 1.5f + (i % 3), (Color){180, 220, 255, 110});
    }

    const char* titulo = "MIND DROP";
    int tituloLargura = MeasureText(titulo, 80);
    int tituloX = (GetScreenWidth() - tituloLargura) / 2;
    DrawText(titulo, tituloX + 4, 88 + 4, 80, (Color){10, 12, 18, 180});
    DrawText(titulo, tituloX, 88, 80, (Color){180, 220, 255, 255});

    const char* subtitulo = "LOGIC ARCADE MODE";
    DrawText(subtitulo, (GetScreenWidth() - MeasureText(subtitulo, 24)) / 2, 178, 24, (Color){200, 220, 255, 200});

    Rectangle botaoClassico = {(GetScreenWidth() - 360) / 2.0f, 300, 360, 60};
    Rectangle botaoLogica = {(GetScreenWidth() - 360) / 2.0f, 384, 360, 60};

    Vector2 mouse = GetMousePosition();
    bool hoverClassico = CheckCollisionPointRec(mouse, botaoClassico);
    bool hoverLogica = CheckCollisionPointRec(mouse, botaoLogica);

    bool selecionadoClassico = (opcaoSelecionada == 0) || hoverClassico;
    bool selecionadoLogica = (opcaoSelecionada == 1) || hoverLogica;

    Color fundoClassico = selecionadoClassico ? (Color){245, 200, 60, 255} : (Color){35, 35, 54, 255};
    Color fundoLogica = selecionadoLogica ? (Color){245, 200, 60, 255} : (Color){35, 35, 54, 255};
    Color sombraClassico = selecionadoClassico ? (Color){185, 135, 20, 255} : (Color){18, 18, 28, 255};
    Color sombraLogica = selecionadoLogica ? (Color){185, 135, 20, 255} : (Color){18, 18, 28, 255};
    Color textoClassico = selecionadoClassico ? BLACK : RAYWHITE;
    Color textoLogica = selecionadoLogica ? BLACK : RAYWHITE;

    DrawRectangle((int)botaoClassico.x + 5, (int)botaoClassico.y + 5, (int)botaoClassico.width, (int)botaoClassico.height, sombraClassico);
    DrawRectangleRec(botaoClassico, fundoClassico);
    DrawRectangleLinesEx(botaoClassico, 3.0f, selecionadoClassico ? WHITE : (Color){120, 120, 160, 255});
    if (selecionadoClassico) {
        DrawText(">", (int)(botaoClassico.x - 28), (int)(botaoClassico.y + 16), 36, YELLOW);
    }
    DrawText("Modo Classico", (int)(botaoClassico.x + (botaoClassico.width - MeasureText("Modo Classico", 30)) / 2), (int)(botaoClassico.y + 14), 30, textoClassico);

    DrawRectangle((int)botaoLogica.x + 5, (int)botaoLogica.y + 5, (int)botaoLogica.width, (int)botaoLogica.height, sombraLogica);
    DrawRectangleRec(botaoLogica, fundoLogica);
    DrawRectangleLinesEx(botaoLogica, 3.0f, selecionadoLogica ? WHITE : (Color){120, 120, 160, 255});
    if (selecionadoLogica) {
        DrawText(">", (int)(botaoLogica.x - 28), (int)(botaoLogica.y + 16), 36, YELLOW);
    }
    DrawText("Fase Logica", (int)(botaoLogica.x + (botaoLogica.width - MeasureText("Fase Logica", 30)) / 2), (int)(botaoLogica.y + 14), 30, textoLogica);

    const char* dica = "Use W/S ou ↑/↓ e Enter para selecionar.";
    DrawText(dica, (GetScreenWidth() - MeasureText(dica, 18)) / 2, GetScreenHeight() - 50, 18, (Color){190, 200, 220, 220});
}

static void drawTelaHistorico(const HistoricoRegistro registros[], int totalLinhas, int scroll, int selecionado, int expandido, float tempo) {
    drawFundoArcadeAnimado(tempo);
    const char* titulo = "HISTORICO DE RELATORIOS";
    DrawText(titulo, (GetScreenWidth() - MeasureText(titulo, 38)) / 2, 40, 38, (Color){255, 220, 75, 255});

    Rectangle painelLista = {HIST_LIST_X, HIST_LIST_Y, HIST_LIST_W, HIST_LIST_H};
    Rectangle painelDetalhes = {HIST_DETAIL_X, HIST_DETAIL_Y, HIST_DETAIL_W, HIST_DETAIL_H};
    DrawRectangleRec(painelLista, (Color){12, 12, 20, 220});
    DrawRectangleLinesEx(painelLista, 3.0f, (Color){170, 170, 215, 255});
    DrawRectangleRec(painelDetalhes, (Color){14, 14, 26, 230});
    DrawRectangleLinesEx(painelDetalhes, 3.0f, (Color){170, 170, 215, 255});
    DrawText("RODADAS", (int)painelLista.x + 12, (int)painelLista.y + 10, 22, (Color){255, 220, 75, 255});
    DrawText("DETALHES", (int)painelDetalhes.x + 12, (int)painelDetalhes.y + 10, 22, (Color){255, 220, 75, 255});

    if (totalLinhas == 0) {
        const char* vazio = "Sem relatorios salvos ainda. Jogue uma partida!";
        DrawText(vazio, (GetScreenWidth() - MeasureText(vazio, 24)) / 2, 290, 24, RAYWHITE);
    } else {
        int inicio = scroll;
        int fim = inicio + HIST_VISIVEIS;
        if (fim > totalLinhas) fim = totalLinhas;
        int y = HIST_LIST_Y + 44;
        BeginScissorMode((int)painelLista.x + 1, (int)painelLista.y + 40, (int)painelLista.width - 2, (int)painelLista.height - 46);
        for (int i = inicio; i < fim; i++) {
            Rectangle item = {HIST_LIST_X + 12, (float)y, HIST_LIST_W - 24, HIST_ROW_H - 6};
            bool ativo = (i == selecionado);
            bool aberto = (i == expandido);
            Color fundo = ativo ? (Color){60, 48, 20, 255} : (Color){20, 20, 34, 220};
            DrawRectangleRec(item, fundo);
            DrawRectangleLinesEx(item, 1.5f, aberto ? YELLOW : (Color){110, 110, 145, 255});

            char dataCurta[48];
            char motivoCurto[64];
            textoComReticencias(registros[i].dataHora, dataCurta, sizeof(dataCurta), 15, (int)item.width - 130);
            textoComReticencias(registros[i].motivo, motivoCurto, sizeof(motivoCurto), 14, (int)item.width - 130);

            DrawText(TextFormat("#%d", i + 1), (int)item.x + 10, (int)item.y + 10, 16, (Color){255, 225, 110, 255});
            DrawText(dataCurta, (int)item.x + 54, (int)item.y + 8, 15, RAYWHITE);
            DrawText(motivoCurto, (int)item.x + 54, (int)item.y + 24, 14, (Color){208, 208, 230, 255});
            if (aberto) {
                DrawText("EXPANDIDO", (int)item.x + (int)item.width - 86, (int)item.y + 12, 12, (Color){255, 225, 110, 255});
            }
            y += HIST_ROW_H;
        }
        EndScissorMode();

        if (expandido >= 0 && expandido < totalLinhas) {
            const HistoricoRegistro* r = &registros[expandido];
            int dy = (int)painelDetalhes.y + 48;
            int detalheX = (int)painelDetalhes.x + 16;
            int detalheW = (int)painelDetalhes.width - 32;
            char linhasWrap[8][120];

            BeginScissorMode((int)painelDetalhes.x + 1, (int)painelDetalhes.y + 40, (int)painelDetalhes.width - 2, (int)painelDetalhes.height - 46);
            DrawText(TextFormat("Rodada #%d", expandido + 1), detalheX, dy, 24, WHITE);
            dy += 36;

            int qtd = quebrarTextoComReticencias(TextFormat("Data/Hora: %s", r->dataHora), linhasWrap, 2, 17, detalheW);
            for (int i = 0; i < qtd; i++) {
                DrawText(linhasWrap[i], detalheX, dy, 17, RAYWHITE);
                dy += 22;
            }

            qtd = quebrarTextoComReticencias(TextFormat("Resultado: %s", r->motivo), linhasWrap, 2, 17, detalheW);
            for (int i = 0; i < qtd; i++) {
                DrawText(linhasWrap[i], detalheX, dy, 17, RAYWHITE);
                dy += 22;
            }
            dy += 6;

            if (r->valido) {
                DrawText(TextFormat("Tempo de partida: %.1f s", r->tempo), detalheX, dy, 17, RAYWHITE);
                dy += 24;
                qtd = quebrarTextoComReticencias(
                    TextFormat("Acertos no boss: %d/%d (%.1f%%)", r->acertosBoss, r->tiros, r->precisao),
                    linhasWrap, 2, 17, detalheW
                );
                for (int i = 0; i < qtd; i++) {
                    DrawText(linhasWrap[i], detalheX, dy, 17, RAYWHITE);
                    dy += 22;
                }
                qtd = quebrarTextoComReticencias(
                    TextFormat("Desafios vencidos: %d/%d (%.1f%%)", r->desafiosVencidos, r->desafiosTotal, r->taxaDesafios),
                    linhasWrap, 2, 17, detalheW
                );
                for (int i = 0; i < qtd; i++) {
                    DrawText(linhasWrap[i], detalheX, dy, 17, RAYWHITE);
                    dy += 22;
                }
            } else {
                DrawText("Relatorio em formato legado:", detalheX, dy, 16, RAYWHITE);
                dy += 22;
                char linhas[4][120];
                int qtdLinhas = quebrarTextoComReticencias(r->linhaOriginal, linhas, 6, 15, detalheW);
                for (int l = 0; l < qtdLinhas; l++) {
                    DrawText(linhas[l], detalheX, dy, 15, (Color){210, 210, 230, 255});
                    dy += 18;
                }
            }
            EndScissorMode();
        } else {
            const char* dica = "Clique em uma rodada para expandir.";
            DrawText(dica, (int)painelDetalhes.x + 16, (int)painelDetalhes.y + 52, 19, (Color){220, 220, 235, 240});
        }
    }

    const char* rodape = "ESC/B voltar | W/S navega | ENTER ou clique expande";
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
    const char* sair = "ENTER, ESC ou B para voltar ao menu";
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
    InitLogicPhase();
    SetExitKey(KEY_NULL);
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
    int historicoSelecionado = -1;
    int historicoExpandido = -1;
    HistoricoRegistro registrosHistorico[MAX_HISTORICO_LINHAS];
    bool relatorioSalvo = false;
    GameState currentState = STATE_MENU;

    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);

    /* Carregar histórico de partidas ao iniciar (arquivo opcional). */
    {
        const char *caminhoHistorico = "resources/historico.txt";
        HistoricoErro err = carregarHistorico(caminhoHistorico, &g_historico, &g_historico_count);
        g_historico_err = (int)err;
        if (err == HIST_OK) {
            if (validarDadosHistorico(g_historico, g_historico_count) == HIST_OK) {
                g_media_score = calcularMediaScore(g_historico, g_historico_count);
                g_desvio_score = calcularDesvioPadraoScore(g_historico, g_historico_count);
                int found = 0;
                g_melhor_partida = melhorPartida(g_historico, g_historico_count, &found);
                g_pior_partida = piorPartida(g_historico, g_historico_count, &found);
                g_has_analise = 1;
            } else {
                liberarHistorico(g_historico);
                g_historico = NULL;
                g_historico_count = 0;
                g_has_analise = 0;
            }
        } else {
            /* Não encontrou ou arquivo inválido — segue sem análise */
            g_has_analise = 0;
        }
    }

    // Loop Principal
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float tempo = (float)GetTime();

if (IsKeyPressed(KEY_M)) currentState = STATE_LOGIC_PHASE;

        if (currentState == STATE_MENU) {
            Vector2 mouse = GetMousePosition();
            Rectangle botaoClassico = {(GetScreenWidth() - 360) / 2.0f, 300, 360, 60};
            Rectangle botaoLogica = {(GetScreenWidth() - 360) / 2.0f, 384, 360, 60};
            bool hoverClassico = CheckCollisionPointRec(mouse, botaoClassico);
            bool hoverLogica = CheckCollisionPointRec(mouse, botaoLogica);

            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                menuSelection = 0;
            } else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                menuSelection = 1;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (hoverClassico) {
                    menuSelection = 0;
                    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);
                    relatorioSalvo = false;
                    telaAtual = TELA_JOGO;
                    currentState = STATE_LOGIC_PHASE;
                } else if (hoverLogica) {
                    menuSelection = 1;
                    currentState = STATE_TUTORIAL_LOGICA;
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (menuSelection == 0) {
                    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);
                    relatorioSalvo = false;
                    telaAtual = TELA_JOGO;
                    currentState = STATE_LOGIC_PHASE;
                } else if (menuSelection == 1) {
                    currentState = STATE_TUTORIAL_LOGICA;
                }
            }
        }

        if (currentState == STATE_LOGIC_PHASE) {
        if (telaAtual == TELA_MENU) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                opcaoMenu = (opcaoMenu + 2) % 3;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                opcaoMenu = (opcaoMenu + 1) % 3;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (opcaoMenu == 0) {
                    inicializarPartida(&jogador, bala, &boss, balasBoss, &estrela, &desafio, &perguntaAtiva, &jogoEncerrado, &motivoFimJogo, &stats);
                    relatorioSalvo = false;
                    telaAtual = TELA_JOGO;
                } else if (opcaoMenu == 1) {
                    totalHistorico = carregarHistoricoRelatorios(linhasHistorico, MAX_HISTORICO_LINHAS);
                    scrollHistorico = 0;
                    historicoSelecionado = totalHistorico > 0 ? 0 : -1;
                    historicoExpandido = -1;
                    for (int i = 0; i < totalHistorico; i++) {
                        registrosHistorico[i] = parseLinhaHistorico(linhasHistorico[i]);
                    }
                    telaAtual = TELA_HISTORICO;
                } else if (opcaoMenu == 2) {
                    telaAtual = TELA_RELATORIO_ANALITICO;
                }
            }
        } else if (telaAtual == TELA_HISTORICO) {
            if (totalHistorico > 0) {
                if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && historicoSelecionado < totalHistorico - 1) {
                    historicoSelecionado++;
                }
                if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && historicoSelecionado > 0) {
                    historicoSelecionado--;
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    historicoExpandido = (historicoExpandido == historicoSelecionado) ? -1 : historicoSelecionado;
                }

                if (historicoSelecionado < scrollHistorico) scrollHistorico = historicoSelecionado;
                if (historicoSelecionado >= scrollHistorico + HIST_VISIVEIS) {
                    scrollHistorico = historicoSelecionado - HIST_VISIVEIS + 1;
                }
                if (scrollHistorico < 0) scrollHistorico = 0;

                float rodaMouse = GetMouseWheelMove();
                if (rodaMouse != 0.0f) {
                    scrollHistorico -= (int)rodaMouse;
                }
                int maxScroll = totalHistorico - HIST_VISIVEIS;
                if (maxScroll < 0) maxScroll = 0;
                if (scrollHistorico > maxScroll) scrollHistorico = maxScroll;

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mouse = GetMousePosition();
                    Rectangle areaLista = {HIST_LIST_X + 12, HIST_LIST_Y + 44, HIST_LIST_W - 24, HIST_VISIVEIS * HIST_ROW_H};
                    if (CheckCollisionPointRec(mouse, areaLista)) {
                        int idxLocal = (int)((mouse.y - areaLista.y) / HIST_ROW_H);
                        int idxReal = scrollHistorico + idxLocal;
                        if (idxReal >= 0 && idxReal < totalHistorico) {
                            historicoSelecionado = idxReal;
                            historicoExpandido = (historicoExpandido == idxReal) ? -1 : idxReal;
                        }
                    }
                }
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_B)) telaAtual = TELA_MENU;
        } else if (telaAtual == TELA_RELATORIO_ANALITICO) {
            Rectangle botaoVoltarAnalise = {40, 536, 150, 36};
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_B)) {
                telaAtual = TELA_MENU;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botaoVoltarAnalise)) {
                telaAtual = TELA_MENU;
            }
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
                moverBoss(&boss, &jogador, deltaTime);
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
                atualizarBoostDanoPlayer(&jogador, deltaTime);
                atualizarBoostVelocidadePlayer(&jogador, deltaTime);

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
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_B)) {
                telaAtual = TELA_MENU;
            }
        }
        }
        if (currentState == STATE_TUTORIAL_LOGICA) {
            UpdateLogicTutorial(&currentState);
        } else if (currentState == STATE_LOGIC_PHASE) {
            UpdateLogicPhase(&currentState);
            if (LogicPhaseHasEnded()) {
                currentState = STATE_MENU;
            }
        } else if (currentState == STATE_LOGIC_REPORT) {
            UpdateLogicReport(&currentState);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (currentState == STATE_LOGIC_PHASE) {
        if (telaAtual == TELA_MENU) {
            drawMenuInicial(opcaoMenu, tempo);
        } else if (telaAtual == TELA_HISTORICO) {
            drawTelaHistorico(registrosHistorico, totalHistorico, scrollHistorico, historicoSelecionado, historicoExpandido, tempo);
        } else if (telaAtual == TELA_RELATORIO_ANALITICO) {
            drawFundoArcadeAnimado(tempo);
            renderizarRelatorioAnalitico(NULL);
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
        } else if (currentState == STATE_MENU) {
            drawMenuInicial(menuSelection, tempo);
        } else if (currentState == STATE_TUTORIAL_LOGICA) {
            DrawLogicTutorial();
        } else if (currentState == STATE_LOGIC_PHASE) {
            DrawLogicPhase();
        } else if (currentState == STATE_LOGIC_REPORT) {
            DrawLogicReport();
        }

            EndDrawing();
        }

        /* Liberar recursos de análise se necessário */
        if (g_historico) {
            liberarHistorico(g_historico);
            g_historico = NULL;
            g_historico_count = 0;
        }

        CloseWindow();
        return 0;
    }
