#include "boss.h"
#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BOSS_BULLET_RADIUS 6
#define BOSS_HEALTH_BAR_WIDTH 240
#define BOSS_HEALTH_BAR_HEIGHT 18

// Converte o centro/tamanho do boss para um retangulo usado na colisao.
static Rectangle getBossRect(const Boss* boss) {
    return (Rectangle) {
        boss->posicaoX - (boss->largura / 2.0f),
        boss->posicaoY - (boss->altura / 2.0f),
        boss->largura,
        boss->altura
    };
}

static float limitarFloat(float valor, float minimo, float maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

static int calcularFaseBoss(const Boss* boss) {
    if (boss->hpMaximo <= 0) return 0;

    float percentualVida = (float)boss->hp / (float)boss->hpMaximo;
    if (percentualVida <= 0.15f) return 3;
    if (percentualVida <= 0.40f) return 2;
    if (percentualVida <= 0.70f) return 1;
    return 0;
}

static float calcularDificuldadeProgressiva(const Boss* boss) {
    if (boss->hpMaximo <= 0) return 1.0f;

    float vidaPerdida = 1.0f - ((float)boss->hp / (float)boss->hpMaximo);
    return 1.0f + limitarFloat(vidaPerdida, 0.0f, 1.0f) * 0.85f;
}

static void atualizarFaseBoss(Boss* boss) {
    int novaFase = calcularFaseBoss(boss);
    if (novaFase != boss->fase) {
        boss->fase = novaFase;
        boss->padraoMovimento = GetRandomValue(0, 3);
        boss->padraoAtaque = GetRandomValue(0, 2);
        boss->tempoMudancaMovimento = 0.25f;
        boss->tempoDash = 0.35f;
    }
}

static void atualizarPadraoMovimento(Boss* boss, const Player* player, float deltaTime) {
    boss->tempoMudancaMovimento -= deltaTime;
    if (boss->tempoMudancaMovimento > 0.0f) return;

    boss->padraoMovimento = GetRandomValue(0, 3);
    boss->tempoMudancaMovimento = (float)GetRandomValue(85, 170) / 100.0f - (float)boss->fase * 0.08f;
    if (boss->tempoMudancaMovimento < 0.55f) boss->tempoMudancaMovimento = 0.55f;

    if (player && GetRandomValue(0, 100) < 50 + boss->fase * 7) {
        boss->direcao = player->posicaoX >= boss->posicaoX ? 1 : -1;
    } else if (GetRandomValue(0, 100) < 45) {
        boss->direcao *= -1;
    }
}

static void executarDashBoss(Boss* boss, const Player* player, float deltaTime) {
    if (boss->duracaoDash > 0.0f) {
        boss->duracaoDash -= deltaTime;
        if (boss->duracaoDash <= 0.0f) {
            boss->direcaoDash = 0.0f;
        }
        return;
    }

    boss->tempoDash -= deltaTime;
    if (boss->tempoDash > 0.0f) return;

    boss->direcaoDash = boss->direcao != 0 ? (float)boss->direcao : 1.0f;
    if (player && GetRandomValue(0, 100) < 75) {
        boss->direcaoDash = player->posicaoX >= boss->posicaoX ? 1.0f : -1.0f;
    }

    boss->duracaoDash = (float)GetRandomValue(12, 22) / 100.0f;
    boss->tempoDash = (float)GetRandomValue(230, 390) / 100.0f - (float)boss->fase * 0.18f;
    if (boss->tempoDash < 1.15f) boss->tempoDash = 1.15f;
}

static int buscarBalaLivre(BossBullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (!bullets[i].ativa) return i;
    }
    return -1;
}

static void dispararBalaBoss(Boss* boss, BossBullet bullets[], int count, const Player* player, float deslocamentoX, float deslocamentoAngulo, float velocidadeExtra) {
    int indice = buscarBalaLivre(bullets, count);
    if (indice < 0 || !player) return;

    float origemX = boss->posicaoX + deslocamentoX;
    float origemY = boss->posicaoY + (boss->altura / 2.0f);
    float direcaoX = player->posicaoX - origemX + deslocamentoAngulo;
    float direcaoY = player->posicaoY - origemY;
    float tamanhoDirecao = sqrtf(direcaoX * direcaoX + direcaoY * direcaoY);

    if (tamanhoDirecao <= 0.0f) return;

    bullets[indice].posicaoX = origemX;
    bullets[indice].posicaoY = origemY;
    bullets[indice].velocidadeX = (direcaoX / tamanhoDirecao) * (BOSS_BULLET_SPEED + velocidadeExtra);
    bullets[indice].velocidadeY = (direcaoY / tamanhoDirecao) * (BOSS_BULLET_SPEED + velocidadeExtra);
    bullets[indice].ativa = true;
}

static float obterCooldownAtaque(const Boss* boss) {
    float dificuldade = calcularDificuldadeProgressiva(boss);
    float cooldown = BOSS_BULLET_COOLDOWN / dificuldade;

    if (boss->fase == 1) cooldown *= 0.90f;
    if (boss->fase == 2) cooldown *= 0.76f;
    if (boss->fase == 3) cooldown *= 0.58f;

    return cooldown < 0.28f ? 0.28f : cooldown;
}

// Aplica dano, aciona feedback visual e desativa o boss quando a vida zera.
static void aplicarDanoBoss(Boss* boss, int dano) {
    if (!boss->ativa) return;

    boss->hp -= dano;
    boss->tempoPiscandoDano = BOSS_DAMAGE_FLASH_DURATION;

    if (boss->hp <= 0) {
        boss->hp = 0;
        boss->ativa = false;
    }
}

void verificarColisaoBalasComPlayer(Player* player, BossBullet bullets[], int count) {
    if (player->hp <= 0) return;

    // Balas do boss causam dano real no player e alimentam a regra de vida do desafio.
    for (int i = 0; i < count; i++) {
        if (!bullets[i].ativa) continue;

        if (CheckCollisionCircles((Vector2){player->posicaoX, player->posicaoY}, PLAYER_RADIUS,
                                  (Vector2){bullets[i].posicaoX, bullets[i].posicaoY}, BOSS_BULLET_RADIUS)) {
            bullets[i].ativa = false;
            aplicarDanoPlayer(player, 1);
        }
    }
}

// Define o estado inicial do boss antes do loop principal comecar.
void inicializarBoss(Boss* boss) {
    boss->posicaoX = 400.0f;
    boss->posicaoY = 100.0f;
    boss->velocidade = 140.0f;
    boss->largura = 80.0f;
    boss->altura = 80.0f;
    boss->hpMaximo = BOSS_MAX_HP;
    boss->hp = boss->hpMaximo;
    boss->ativa = true;
    boss->direcao = 1;
    boss->tempoDesdeUltimoTiro = 0.0f;
    boss->tempoPiscandoDano = 0.0f;
    boss->fase = 0;
    boss->padraoMovimento = 0;
    boss->padraoAtaque = 0;
    boss->tempoIA = 0.0f;
    boss->tempoMudancaMovimento = 0.8f;
    boss->tempoDash = 2.0f;
    boss->duracaoDash = 0.0f;
    boss->direcaoDash = 0.0f;
    boss->tirosRajadaRestantes = 0;
    boss->tempoEntreRajadas = 0.0f;
}

// Move o boss com padrões variáveis, perseguição parcial e dashes agressivos.
void moverBoss(Boss* boss, const Player* player, float deltaTime) {
    if (!boss->ativa) return;

    atualizarFaseBoss(boss);
    atualizarPadraoMovimento(boss, player, deltaTime);
    executarDashBoss(boss, player, deltaTime);

    boss->tempoIA += deltaTime;

    float dificuldade = calcularDificuldadeProgressiva(boss);
    float velocidadeBase = boss->velocidade * dificuldade + (float)boss->fase * 18.0f;
    float oscilacao = sinf(boss->tempoIA * (2.1f + (float)boss->fase * 0.32f)) * (22.0f + (float)boss->fase * 5.0f);
    float direcaoMovimento = (float)boss->direcao;

    if (boss->padraoMovimento == 1 && player) {
        float distanciaX = player->posicaoX - boss->posicaoX;
        direcaoMovimento = limitarFloat(distanciaX / 180.0f, -1.0f, 1.0f);
        if (fabsf(direcaoMovimento) < 0.25f) direcaoMovimento = (float)boss->direcao * 0.55f;
    } else if (boss->padraoMovimento == 2) {
        direcaoMovimento = sinf(boss->tempoIA * (3.2f + (float)boss->fase * 0.35f));
    } else if (boss->padraoMovimento == 3) {
        direcaoMovimento = (float)boss->direcao * (0.55f + fabsf(sinf(boss->tempoIA * 5.5f)));
    }

    if (boss->duracaoDash > 0.0f) {
        direcaoMovimento = boss->direcaoDash;
        velocidadeBase *= 1.85f + (float)boss->fase * 0.18f;
    }

    boss->posicaoX += (velocidadeBase * direcaoMovimento + oscilacao) * deltaTime;

    float margemEsquerda = boss->largura / 2.0f + 24.0f;
    float margemDireita = (float)SCREEN_WIDTH - boss->largura / 2.0f - 24.0f;

    if (boss->posicaoX < margemEsquerda + 55.0f && boss->direcao < 0 && GetRandomValue(0, 100) < 55) {
        boss->direcao = 1;
    }
    if (boss->posicaoX > margemDireita - 55.0f && boss->direcao > 0 && GetRandomValue(0, 100) < 55) {
        boss->direcao = -1;
    }

    if (boss->posicaoX < margemEsquerda) {
        boss->posicaoX = margemEsquerda;
        boss->direcao = 1;
        boss->duracaoDash = 0.0f;
    }
    if (boss->posicaoX > margemDireita) {
        boss->posicaoX = margemDireita;
        boss->direcao = -1;
        boss->duracaoDash = 0.0f;
    }
}

// Desenha o boss e alterna a cor durante o feedback de dano.
void drawBoss(Boss* boss) {
    if (boss->ativa) {
        int framePiscando = (int)(boss->tempoPiscandoDano * 20.0f);
        Color corBoss = (boss->tempoPiscandoDano > 0.0f && framePiscando % 2 == 0) ? WHITE : RED;

        DrawRectangle(
            (int)boss->posicaoX - (int)boss->largura / 2,
            (int)boss->posicaoY - (int)boss->altura / 2,
            (int)boss->largura,
            (int)boss->altura,
            corBoss
        );
    }
}

// Reduz o timer de pisca ate encerrar o feedback visual de dano.
void atualizarFeedbackDanoBoss(Boss* boss, float deltaTime) {
    if (boss->tempoPiscandoDano <= 0.0f) return;

    boss->tempoPiscandoDano -= deltaTime;
    if (boss->tempoPiscandoDano < 0.0f) {
        boss->tempoPiscandoDano = 0.0f;
    }
}

// Verifica balas do jogador contra o retangulo do boss e aplica dano.
void verificarColisaoBalasComBoss(Boss* boss, Bullet bullets[], int count) {
    if (!boss->ativa) return;

    Rectangle bossRect = getBossRect(boss);

    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa &&
            CheckCollisionCircleRec((Vector2){ bullets[i].posicaoX, bullets[i].posicaoY }, PLAYER_BULLET_RADIUS, bossRect)) {
            bullets[i].ativa = false;
            aplicarDanoBoss(boss, bullets[i].dano);
            if (!boss->ativa) return;
        }
    }
}

// Renderiza a barra de vida do boss no topo central da tela.
void drawBarraVidaBossEm(const Boss* boss, int posicaoY) {
    int posicaoX = (SCREEN_WIDTH - BOSS_HEALTH_BAR_WIDTH) / 2;
    float percentualVida = boss->hpMaximo > 0 ? (float)boss->hp / (float)boss->hpMaximo : 0.0f;
    int larguraVida = (int)(BOSS_HEALTH_BAR_WIDTH * percentualVida);

    DrawRectangle(posicaoX, posicaoY, BOSS_HEALTH_BAR_WIDTH, BOSS_HEALTH_BAR_HEIGHT, DARKGRAY);
    DrawRectangle(posicaoX, posicaoY, larguraVida, BOSS_HEALTH_BAR_HEIGHT, RED);
    DrawRectangleLines(posicaoX, posicaoY, BOSS_HEALTH_BAR_WIDTH, BOSS_HEALTH_BAR_HEIGHT, WHITE);
    DrawText(TextFormat("BOSS HP: %d/%d", boss->hp, boss->hpMaximo), posicaoX + 50, posicaoY + 24, 16, WHITE);
}

void drawBarraVidaBoss(const Boss* boss) {
    drawBarraVidaBossEm(boss, 20);
}

// Zera o pool de projeteis do boss para todos iniciarem inativos.
void inicializarBalasBoss(BossBullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        bullets[i].posicaoX = 0.0f;
        bullets[i].posicaoY = 0.0f;
        bullets[i].velocidadeX = 0.0f;
        bullets[i].velocidadeY = 0.0f;
        bullets[i].ativa = false;
    }
}

// Controla cooldown e alterna tiros simples, múltiplos e rajadas progressivas.
void atualizarTiroBoss(Boss* boss, BossBullet bullets[], int count, const Player* player, float deltaTime) {
    if (!boss->ativa) return;

    atualizarFaseBoss(boss);

    if (boss->tirosRajadaRestantes > 0) {
        boss->tempoEntreRajadas -= deltaTime;
        if (boss->tempoEntreRajadas <= 0.0f) {
            float abertura = 14.0f + (float)boss->fase * 7.0f;
            dispararBalaBoss(boss, bullets, count, player, 0.0f, abertura * (float)(boss->tirosRajadaRestantes % 2 == 0 ? 1 : -1), (float)boss->fase * 16.0f);
            boss->tirosRajadaRestantes--;
            boss->tempoEntreRajadas = 0.13f;
        }
    }

    boss->tempoDesdeUltimoTiro += deltaTime;
    if (boss->tempoDesdeUltimoTiro < obterCooldownAtaque(boss)) return;

    boss->padraoAtaque = GetRandomValue(0, boss->fase >= 2 ? 3 : 2);

    if (boss->fase == 0) {
        dispararBalaBoss(boss, bullets, count, player, 0.0f, 0.0f, 0.0f);
    } else if (boss->fase == 1) {
        dispararBalaBoss(boss, bullets, count, player, -14.0f, -16.0f, 8.0f);
        dispararBalaBoss(boss, bullets, count, player, 14.0f, 16.0f, 8.0f);
    } else if (boss->fase == 2) {
        dispararBalaBoss(boss, bullets, count, player, -18.0f, -24.0f, 18.0f);
        dispararBalaBoss(boss, bullets, count, player, 0.0f, 0.0f, 18.0f);
        dispararBalaBoss(boss, bullets, count, player, 18.0f, 24.0f, 18.0f);
        if (boss->padraoAtaque == 3) {
            boss->tirosRajadaRestantes = 2;
            boss->tempoEntreRajadas = 0.12f;
        }
    } else {
        dispararBalaBoss(boss, bullets, count, player, -24.0f, -34.0f, 30.0f);
        dispararBalaBoss(boss, bullets, count, player, 0.0f, 0.0f, 30.0f);
        dispararBalaBoss(boss, bullets, count, player, 24.0f, 34.0f, 30.0f);
        boss->tirosRajadaRestantes = 3;
        boss->tempoEntreRajadas = 0.10f;
    }

    boss->tempoDesdeUltimoTiro = 0.0f;
}

// Move os projeteis do boss e desativa os que saem dos limites da tela.
void moverBalasBoss(BossBullet bullets[], int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            bullets[i].posicaoX += bullets[i].velocidadeX * deltaTime;
            bullets[i].posicaoY += bullets[i].velocidadeY * deltaTime;

            if (bullets[i].posicaoX < 0.0f || bullets[i].posicaoX > SCREEN_WIDTH ||
                bullets[i].posicaoY < 0.0f || bullets[i].posicaoY > SCREEN_HEIGHT) {
                bullets[i].ativa = false;
            }
        }
    }
}

// Desenha apenas as balas ativas do boss.
void drawBalasBoss(BossBullet bullets[], int count) {
    for (int i = 0; i < count; i++) {
        if (bullets[i].ativa) {
            DrawCircle((int)bullets[i].posicaoX, (int)bullets[i].posicaoY, BOSS_BULLET_RADIUS, ORANGE);
        }
    }
}
