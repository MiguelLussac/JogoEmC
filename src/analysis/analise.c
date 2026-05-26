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

# Função auxiliar para trim de strings (remove espaços em branco no início e fim)
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

# Função auxiliar para parse de linha do arquivo
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

# Implementação das funções da API
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

# Validação de dados: verifica consistência dos campos
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

# Retorna o número de partidas (simples helper).
size_t quantidadePartidas(const HistoricoPartida *arr, size_t count) {
    (void)arr;
    return count;
}

# Estatísticas básicas (operam sobre o array carregado).
double calcularMediaScore(const HistoricoPartida *arr, size_t count) {
    if (!arr || count == 0) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) sum += (double)arr[i].score;
    return sum / (double)count;
}

# Cálculo do desvio padrão amostral
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

# Melhor e pior partida (baseado na pontuação)
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

# Pior partida (baseado na pontuação)
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

# Libera o array retornado por carregarHistorico.
void liberarHistorico(HistoricoPartida *arr) {
    if (!arr) return;
    free(arr);
}

/* FIM de analise.c */
