#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int posicaoX;
    int velocidade;
} Player;

void moverEsquerdaDireita(Player* player);
void drawPlayer(Player* player);

#endif // PLAYER_H