/* analise.h
 * Estrutura e funções para análise estatística de partidas
 * Implementa a struct HistoricoPartida e a API para carregar/validar
 * e calcular estatísticas básicas (média, melhor, pior, desvio).
 */

#ifndef ANALISE_H
#define ANALISE_H

#include <stddef.h>

/* Representa um registro histórico de uma partida. */
typedef struct HistoricoPartida {
    int id;           /* identificador da partida (opcional) */
    int score;        /* pontuação obtida na partida */
    int acertos;      /* número de acertos */
    int tentativas;   /* número de tentativas realizadas */
    double tempo;     /* tempo da partida em segundos */
} HistoricoPartida;

/* Códigos de erro retornados pelas funções de carregamento/validação */
typedef enum {
    HIST_OK = 0,
    HIST_ERR_NOFILE = 1,
    HIST_ERR_EMPTY = 2,
    HIST_ERR_INVALID = 3,
    HIST_ERR_NOMEM = 4
} HistoricoErro;

/* Carrega o histórico do arquivo 'caminho'.
 * O arquivo deve conter um registro por linha com 5 campos:
 * id score acertos tentativas tempo
 * separados por espaços ou por vírgula.
 * Ao sucesso, retorna HIST_OK, *outArray aponta para um array alocado
 * (liberar com liberarHistorico) e *outCount contém o número de registros.
 */
HistoricoErro carregarHistorico(const char *caminho, HistoricoPartida **outArray, size_t *outCount);

/* Valida um array de históricos; retorna HIST_OK se válido ou
 * HIST_ERR_INVALID em caso de dados inconsistentes.
 */
HistoricoErro validarDadosHistorico(const HistoricoPartida *arr, size_t count);

/* Retorna o número de partidas (simples helper). */
size_t quantidadePartidas(const HistoricoPartida *arr, size_t count);

/* Estatísticas básicas (operam sobre o array carregado).
 * Para funções que retornam um registro (melhor/pior), o parâmetro
 * found é preenchido com 1 se houver resultado, 0 caso contrário.
 */
double calcularMediaScore(const HistoricoPartida *arr, size_t count);
double calcularDesvioPadraoScore(const HistoricoPartida *arr, size_t count);
HistoricoPartida melhorPartida(const HistoricoPartida *arr, size_t count, int *found);
HistoricoPartida piorPartida(const HistoricoPartida *arr, size_t count, int *found);

/* Libera o array retornado por carregarHistorico. */
void liberarHistorico(HistoricoPartida *arr);

#endif /* ANALISE_H */
