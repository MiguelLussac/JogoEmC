// Structs e Enums
typedef enum {
    // Representados por: 0, 1, 2, 3, 4
    muitoAlto, 
    alto,
    correto, 
    baixo, 
    muitoBaixo
} StatusChute;
// OBS
// Talvez faça sentido adicionar chute, timestamp e numero da tentativa para a struct da questao
typedef struct {
    int numeroSecreto;
    StatusChute status;
} Questao;

// Funções

int gerarNumeroSecreto(void);

void executaQuestao(Questao *questao, int numeroSecreto, int chute);