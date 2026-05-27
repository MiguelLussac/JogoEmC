#include "partida.h"
#include "screen_config.h"

void inicializarPartida(Player* jogador, Bullet balasJogador[], Boss* boss, BossBullet balasBoss[],
                        Estrela* estrela, DesafioPergunta* desafio, bool* perguntaAtiva,
                        bool* jogoEncerrado, MotivoFimJogo* motivoFimJogo, EstatisticasPartida* stats) {
    jogador->posicaoX = SCREEN_WIDTH / 2.0f;
    jogador->posicaoY = SCREEN_HEIGHT - 100.0f;
    jogador->velocidade = PLAYER_BASE_SPEED;
    jogador->hp = PLAYER_MAX_HP;
    jogador->tempoPiscandoDano = 0.0f;
    jogador->tempoInvencivel = 0.0f;
    jogador->danoTiro = PLAYER_BASE_BULLET_DAMAGE;
    jogador->tempoBoostDano = 0.0f;
    jogador->tempoBoostVelocidade = 0.0f;
    jogador->tempoCooldownTiro = 0.0f;

    for (int i = 0; i < MAX_BULLETS; i++) {
        balasJogador[i].posicaoX = SCREEN_WIDTH / 2.0f;
        balasJogador[i].posicaoY = SCREEN_HEIGHT - 100.0f;
        balasJogador[i].velocidade = 500.0f;
        balasJogador[i].dano = PLAYER_BASE_BULLET_DAMAGE;
        balasJogador[i].ativa = false;
    }

    inicializarBoss(boss);
    inicializarBalasBoss(balasBoss, MAX_BOSS_BULLETS);
    inicializarEstrela(estrela);
    inicializarDesafio(desafio);

    *perguntaAtiva = false;
    *jogoEncerrado = false;
    *motivoFimJogo = FIM_JOGO_NENHUM;

    stats->tirosAtirados = 0;
    stats->acertosNoBoss = 0;
    stats->desafiosIniciados = 0;
    stats->desafiosVencidos = 0;
    stats->tempoPartida = 0.0f;
    stats->logicAcertos = 0;
    stats->logicErros = 0;
    stats->logicComboMax = 0;
    stats->logicPowerUps = 0;
    stats->logicBuffs = 0;
}

const char* textoMotivoFimJogo(MotivoFimJogo motivoFimJogo) {
    switch (motivoFimJogo) {
        case FIM_JOGO_BOSS_DERROTADO: return "Boss derrotado";
        case FIM_JOGO_PLAYER_DERROTADO: return "Player derrotado";
        case FIM_JOGO_NENHUM:
        default: return "Fim de jogo";
    }
}
