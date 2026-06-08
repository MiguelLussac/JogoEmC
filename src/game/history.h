#ifndef HISTORY_H
#define HISTORY_H

#include "partida.h"

#define MAX_HISTORICO_LINHAS 128
#define TAMANHO_LINHA_HISTORICO 180
#define CAMINHO_HISTORICO "historico_relatorios.txt"

void salvarRelatorioHistorico(MotivoFimJogo motivoFimJogo, const EstatisticasPartida* stats);
int carregarHistoricoRelatorios(char linhas[][TAMANHO_LINHA_HISTORICO], int maxLinhas);

#endif
