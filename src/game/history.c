#include "history.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void salvarRelatorioHistorico(MotivoFimJogo motivoFimJogo, const EstatisticasPartida* stats) {
    FILE* arquivo = fopen(CAMINHO_HISTORICO, "a");
    if (arquivo == NULL) return;

    time_t agora = time(NULL);
    struct tm* infoLocal = localtime(&agora);
    char dataHora[32] = "sem-data";
    if (infoLocal != NULL) {
        strftime(dataHora, sizeof(dataHora), "%d/%m/%Y %H:%M", infoLocal);
    }

    if (stats->modo == MODO_LOGICO) {
        int total = stats->logicAcertos + stats->logicErros;
        float taxa = total > 0 ? ((float)stats->logicAcertos * 100.0f) / (float)total : 0.0f;
        fprintf(arquivo,
                "[%s] [LOGICO] %s | tempo: %.1fs | expressoes: %d/%d (%.1f%%) | combo max: %d | powerups: %d | buffs: %d\n",
                dataHora,
                textoMotivoFimJogo(motivoFimJogo),
                stats->tempoPartida,
                stats->logicAcertos,
                total,
                taxa,
                stats->logicComboMax,
                stats->logicPowerUps,
                stats->logicBuffs);
    } else {
        float precisao = stats->tirosAtirados > 0
            ? ((float)stats->acertosNoBoss * 100.0f) / (float)stats->tirosAtirados
            : 0.0f;
        float taxaDesafios = stats->desafiosIniciados > 0
            ? ((float)stats->desafiosVencidos * 100.0f) / (float)stats->desafiosIniciados
            : 0.0f;

        fprintf(arquivo,
                "[%s] [ARCADE] %s | tempo: %.1fs | tiros: %d | acertos boss: %d | precisao: %.1f%% | desafios: %d/%d (%.1f%%) | buffs: %d\n",
                dataHora,
                textoMotivoFimJogo(motivoFimJogo),
                stats->tempoPartida,
                stats->tirosAtirados,
                stats->acertosNoBoss,
                precisao,
                stats->desafiosVencidos,
                stats->desafiosIniciados,
                taxaDesafios,
                stats->arcadeBuffs);
    }

    fclose(arquivo);
}

int carregarHistoricoRelatorios(char linhas[][TAMANHO_LINHA_HISTORICO], int maxLinhas) {
    FILE* arquivo = fopen(CAMINHO_HISTORICO, "r");
    if (arquivo == NULL) return 0;

    int count = 0;
    while (count < maxLinhas && fgets(linhas[count], TAMANHO_LINHA_HISTORICO, arquivo) != NULL) {
        size_t tamanho = strlen(linhas[count]);
        if (tamanho > 0 && linhas[count][tamanho - 1] == '\n') {
            linhas[count][tamanho - 1] = '\0';
        }
        count++;
    }
    fclose(arquivo);
    return count;
}
