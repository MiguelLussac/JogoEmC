typedef enum {
    // Representados por: 0, 1, 2, 3, 4
    muitoAlto, 
    alto,
    correto, 
    baixo, 
    muitoBaixo
} StatusChute;

typedef struct {
    int numeroSecreto;
    StatusChute status;
} Questao;

int gerarNumeroSecreto(void);