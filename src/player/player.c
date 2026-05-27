#include "player.h"
#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_RADIUS 20

// Move o jogador no eixo X e limita a posicao dentro da largura da tela.
void moverEsquerdaDireita(Player* player, float deltaTime) {
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->posicaoX -= player->velocidade * deltaTime;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->posicaoX += player->velocidade * deltaTime;
    }
    if (player->posicaoX < PLAYER_RADIUS) player->posicaoX = PLAYER_RADIUS;
    if (player->posicaoX > SCREEN_WIDTH - PLAYER_RADIUS) player->posicaoX = SCREEN_WIDTH - PLAYER_RADIUS;
}

void drawPlayer(Player* player) {
    // Pisca o player apos dano para comunicar invulnerabilidade/feedback visual.
    int framePiscando = (int)(player->tempoPiscandoDano * 20.0f);
    Color corJogador = (player->tempoPiscandoDano > 0.0f && framePiscando % 2 == 0) ? WHITE : RED;
    DrawCircle((int)player->posicaoX, (int)player->posicaoY, PLAYER_RADIUS, corJogador);
}

void drawPlayerHP(const Player* player) {
    // Barra de vida usada pelo desafio para mostrar o custo da ultima tentativa.
    float percentualVida = player->hp > 0 ? (float)player->hp / (float)PLAYER_MAX_HP : 0.0f;
    if (percentualVida > 1.0f) percentualVida = 1.0f;
    int larguraVida = (int)(PLAYER_HEALTH_BAR_WIDTH * percentualVida);

    DrawRectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, DARKGRAY);
    DrawRectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, larguraVida, PLAYER_HEALTH_BAR_HEIGHT, RED);
    DrawRectangleLines(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, WHITE);
    DrawText(TextFormat("PLAYER HP: %d/%d", player->hp, PLAYER_MAX_HP), PLAYER_HEALTH_BAR_X + 8, PLAYER_HEALTH_BAR_Y + 22, 16, WHITE);
    DrawText(TextFormat("ESCUDOS: %d", player->escudos), PLAYER_HEALTH_BAR_X + 8, PLAYER_HEALTH_BAR_Y + 40, 16, SKYBLUE);
    DrawText("BUFFS ATIVOS:", PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y - 62, 16, RAYWHITE);

    int yBuff = PLAYER_HEALTH_BAR_Y - 42;
    bool temBuffAtivo = false;
    if (player->tempoBoostDano > 0.0f) {
        DrawText(TextFormat("DANO x%d: %.1fs", PLAYER_DAMAGE_BOOST_MULTIPLIER, player->tempoBoostDano), PLAYER_HEALTH_BAR_X, yBuff, 16, YELLOW);
        yBuff += 18;
        temBuffAtivo = true;
    }
    if (player->tempoBoostVelocidade > 0.0f) {
        DrawText(TextFormat("VEL x%.1f: %.1fs", PLAYER_SPEED_BOOST_MULTIPLIER, player->tempoBoostVelocidade), PLAYER_HEALTH_BAR_X, yBuff, 16, SKYBLUE);
        temBuffAtivo = true;
    }
    if (!temBuffAtivo) {
        DrawText("Nenhum", PLAYER_HEALTH_BAR_X, yBuff, 16, GRAY);
    }
}

void aplicarDanoPlayer(Player* player, int dano) {
    // Tambem usado pelo desafio quando o player aceita gastar 1 vida.
    while (dano > 0 && player->escudos > 0) {
        player->escudos--;
        dano--;
    }
    player->hp -= dano;
    if (player->hp < 0) player->hp = 0;
    player->tempoPiscandoDano = PLAYER_DAMAGE_FLASH_DURATION;
}

// Reduz o timer do pisca de dano ate o player voltar ao desenho normal.
void atualizarFeedbackDanoPlayer(Player* player, float deltaTime) {
    if (player->tempoPiscandoDano <= 0.0f) return;
    player->tempoPiscandoDano -= deltaTime;
    if (player->tempoPiscandoDano < 0.0f) player->tempoPiscandoDano = 0.0f;
}

void aplicarBoostDanoPlayer(Player* player) {
    player->danoTiro = PLAYER_BASE_BULLET_DAMAGE * PLAYER_DAMAGE_BOOST_MULTIPLIER;
    player->tempoBoostDano = PLAYER_DAMAGE_BOOST_DURATION;
}

void atualizarBoostDanoPlayer(Player* player, float deltaTime) {
    if (player->tempoBoostDano <= 0.0f) return;

    player->tempoBoostDano -= deltaTime;
    if (player->tempoBoostDano <= 0.0f) {
        player->tempoBoostDano = 0.0f;
        player->danoTiro = PLAYER_BASE_BULLET_DAMAGE;
    }
}

void aplicarBoostVelocidadePlayer(Player* player) {
    player->velocidade = PLAYER_BASE_SPEED * PLAYER_SPEED_BOOST_MULTIPLIER;
    player->tempoBoostVelocidade = PLAYER_SPEED_BOOST_DURATION;
}

void atualizarBoostVelocidadePlayer(Player* player, float deltaTime) {
    if (player->tempoBoostVelocidade <= 0.0f) return;

    player->tempoBoostVelocidade -= deltaTime;
    if (player->tempoBoostVelocidade <= 0.0f) {
        player->tempoBoostVelocidade = 0.0f;
        player->velocidade = PLAYER_BASE_SPEED;
    }
}

// Atualiza as balas do jogador e desativa as que saem pelo topo da tela.
void moverBalas(Bullet bullets[], int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            bullets[i].posicaoY -= bullets[i].velocidade * deltaTime;
            if (bullets[i].posicaoY < 0) {
                bullets[i].ativa = false;
            }
        }
    }
}

// Renderiza somente as balas ativas do pool do jogador.
void drawBalas(Bullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            Color corBala = bullets[i].dano > PLAYER_BASE_BULLET_DAMAGE ? ORANGE : YELLOW;
            DrawCircle((int)bullets[i].posicaoX, (int)bullets[i].posicaoY, PLAYER_BULLET_RADIUS, corBala);
        }
    }
}

// Procura uma bala livre no pool e dispara a partir da posicao atual do player.
void atirar(Player* player, Bullet bullets[], int count) {
    if (IsKeyPressed(KEY_SPACE)) {
        for (int i = 0; i < count; i++) {
            if (!bullets[i].ativa) {
                bullets[i].posicaoX = player->posicaoX;
                bullets[i].posicaoY = player->posicaoY - PLAYER_RADIUS; // Posição inicial da bala
                bullets[i].velocidade = 500.0f; // Velocidade aumentada para balas mais rápidas
                bullets[i].dano = player->danoTiro > 0 ? player->danoTiro : PLAYER_BASE_BULLET_DAMAGE;
                bullets[i].ativa = true;
                break;
            }
        }
    }
}
