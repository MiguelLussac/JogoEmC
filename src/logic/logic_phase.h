#ifndef LOGIC_PHASE_H
#define LOGIC_PHASE_H

#include "raylib.h"
#include <stdbool.h>
#include "../boss/boss.h"
#include "../player/player.h"
#include "../game/partida.h"

#define LOGIC_MAX_DROPS   24
#define LOGIC_DANO_BASE    4

typedef enum { PORTA_AND, PORTA_OR, PORTA_XOR } PortaLogica;

typedef enum {
    DROP_V,
    DROP_F,
    DROP_OP_AND,
    DROP_OP_OR,
    DROP_OP_XOR,
    DROP_BOMB,
    DROP_FREEZE,
    DROP_AUTOCOMPLETE,
    DROP_SLOW,
    DROP_SHIELD,
    DROP_BOOST_HP,
    DROP_BOOST_DMG,
    DROP_BOOST_SPD,
} TipoDrop;

typedef enum {
    EXPR_AB,           /* [?] PORTA [?] = obj            */
    EXPR_NOT,          /* NOT [?] = obj                   */
    EXPR_A_FIXO,       /* vA PORTA [?] = obj              */
    EXPR_B_FIXO,       /* [?] PORTA vB = obj              */
    EXPR_ESCOLHER_OP,  /* vA [?] vB = obj (operator slot) */
} TipoExpressao;

typedef enum {
    EVENTO_NENHUM,
    EVENTO_CHUVA,
    EVENTO_RELAMPAGO,
    EVENTO_NOT_MODE,
    EVENTO_FREEZE_TIROS,
} TipoEvento;

typedef struct {
    float    x, y;
    TipoDrop tipo;
    bool     ativo;
} PremissaDrop;

typedef struct {
    /* expressão atual */
    TipoExpressao tipoExpr;
    PortaLogica   porta;
    bool          valFixo[2];
    bool          objetivo;

    /* slots a preencher */
    int  slotsTotal;
    bool slotPreenchido[2];
    bool slotEhValor[2];   /* true = booleano, false = operador */
    bool slotValor[2];
    PortaLogica slotOp;    /* para EXPR_ESCOLHER_OP */

    /* pressão temporal */
    bool  temTimer;
    float timerExpr;
    float timerMaxExpr;

    /* drops */
    PremissaDrop drops[LOGIC_MAX_DROPS];
    float timerSpawn;
    float intervaloSpawn;

    /* estado da rodada */
    float timerLaser;
    float timerCurto;
    float timerPausa;
    bool  laserAtivo;
    bool  curtoAtivo;
    bool  pausaRodada;
    bool  ativa;

    /* sistema de combo */
    int   combo;
    float timerComboTexto;
    int   nivelLaser;     /* 0..3, aumenta com combo */

    /* power-ups ativos */
    float timerSlow;
    float timerShield;
    float timerFreeze;
    bool  shieldAtivo;

    /* feedback visual */
    float flashTimer;
    Color flashCor;
    float shakeTimer;
    float shakeX, shakeY;
    char  textoFeedback[48];
    float timerFeedback;
    Color corFeedback;

    /* eventos especiais */
    TipoEvento eventoAtual;
    float timerEvento;
    float timerProxEvento;

    /* fase da expressão (0 básico, 1 operadores, 2 full mix) */
    int faseExpr;

    /* anti-repetição do desafio anterior */
    TipoExpressao ultimoTipo;
    PortaLogica   ultimoPorta;
    bool          ultimoObjetivo;
    bool          ultimoVal0;
    bool          ultimoVal1;
    int           indiceRodada;
    int           rodadasAcertadas;
    int           rodadasErradas;
    int           comboMaximo;
    int           powerUpsColetados;
    int           buffsObtidos;
} FaseLogica;

void inicializarFaseLogica(FaseLogica* fase, Boss* boss, Player* jogador);
void atualizarFaseLogica(FaseLogica* fase, Boss* boss, Player* jogador,
                         BossBullet balasBoss[], int maxBoss, float deltaTime);
void desenharFaseLogica(const FaseLogica* fase, const Boss* boss, const Player* jogador);
void sincronizarStatsFaseLogica(const FaseLogica* fase, EstatisticasPartida* stats);

#endif
