// TODO: Arquivo Questions
// Gera o número secreto
// Avalia o chute
// Devolve StatusChute

#include "questions.h"
#include "raylib.h"

#define NUMEROMIN 1
#define NUMEROMAX 100
#define LIMITE_DIFERENCA 10

int gerarNumeroSecreto() {
    int numeroSecreto;
    numeroSecreto = GetRandomValue(NUMEROMIN, NUMEROMAX);
    return numeroSecreto;
}

void executaQuestao(Questao *questao, int numeroSecreto, int chute){
    
    questao->numeroSecreto = numeroSecreto;
    int diferenca = chute - numeroSecreto;

    if (diferenca == 0) {
        questao->status = correto;
    } else if (diferenca > 0) {
        if (chute > numeroSecreto + LIMITE_DIFERENCA) {
            questao->status = muitoAlto;
        } else {
            questao->status = alto;
        }
    } else {
        if (chute < (numeroSecreto - LIMITE_DIFERENCA)) {
            questao->status = muitoBaixo;
        } else {
            questao->status = baixo;
        }
    }
}