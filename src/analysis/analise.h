/* analise.h
 * Estrutura e funções para análise estatística de partidas
 * Implementa a struct HistoricoPartida e a API para carregar/validar
 * e calcular estatísticas básicas (média, melhor, pior, desvio).
 * Inclui funções recursivas obrigatórias e geração de relatório analítico.
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
    int vitoria;      /* 1 se o boss foi derrotado */
    int modoLogico;   /* 1 para modo LOGICO, 0 para ARCADE */
    int tiros;        /* tiros disparados no modo arcade */
    double precisao;  /* precisao em percentual */
    int buffs;        /* buffs coletados/aplicados */
    int desafiosVencidos;
    int desafiosTotal;
    int comboMax;
} HistoricoPartida;

/* Relatório analítico completo com todas as estatísticas e heurísticas. */
typedef struct RelatorioAnalitico {
    size_t quantidadePartidas;
    double mediaScore;
    double desvioPadrao;
    int melhorScore;
    int piorScore;
    double melhorTempo;
    double piorTempo;
    double mediaAcertos;
    double taxaSucessoMedia;
    double taxaVitoria;
    double precisaoMedia;
    double tempoMedio;
    double buffsMedios;
    char heuristicas[4][256];  /* até 4 heurísticas textuais */
    int quantidadeHeuristicas;
} RelatorioAnalitico;

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

/* ===== FUNÇÕES RECURSIVAS OBRIGATÓRIAS ===== */

/* Calcula a soma recursiva dos scores.
 * Parâmetros: arr (array), idx (índice atual, iniciar com 0), count (tamanho total)
 * Caso base: idx >= count, retorna 0
 */
int somaRecursiva(const HistoricoPartida *arr, size_t idx, size_t count);

/* Encontra o valor mínimo recursivamente nos scores.
 * Parâmetros: arr (array), idx (índice atual, iniciar com 0), count (tamanho total)
 * Caso base: idx >= count-1, retorna arr[0].score
 */
int minimoRecursivo(const HistoricoPartida *arr, size_t idx, size_t count);

/* Encontra o valor máximo recursivamente nos scores.
 * Parâmetros: arr (array), idx (índice atual, iniciar com 0), count (tamanho total)
 * Caso base: idx >= count-1, retorna arr[0].score
 */
int maximoRecursivo(const HistoricoPartida *arr, size_t idx, size_t count);

/* Calcula a soma dos quadrados dos scores recursivamente.
 * Parâmetros: arr (array), idx (índice atual, iniciar com 0), count (tamanho total)
 * Caso base: idx >= count, retorna 0
 */
double somaQuadradosRecursiva(const HistoricoPartida *arr, size_t idx, size_t count);

/* ===== FUNÇÕES DE ANÁLISE E RELATÓRIO ===== */

/* Gera relatório analítico completo a partir do histórico.
 * Calcula todas as estatísticas e heurísticas estratégicas.
 */
RelatorioAnalitico gerarRelatorioAnalitico(const HistoricoPartida *arr, size_t count);

/* Gera heurísticas estratégicas textuais baseadas no desempenho.
 * Preenche o array de heurísticas do relatório.
 */
void gerarHeuristicas(const HistoricoPartida *arr, size_t count,
                      const RelatorioAnalitico *relatorio,
                      char heuristicas[][256], int *quantidadeHeuristicas);

/* Renderiza o relatório analítico na tela usando Raylib.
 * Exibe em seções: título, estatísticas, desempenho e heurísticas.
 * Integra-se com a interface do jogo para exibição organizada e legível.
 */
void renderizarRelatorioAnalitico(const RelatorioAnalitico *relatorio);

/* Libera o array retornado por carregarHistorico. */
void liberarHistorico(HistoricoPartida *arr);

#endif /* ANALISE_H */
