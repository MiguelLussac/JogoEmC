// TODO: Arquivo Questions
// Gera o número secreto
// Avalia o chute
// Devolve StatusChute

#include "questions.h"
#include "raylib.h"

#define NUMEROMIN 1
#define NUMEROMAX 100

int gerarNumeroSecreto() {
    int numeroSecreto;
    numeroSecreto = GetRandomValue(1, 100);
    return numeroSecreto;
}