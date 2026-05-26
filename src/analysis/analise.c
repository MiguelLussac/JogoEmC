/* analise.c
 * Implementação das funções para carregamento e análise estatística
 * do histórico de partidas.
 */

#include "analise.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

/* Função auxiliar para trim de strings (remove espaços em branco no início e fim) */
static char *trim(char *s) {
    if (!s) return s;
    /* trim left */
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    /* trim right */
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

/* Função auxiliar para parse de linha do arquivo */
static int parse_line(const char *line, HistoricoPartida *out) {
    /* aceita separadores espaço, tab ou vírgula */
    char buf[1024];
    strncpy(buf, line, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    char *tok;
    char *saveptr;
    char *ptr = buf;
    int fields = 0;
    long v_id = 0;
    long v_score = 0;
    long v_acertos = 0;
    long v_tent = 0;
    double v_tempo = 0.0;

    const char *delim = ",\t \f\v"; /* comma and whitespace */
    tok = strtok_r(ptr, delim, &saveptr);
    while (tok) {
        char *t = trim(tok);
        if (*t == '\0') { tok = strtok_r(NULL, delim, &saveptr); continue; }
        errno = 0;
        if (fields == 0) v_id = strtol(t, NULL, 10);
        else if (fields == 1) v_score = strtol(t, NULL, 10);
        else if (fields == 2) v_acertos = strtol(t, NULL, 10);
        else if (fields == 3) v_tent = strtol(t, NULL, 10);
        else if (fields == 4) v_tempo = strtod(t, NULL);
        fields++;
        tok = strtok_r(NULL, delim, &saveptr);
    }

    if (fields < 5) return 0; /* inválido */

    out->id = (int)v_id;
    out->score = (int)v_score;
    out->acertos = (int)v_acertos;
    out->tentativas = (int)v_tent;
    out->tempo = v_tempo;
    return 1;
}

/* Implementação das funções da API */
HistoricoErro carregarHistorico(const char *caminho, HistoricoPartida **outArray, size_t *outCount) {
    if (!caminho || !outArray || !outCount) return HIST_ERR_INVALID;
    *outArray = NULL;
    *outCount = 0;

    FILE *f = fopen(caminho, "r");
    if (!f) return HIST_ERR_NOFILE;

    /* verificar se vazio */
    int ch = fgetc(f);
    if (ch == EOF) { fclose(f); return HIST_ERR_EMPTY; }
    rewind(f);

    size_t cap = 16;
    size_t count = 0;
    HistoricoPartida *arr = (HistoricoPartida*)malloc(sizeof(HistoricoPartida) * cap);
    if (!arr) { fclose(f); return HIST_ERR_NOMEM; }

    char line[1024];
    size_t lineno = 0;
    while (fgets(line, sizeof(line), f)) {
        lineno++;
        char *s = trim(line);
        if (!s || *s == '\0') continue; /* ignora linhas em branco */

        HistoricoPartida item;
        if (!parse_line(s, &item)) {
            free(arr);
            fclose(f);
            return HIST_ERR_INVALID;
        }

        if (count >= cap) {
            size_t ncap = cap * 2;
            HistoricoPartida *tmp = (HistoricoPartida*)realloc(arr, sizeof(HistoricoPartida) * ncap);
            if (!tmp) { free(arr); fclose(f); return HIST_ERR_NOMEM; }
            arr = tmp; cap = ncap;
        }
        arr[count++] = item;
    }

    fclose(f);
    if (count == 0) { free(arr); return HIST_ERR_EMPTY; }

    *outArray = arr;
    *outCount = count;
    return HIST_OK;
}

/* Validação de dados: verifica consistência dos campos */
HistoricoErro validarDadosHistorico(const HistoricoPartida *arr, size_t count) {
    if (!arr) return HIST_ERR_INVALID;
    for (size_t i = 0; i < count; ++i) {
        const HistoricoPartida *h = &arr[i];
        if (h->tentativas <= 0) return HIST_ERR_INVALID;
        if (h->acertos < 0 || h->acertos > h->tentativas) return HIST_ERR_INVALID;
        if (h->score < 0) return HIST_ERR_INVALID;
        if (h->tempo < 0.0) return HIST_ERR_INVALID;
    }
    return HIST_OK;
}

/* Retorna o número de partidas (simples helper). */
size_t quantidadePartidas(const HistoricoPartida *arr, size_t count) {
    (void)arr;
    return count;
}

/* Estatísticas básicas (operam sobre o array carregado). */
double calcularMediaScore(const HistoricoPartida *arr, size_t count) {
    if (!arr || count == 0) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) sum += (double)arr[i].score;
    return sum / (double)count;
}

/* Cálculo do desvio padrão amostral */
double calcularDesvioPadraoScore(const HistoricoPartida *arr, size_t count) {
    if (!arr || count < 2) return 0.0;
    double mean = calcularMediaScore(arr, count);
    double s = 0.0;
    for (size_t i = 0; i < count; ++i) {
        double d = (double)arr[i].score - mean;
        s += d * d;
    }
    return sqrt(s / (double)(count - 1)); /* amostral */
}

/* Melhor e pior partida (baseado na pontuação) */
HistoricoPartida melhorPartida(const HistoricoPartida *arr, size_t count, int *found) {
    HistoricoPartida empty = {0,0,0,0,0.0};
    if (!arr || count == 0) { if (found) *found = 0; return empty; }
    size_t idx = 0;
    for (size_t i = 1; i < count; ++i) {
        if (arr[i].score > arr[idx].score) idx = i;
    }
    if (found) *found = 1;
    return arr[idx];
}

/* Pior partida (baseado na pontuação) */
HistoricoPartida piorPartida(const HistoricoPartida *arr, size_t count, int *found) {
    HistoricoPartida empty = {0,0,0,0,0.0};
    if (!arr || count == 0) { if (found) *found = 0; return empty; }
    size_t idx = 0;
    for (size_t i = 1; i < count; ++i) {
        if (arr[i].score < arr[idx].score) idx = i;
    }
    if (found) *found = 1;
    return arr[idx];
}

/* ===== IMPLEMENTAÇÃO DAS FUNÇÕES RECURSIVAS OBRIGATÓRIAS ===== */

/* Soma recursiva dos scores */
/* Caso base: idx >= count, retorna 0 */
/* Caso recursivo: arr[idx].score + somaRecursiva(arr, idx+1, count) */
int somaRecursiva(const HistoricoPartida *arr, size_t idx, size_t count) {
    if (!arr || idx >= count) return 0;
    return arr[idx].score + somaRecursiva(arr, idx + 1, count);
}

/* Valor mínimo recursivo nos scores */
/* Caso base: idx >= count-1, retorna arr[0].score */
/* Caso recursivo: compara arr[idx].score com o mínimo dos restantes */
int minimoRecursivo(const HistoricoPartida *arr, size_t idx, size_t count) {
    if (!arr || count == 0) return 0;
    if (idx >= count - 1) return arr[0].score;
    int proxMin = minimoRecursivo(arr, idx + 1, count);
    return arr[idx].score < proxMin ? arr[idx].score : proxMin;
}

/* Valor máximo recursivo nos scores */
/* Caso base: idx >= count-1, retorna arr[0].score */
/* Caso recursivo: compara arr[idx].score com o máximo dos restantes */
int maximoRecursivo(const HistoricoPartida *arr, size_t idx, size_t count) {
    if (!arr || count == 0) return 0;
    if (idx >= count - 1) return arr[0].score;
    int proxMax = maximoRecursivo(arr, idx + 1, count);
    return arr[idx].score > proxMax ? arr[idx].score : proxMax;
}

/* Soma dos quadrados dos scores recursivamente */
/* Caso base: idx >= count, retorna 0 */
/* Caso recursivo: (arr[idx].score)^2 + somaQuadradosRecursiva(arr, idx+1, count) */
double somaQuadradosRecursiva(const HistoricoPartida *arr, size_t idx, size_t count) {
    if (!arr || idx >= count) return 0.0;
    double score = (double)arr[idx].score;
    return score * score + somaQuadradosRecursiva(arr, idx + 1, count);
}

/* ===== GERAÇÃO DE HEURÍSTICAS ESTRATÉGICAS ===== */

static double calcularTaxaAcertosMedia(const HistoricoPartida *arr, size_t count) {
    if (!arr || count == 0) return 0.0;
    double somaAcertos = 0.0;
    for (size_t i = 0; i < count; ++i) {
        if (arr[i].tentativas > 0) {
            somaAcertos += (double)arr[i].acertos / (double)arr[i].tentativas;
        }
    }
    return somaAcertos / (double)count;
}

void gerarHeuristicas(const HistoricoPartida *arr, size_t count,
                      const RelatorioAnalitico *relatorio,
                      char heuristicas[][256], int *quantidadeHeuristicas) {
    if (!arr || !relatorio || !heuristicas || !quantidadeHeuristicas) return;
    
    *quantidadeHeuristicas = 0;
    
    /* Heurística 1: Análise da taxa de sucesso */
    double taxaAcertos = calcularTaxaAcertosMedia(arr, count);
    if (taxaAcertos > 0.8) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Excelente taxa de acertos! Mantenha a consistência.");
        (*quantidadeHeuristicas)++;
    } else if (taxaAcertos > 0.6) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Taxa de acertos acima da média. Concentre em melhorar timing.");
        (*quantidadeHeuristicas)++;
    } else if (taxaAcertos > 0.4) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Taxa de acertos moderada. Pratique padrões de movimentação.");
        (*quantidadeHeuristicas)++;
    } else {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Taxa de acertos baixa. Revise estratégia de posicionamento.");
        (*quantidadeHeuristicas)++;
    }
    
    /* Heurística 2: Análise de variabilidade */
    if (relatorio->desvioPadrao > relatorio->mediaScore * 0.5) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Alto desvio de desempenho. Procure regularizar execução.");
        (*quantidadeHeuristicas)++;
    } else if (relatorio->desvioPadrao < relatorio->mediaScore * 0.2) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Excelente regularidade! Desempenho previsível e constante.");
        (*quantidadeHeuristicas)++;
    }
    
    /* Heurística 3: Análise de progressão */
    if (count >= 3) {
        int ultimaTri = arr[count-1].score;
        int penultima = arr[count > 1 ? count-2 : 0].score;
        if (ultimaTri > penultima && penultima > arr[0].score) {
            snprintf(heuristicas[*quantidadeHeuristicas], 256,
                     "Tendência crescente detectada. Progresso em dia!");
            (*quantidadeHeuristicas)++;
        } else if (ultimaTri < penultima && penultima < arr[0].score) {
            snprintf(heuristicas[*quantidadeHeuristicas], 256,
                     "Desempenho em declínio. Revise estratégia geral.");
            (*quantidadeHeuristicas)++;
        }
    }
    
    /* Heurística 4: Recomendação de foco estratégico */
    if (relatorio->melhorScore - relatorio->piorScore > relatorio->mediaScore) {
        snprintf(heuristicas[*quantidadeHeuristicas], 256,
                 "Reproduza condições das melhores partidas para consistência.");
        (*quantidadeHeuristicas)++;
    }
}

/* ===== GERAÇÃO DO RELATÓRIO ANALÍTICO COMPLETO ===== */

RelatorioAnalitico gerarRelatorioAnalitico(const HistoricoPartida *arr, size_t count) {
    RelatorioAnalitico rel = {0};
    
    if (!arr || count == 0) return rel;
    
    /* Validação básica */
    if (validarDadosHistorico(arr, count) != HIST_OK) return rel;
    
    /* Preencher quantidade de partidas */
    rel.quantidadePartidas = count;
    
    /* Calcular média usando o método iterativo existente */
    rel.mediaScore = calcularMediaScore(arr, count);
    
    /* Calcular desvio padrão */
    rel.desvioPadrao = calcularDesvioPadraoScore(arr, count);
    
    /* Encontrar melhor e pior usando as funções recursivas */
    rel.melhorScore = maximoRecursivo(arr, 0, count);
    rel.piorScore = minimoRecursivo(arr, 0, count);
    
    /* Encontrar melhor e pior tempo */
    rel.melhorTempo = arr[0].tempo;
    rel.piorTempo = arr[0].tempo;
    for (size_t i = 1; i < count; ++i) {
        if (arr[i].tempo < rel.melhorTempo) rel.melhorTempo = arr[i].tempo;
        if (arr[i].tempo > rel.piorTempo) rel.piorTempo = arr[i].tempo;
    }
    
    /* Calcular média de acertos */
    double somaAcertos = 0.0;
    for (size_t i = 0; i < count; ++i) {
        somaAcertos += (double)arr[i].acertos;
    }
    rel.mediaAcertos = somaAcertos / (double)count;
    
    /* Calcular taxa de sucesso média */
    rel.taxaSucessoMedia = calcularTaxaAcertosMedia(arr, count);
    
    /* Gerar heurísticas */
    gerarHeuristicas(arr, count, &rel, rel.heuristicas, &rel.quantidadeHeuristicas);
    
    return rel;
}

/* ===== RENDERIZAÇÃO GRÁFICA DO RELATÓRIO COM RAYLIB ===== */

void liberarHistorico(HistoricoPartida *arr) {
    if (!arr) return;
    free(arr);
}

#include "raylib.h"

static void renderizarRelatorioAnaliticoComDados(const RelatorioAnalitico *relatorio) {
    if (!relatorio || relatorio->quantidadePartidas == 0) {
        /* Exibir mensagem sem dados */
        DrawText("Nenhum dado de partidas para exibir", 350, 300, 20, RED);
        return;
    }
    
    /* Cores para UI */
    Color corFundo = (Color){30, 30, 40, 255};
    Color corTitulo = (Color){100, 200, 255, 255};
    Color corTexto = (Color){240, 240, 240, 255};
    Color corDestaque = (Color){150, 255, 150, 255};
    Color corAviso = (Color){255, 150, 100, 255};
    Color corBorda = (Color){80, 150, 200, 255};
    
    /* Dimensões da tela e posicionamento */
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int margemLateral = 50;
    int margemTopo = 80;
    int larguraConteudo = screenWidth - 2 * margemLateral;
    
    /* ===== TÍTULO DO RELATÓRIO ===== */
    DrawText("RELATORIO ANALITICO", margemLateral, margemTopo - 50, 32, corTitulo);
    DrawText("Analise de Desempenho em Partidas", margemLateral, margemTopo - 15, 14, corTexto);
    
    /* Linha divisória */
    DrawLineEx(
        (Vector2){margemLateral, margemTopo},
        (Vector2){screenWidth - margemLateral, margemTopo},
        2, corBorda
    );
    
    /* ===== SEÇÃO 1: ESTATÍSTICAS GERAIS ===== */
    int posY = margemTopo + 30;
    DrawText("ESTATISTICAS GERAIS", margemLateral, posY, 20, corDestaque);
    posY += 35;
    
    /* Caixa de estatísticas gerais */
    DrawRectangle(margemLateral, posY - 5, larguraConteudo / 2 - 15, 100, 
                  (Color){50, 50, 60, 200});
    DrawRectangleLines(margemLateral, posY - 5, larguraConteudo / 2 - 15, 100, corBorda);
    
    char bufQtd[32];
    snprintf(bufQtd, sizeof(bufQtd), "Partidas: %zu", relatorio->quantidadePartidas);
    DrawText(bufQtd, margemLateral + 20, posY + 5, 16, corTexto);
    
    char bufMedia[48];
    snprintf(bufMedia, sizeof(bufMedia), "Score Medio: %.1f", relatorio->mediaScore);
    DrawText(bufMedia, margemLateral + 20, posY + 30, 16, corDestaque);
    
    char bufDesvio[64];
    snprintf(bufDesvio, sizeof(bufDesvio), "Desvio Padrao: %.2f", relatorio->desvioPadrao);
    DrawText(bufDesvio, margemLateral + 20, posY + 55, 16, corTexto);
    
    /* ===== SEÇÃO 2: DESEMPENHO EXTREMO ===== */
    int posY2 = posY;
    DrawRectangle(margemLateral + larguraConteudo / 2 + 5, posY2 - 5, 
                  larguraConteudo / 2 - 15, 100, (Color){50, 50, 60, 200});
    DrawRectangleLines(margemLateral + larguraConteudo / 2 + 5, posY2 - 5, 
                       larguraConteudo / 2 - 15, 100, corBorda);
    
    char bufMelhor[48];
    snprintf(bufMelhor, sizeof(bufMelhor), "Melhor Score: %d", relatorio->melhorScore);
    DrawText(bufMelhor, margemLateral + larguraConteudo / 2 + 25, posY2 + 5, 16, corDestaque);
    
    char bufPior[48];
    snprintf(bufPior, sizeof(bufPior), "Pior Score: %d", relatorio->piorScore);
    DrawText(bufPior, margemLateral + larguraConteudo / 2 + 25, posY2 + 30, 16, corAviso);
    
    char bufTaxaAcertos[64];
    snprintf(bufTaxaAcertos, sizeof(bufTaxaAcertos), "Taxa Acertos: %.1f%%", 
             relatorio->taxaSucessoMedia * 100.0);
    DrawText(bufTaxaAcertos, margemLateral + larguraConteudo / 2 + 25, posY2 + 55, 16, corTexto);
    
    posY += 120;
    
    /* ===== SEÇÃO 3: TEMPO DE PARTIDA ===== */
    DrawText("TEMPO DE PARTIDA", margemLateral, posY, 20, corDestaque);
    posY += 30;
    
    DrawRectangle(margemLateral, posY - 5, larguraConteudo, 60,
                  (Color){50, 50, 60, 200});
    DrawRectangleLines(margemLateral, posY - 5, larguraConteudo, 60, corBorda);
    
    char bufMelhorTempo[64];
    snprintf(bufMelhorTempo, sizeof(bufMelhorTempo), "Melhor Tempo: %.2f s", 
             relatorio->melhorTempo);
    DrawText(bufMelhorTempo, margemLateral + 20, posY + 5, 16, corDestaque);
    
    char bufPiorTempo[64];
    snprintf(bufPiorTempo, sizeof(bufPiorTempo), "Pior Tempo: %.2f s", 
             relatorio->piorTempo);
    DrawText(bufPiorTempo, margemLateral + 20, posY + 28, 16, corAviso);
    
    posY += 80;
    
    /* ===== SEÇÃO 4: HEURÍSTICAS ESTRATÉGICAS ===== */
    DrawText("HEURISTICAS ESTRATEGICAS", margemLateral, posY, 20, corDestaque);
    posY += 30;
    
    /* Desenhar cada heurística em uma caixa */
    for (int i = 0; i < relatorio->quantidadeHeuristicas && i < 4; ++i) {
        int alturaBox = 50;
        DrawRectangle(margemLateral, posY - 5, larguraConteudo, alturaBox,
                      (Color){60, 70, 80, 180});
        DrawRectangleLines(margemLateral, posY - 5, larguraConteudo, alturaBox, corBorda);
        
        /* Renderizar texto em múltiplas linhas se necessário */
        DrawText(relatorio->heuristicas[i], margemLateral + 15, posY + 8, 14, corTexto);
        
        posY += alturaBox + 10;
    }
    
    /* ===== SEÇÃO 5: MÉDIA DE ACERTOS ===== */
    posY += 10;
    DrawText("ANALISE DE ACERTOS", margemLateral, posY, 20, corDestaque);
    posY += 30;
    
    DrawRectangle(margemLateral, posY - 5, larguraConteudo, 50,
                  (Color){50, 50, 60, 200});
    DrawRectangleLines(margemLateral, posY - 5, larguraConteudo, 50, corBorda);
    
    char bufMediaAcertos[64];
    snprintf(bufMediaAcertos, sizeof(bufMediaAcertos), "Media de Acertos por Partida: %.2f", 
             relatorio->mediaAcertos);
    DrawText(bufMediaAcertos, margemLateral + 20, posY + 10, 16, corDestaque);
    
    /* ===== RODAPÉ COM AÇÃO ===== */
    int rodapeY = screenHeight - 60;
    DrawLineEx(
        (Vector2){margemLateral, rodapeY},
        (Vector2){screenWidth - margemLateral, rodapeY},
        2, corBorda
    );
    DrawText("Pressione ESC para voltar ao menu | R para recarregar dados", 
             margemLateral, rodapeY + 15, 14, corTexto);
}

void renderizarRelatorioAnalitico(const RelatorioAnalitico *relatorio) {
    (void)relatorio;

    Color corPainel = (Color){12, 12, 20, 220};
    Color corPainel2 = (Color){14, 14, 26, 230};
    Color corBorda = (Color){170, 170, 215, 255};
    Color corTitulo = (Color){255, 220, 75, 255};
    Color corTexto = (Color){220, 220, 235, 240};
    Color corDestaque = (Color){255, 225, 110, 255};

    const char* titulo = "RELATORIO ANALITICO";
    DrawText(titulo, (GetScreenWidth() - MeasureText(titulo, 38)) / 2, 40, 38, corTitulo);

    Rectangle painelStats = {40, 108, 330, 390};
    Rectangle painelHeuristicas = {390, 108, 370, 390};
    DrawRectangleRec(painelStats, corPainel);
    DrawRectangleLinesEx(painelStats, 3.0f, corBorda);
    DrawRectangleRec(painelHeuristicas, corPainel2);
    DrawRectangleLinesEx(painelHeuristicas, 3.0f, corBorda);

    DrawText("ESTATISTICAS", (int)painelStats.x + 12, (int)painelStats.y + 10, 22, corTitulo);
    DrawText("HEURISTICAS", (int)painelHeuristicas.x + 12, (int)painelHeuristicas.y + 10, 22, corTitulo);

    const char* labelsStats[] = {
        "Partidas analisadas",
        "Vitorias",
        "Derrotas",
        "Media de desempenho",
        "Melhor resultado",
        "Pior resultado",
        "Desvio estatistico"
    };
    const char* valoresStats[] = {
        "--",
        "--",
        "--",
        "--%",
        "--",
        "--",
        "--"
    };

    int y = (int)painelStats.y + 52;
    for (int i = 0; i < 7; i++) {
        Rectangle linha = {painelStats.x + 12, (float)y, painelStats.width - 24, 38};
        DrawRectangleRec(linha, (Color){20, 20, 34, 220});
        DrawRectangleLinesEx(linha, 1.5f, (Color){110, 110, 145, 255});
        DrawText(labelsStats[i], (int)linha.x + 10, (int)linha.y + 7, 16, corTexto);
        DrawText(valoresStats[i], (int)(linha.x + linha.width - MeasureText(valoresStats[i], 18) - 12), (int)linha.y + 6, 18, corDestaque);
        y += 46;
    }

    const char* heuristicas[] = {
        "Desempenho defensivo: aguardando dados reais.",
        "Eficiencia com buffs: espaco preparado para analise.",
        "Padroes de derrota: sugestoes futuras aparecerao aqui.",
        "Recomendacao: ajuste estrategia conforme o historico."
    };

    y = (int)painelHeuristicas.y + 52;
    for (int i = 0; i < 4; i++) {
        Rectangle item = {painelHeuristicas.x + 12, (float)y, painelHeuristicas.width - 24, 58};
        DrawRectangleRec(item, (Color){20, 20, 34, 220});
        DrawRectangleLinesEx(item, 1.5f, (Color){110, 110, 145, 255});
        DrawText(TextFormat("%02d", i + 1), (int)item.x + 10, (int)item.y + 9, 18, corDestaque);
        DrawText(heuristicas[i], (int)item.x + 48, (int)item.y + 12, 15, corTexto);
        y += 68;
    }

    Rectangle botaoVoltar = {40, 536, 150, 36};
    DrawRectangle((int)botaoVoltar.x + 4, (int)botaoVoltar.y + 4, (int)botaoVoltar.width, (int)botaoVoltar.height, (Color){18, 18, 28, 255});
    DrawRectangleRec(botaoVoltar, (Color){40, 40, 58, 255});
    DrawRectangleLinesEx(botaoVoltar, 2.0f, corBorda);
    DrawText("VOLTAR", (int)(botaoVoltar.x + (botaoVoltar.width - MeasureText("VOLTAR", 18)) / 2), (int)botaoVoltar.y + 9, 18, RAYWHITE);

    const char* rodape = "ESC, ENTER ou B voltar ao menu";
    DrawText(rodape, (GetScreenWidth() - MeasureText(rodape, 18)) / 2, 546, 18, corTexto);
}

/* FIM de analise.c */
