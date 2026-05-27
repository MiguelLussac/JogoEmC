#include "player.h"
#include "../visual/vfx.h"
#include "raylib.h"
#include <math.h>

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
    float t = (float)GetTime();
    float x = player->posicaoX;
    float y = player->posicaoY;
    bool piscando = player->tempoInvencivel > 0.0f && ((int)(t * 28.0f) % 2 == 0);

    float propulsao = 0.55f + 0.45f * sinf(t * 14.0f);
    bool movendo = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
    float motor = movendo ? propulsao : propulsao * 0.45f;

    Color motorCor = { 0, (unsigned char)(180 + motor * 75), 255, (unsigned char)(90 + motor * 120) };
    vfxDesenharGlowCirculo(x, y + 16.0f, 5.0f + motor * 5.0f, motorCor, 0.8f);
    DrawTriangle(
        (Vector2){ x - 7.0f, y + 14.0f },
        (Vector2){ x + 7.0f, y + 14.0f },
        (Vector2){ x, y + 22.0f + motor * 4.0f },
        (Color){ 255, 120, 40, (unsigned char)(120 + motor * 100) }
    );

    Vector2 nariz = { x, y - 24.0f };
    Vector2 asaE  = { x - 20.0f, y + 14.0f };
    Vector2 asaD  = { x + 20.0f, y + 14.0f };
    Vector2 base  = { x, y + 10.0f };

    Color casco = piscando ? WHITE : (Color){ 70, 230, 255, 255 };
    Color borda = (Color){ 200, 255, 255, 255 };
    DrawTriangle(nariz, asaE, base, casco);
    DrawTriangle(nariz, asaD, base, (Color){ casco.r, casco.g, casco.b, 220 });
    DrawTriangleLines(nariz, asaE, base, borda);
    DrawTriangleLines(nariz, asaD, base, borda);
    DrawLineEx((Vector2){ x, y - 8.0f }, (Vector2){ x, y + 6.0f }, 2.0f, (Color){ 255, 255, 255, 180 });

    if (player->tempoBoostDano > 0.0f) {
        vfxDesenharGlowCirculo(x, y, PLAYER_RADIUS + 4.0f, (Color){ 255, 200, 60, 80 }, 0.6f);
    }
    if (player->tempoBoostVelocidade > 0.0f) {
        DrawCircleLines((int)x, (int)y, PLAYER_RADIUS + 6, (Color){ 100, 220, 255, 200 });
    }
}

void drawPlayerHP(const Player* player) {
    float percentualVida = player->hp > 0 ? (float)player->hp / (float)PLAYER_MAX_HP : 0.0f;
    if (percentualVida > 1.0f) percentualVida = 1.0f;
    int larguraVida = (int)(PLAYER_HEALTH_BAR_WIDTH * percentualVida);
    int bx = PLAYER_HEALTH_BAR_X;
    int by = PLAYER_HEALTH_BAR_Y;

    DrawRectangle(bx - 2, by - 2, PLAYER_HEALTH_BAR_WIDTH + 4, PLAYER_HEALTH_BAR_HEIGHT + 4, (Color){ 10, 20, 40, 200 });
    DrawRectangle(bx, by, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, (Color){ 20, 30, 50, 230 });
    DrawRectangle(bx, by, larguraVida, PLAYER_HEALTH_BAR_HEIGHT, (Color){ 255, 70, 110, 255 });
    DrawRectangleLines(bx, by, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, (Color){ 100, 220, 255, 255 });
    DrawText(TextFormat("HP %d/%d", player->hp, PLAYER_MAX_HP), bx + 8, by + 22, 16, (Color){ 200, 240, 255, 255 });
    DrawText("BUFFS", bx, by - 62, 16, (Color){ 120, 200, 255, 255 });

    int yBuff = by - 42;
    bool temBuffAtivo = false;
    if (player->tempoBoostDano > 0.0f) {
        DrawText(TextFormat("DMG x%d  %.1fs", PLAYER_DAMAGE_BOOST_MULTIPLIER, player->tempoBoostDano), bx, yBuff, 16, (Color){ 255, 220, 80, 255 });
        yBuff += 18;
        temBuffAtivo = true;
    }
    if (player->tempoBoostVelocidade > 0.0f) {
        DrawText(TextFormat("SPD x%.1f  %.1fs", PLAYER_SPEED_BOOST_MULTIPLIER, player->tempoBoostVelocidade), bx, yBuff, 16, SKYBLUE);
        temBuffAtivo = true;
    }
    if (!temBuffAtivo) {
        DrawText("—", bx, yBuff, 16, GRAY);
    }
}

void aplicarDanoPlayer(Player* player, int dano) {
    if (player->tempoInvencivel > 0.0f) return;

    player->hp -= dano;
    if (player->hp < 0) player->hp = 0;
    player->tempoPiscandoDano = PLAYER_DAMAGE_FLASH_DURATION;
    player->tempoInvencivel = PLAYER_INVINCIBILITY_DURATION;
    vfxTremer(vfxObter(), 5.0f, 0.35f);
    vfxExplosao(vfxObter(), player->posicaoX, player->posicaoY, (Color){ 255, 80, 120, 220 }, 10, 90.0f);
}

bool jogadorEstaInvencivel(const Player* player) {
    return player->tempoInvencivel > 0.0f;
}

void atualizarFeedbackDanoPlayer(Player* player, float deltaTime) {
    if (player->tempoInvencivel > 0.0f) {
        player->tempoInvencivel -= deltaTime;
        if (player->tempoInvencivel < 0.0f) player->tempoInvencivel = 0.0f;
    }

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
    VfxEstado* vfx = vfxObter();
    for (int i = 0; i < count; i++) {
        if (!bullets[i].ativa) continue;
        float x = bullets[i].posicaoX;
        float y = bullets[i].posicaoY;
        bool boosted = bullets[i].dano > PLAYER_BASE_BULLET_DAMAGE;
        Color nucleo = boosted ? (Color){ 255, 220, 80, 255 } : (Color){ 120, 255, 255, 255 };
        Color halo   = boosted ? (Color){ 255, 160, 40, 120 } : (Color){ 60, 200, 255, 100 };

        vfxDesenharGlowCirculo(x, y, PLAYER_BULLET_RADIUS, halo, 0.7f);
        vfxDesenharGlowLinha(
            (Vector2){ x, y + 10.0f },
            (Vector2){ x, y - 14.0f },
            boosted ? 3.5f : 2.5f,
            nucleo,
            0.9f
        );
        vfxRastro(vfx, x, y + 6.0f, (Color){ nucleo.r, nucleo.g, nucleo.b, 160 });
    }
}

// Procura uma bala livre no pool e dispara a partir da posicao atual do player.
void atirar(Player* player, Bullet bullets[], int count, float deltaTime) {
    if (player->tempoCooldownTiro > 0.0f) {
        player->tempoCooldownTiro -= deltaTime;
        if (player->tempoCooldownTiro < 0.0f) player->tempoCooldownTiro = 0.0f;
    }
    if (!IsKeyDown(KEY_SPACE) || player->tempoCooldownTiro > 0.0f) return;

    for (int i = 0; i < count; i++) {
        if (!bullets[i].ativa) {
            bullets[i].posicaoX = player->posicaoX;
            bullets[i].posicaoY = player->posicaoY - PLAYER_RADIUS;
            bullets[i].velocidade = 500.0f;
            bullets[i].dano = player->danoTiro > 0 ? player->danoTiro : PLAYER_BASE_BULLET_DAMAGE;
            bullets[i].ativa = true;
            player->tempoCooldownTiro = PLAYER_FIRE_COOLDOWN;
            vfxRastro(vfxObter(), player->posicaoX, player->posicaoY - PLAYER_RADIUS, (Color){ 120, 255, 255, 200 });
            break;
        }
    }
}
