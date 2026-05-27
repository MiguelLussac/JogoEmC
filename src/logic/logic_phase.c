#include "logic_phase.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ── constantes ─────────────────────────────────────────────────────────────── */
#define SCREEN_W     800
#define SCREEN_H     600
#define DROP_W       36.0f
#define DROP_H       36.0f
#define DROP_VEL     115.0f
#define LASER_DUR    0.40f
#define CURTO_DUR    0.55f
#define PAUSA_DUR    0.55f
#define SPAWN_BASE   2.2f
#define SPAWN_MIN    1.1f
#define TIMER_BASE   10.0f
#define TIMER_MIN     3.5f

/* ── helpers internos ────────────────────────────────────────────────────────── */

static float pctVida(const Boss* boss) {
    if (!boss || boss->hpMaximo <= 0) return 1.0f;
    return (float)boss->hp / (float)boss->hpMaximo;
}

static bool calcPorta(PortaLogica p, bool a, bool b) {
    switch (p) {
        case PORTA_AND: return a && b;
        case PORTA_OR:  return a || b;
        case PORTA_XOR: return (a != b);
    }
    return false;
}

static void setFeedback(FaseLogica* f, const char* txt, Color cor) {
    strncpy(f->textoFeedback, txt, sizeof(f->textoFeedback) - 1);
    f->textoFeedback[sizeof(f->textoFeedback) - 1] = '\0';
    f->corFeedback   = cor;
    f->timerFeedback = 1.6f;
}

static void setFlash(FaseLogica* f, Color cor, float dur) {
    f->flashCor   = cor;
    f->flashTimer = dur;
}

static void limparSlots(FaseLogica* f) {
    f->slotPreenchido[0] = false;
    f->slotPreenchido[1] = false;
}

static int buscarDropLivre(const FaseLogica* f) {
    for (int i = 0; i < LOGIC_MAX_DROPS; i++)
        if (!f->drops[i].ativo) return i;
    return -1;
}

/* ── fase da expressão baseada em HP do boss ─────────────────────────────────── */
static int calcFaseExpr(const Boss* boss) {
    float p = pctVida(boss);
    if (p > 0.60f) return 0;
    if (p > 0.25f) return 1;
    return 2;
}

/* ── configuração de slots por tipo de expressão ─────────────────────────────── */
static void configurarSlots(FaseLogica* f) {
    limparSlots(f);
    switch (f->tipoExpr) {
        case EXPR_AB:
            f->slotsTotal    = 2;
            f->slotEhValor[0] = true;
            f->slotEhValor[1] = true;
            break;
        case EXPR_NOT:
        case EXPR_A_FIXO:
        case EXPR_B_FIXO:
            f->slotsTotal    = 1;
            f->slotEhValor[0] = true;
            break;
        case EXPR_ESCOLHER_OP:
            f->slotsTotal    = 1;
            f->slotEhValor[0] = false;
            break;
    }
}

/* ── gerador de expressão ────────────────────────────────────────────────────── */

static bool mesmoDesafio(const FaseLogica* f) {
    if (f->indiceRodada <= 0) return false;
    return f->tipoExpr == f->ultimoTipo
        && f->porta == f->ultimoPorta
        && f->objetivo == f->ultimoObjetivo
        && f->valFixo[0] == f->ultimoVal0
        && f->valFixo[1] == f->ultimoVal1;
}

static void registrarDesafioAtual(FaseLogica* f) {
    f->ultimoTipo     = f->tipoExpr;
    f->ultimoPorta    = f->porta;
    f->ultimoObjetivo = f->objetivo;
    f->ultimoVal0     = f->valFixo[0];
    f->ultimoVal1     = f->valFixo[1];
}

static PortaLogica sortearPorta(int faseMax) {
    int r = GetRandomValue(0, faseMax);
    if (r == 0) return PORTA_AND;
    if (r == 1) return PORTA_OR;
    return PORTA_XOR;
}

static void montarExpressao(FaseLogica* f, int fe) {
    f->valFixo[0] = false;
    f->valFixo[1] = false;
    f->objetivo = (bool)GetRandomValue(0, 1);

    if (f->eventoAtual == EVENTO_NOT_MODE) {
        f->tipoExpr = EXPR_NOT;
        configurarSlots(f);
        return;
    }

    int r = GetRandomValue(0, 99);

    if (fe == 0) {
        if (r < 30) {
            f->tipoExpr = EXPR_AB;
            f->porta    = sortearPorta(1);
        } else if (r < 50) {
            f->tipoExpr = EXPR_NOT;
        } else if (r < 75) {
            f->tipoExpr   = EXPR_A_FIXO;
            f->porta      = sortearPorta(1);
            f->valFixo[0] = (bool)GetRandomValue(0, 1);
        } else {
            f->tipoExpr   = EXPR_B_FIXO;
            f->porta      = sortearPorta(1);
            f->valFixo[1] = (bool)GetRandomValue(0, 1);
        }
    } else if (fe == 1) {
        if (r < 28) {
            f->tipoExpr = EXPR_AB;
            f->porta    = sortearPorta(2);
        } else if (r < 44) {
            f->tipoExpr = EXPR_NOT;
        } else if (r < 60) {
            f->tipoExpr   = EXPR_A_FIXO;
            f->porta      = sortearPorta(2);
            f->valFixo[0] = (bool)GetRandomValue(0, 1);
        } else if (r < 76) {
            f->tipoExpr   = EXPR_B_FIXO;
            f->porta      = sortearPorta(2);
            f->valFixo[1] = (bool)GetRandomValue(0, 1);
        } else {
            f->tipoExpr   = EXPR_ESCOLHER_OP;
            f->valFixo[0] = (bool)GetRandomValue(0, 1);
            f->valFixo[1] = (bool)GetRandomValue(0, 1);
        }
    } else {
        if (r < 22) {
            f->tipoExpr = EXPR_AB;
            f->porta    = sortearPorta(2);
        } else if (r < 36) {
            f->tipoExpr = EXPR_NOT;
        } else if (r < 50) {
            f->tipoExpr   = EXPR_A_FIXO;
            f->porta      = sortearPorta(2);
            f->valFixo[0] = (bool)GetRandomValue(0, 1);
        } else if (r < 64) {
            f->tipoExpr   = EXPR_B_FIXO;
            f->porta      = sortearPorta(2);
            f->valFixo[1] = (bool)GetRandomValue(0, 1);
        } else {
            f->tipoExpr   = EXPR_ESCOLHER_OP;
            f->valFixo[0] = (bool)GetRandomValue(0, 1);
            f->valFixo[1] = (bool)GetRandomValue(0, 1);
        }
    }

    configurarSlots(f);
}

static void aplicarTimerExpressao(FaseLogica* f, int fe) {
    float tBase = TIMER_BASE - (float)fe * 2.0f;
    if (f->eventoAtual == EVENTO_RELAMPAGO) tBase = 3.0f;
    if (tBase < TIMER_MIN) tBase = TIMER_MIN;
    f->temTimer     = (fe >= 1 || f->eventoAtual == EVENTO_RELAMPAGO);
    f->timerMaxExpr = tBase;
    f->timerExpr    = tBase;
}

static void gerarExpressao(FaseLogica* f, const Boss* boss) {
    int fe = calcFaseExpr(boss);
    f->faseExpr = fe;

    for (int tentativa = 0; tentativa < 20; tentativa++) {
        montarExpressao(f, fe);
        if (!mesmoDesafio(f)) break;
    }

    registrarDesafioAtual(f);
    aplicarTimerExpressao(f, fe);
    f->indiceRodada++;
}

/* ── resolução da expressão ──────────────────────────────────────────────────── */
static bool resolverExpressao(const FaseLogica* f) {
    switch (f->tipoExpr) {
        case EXPR_AB:
            return calcPorta(f->porta, f->slotValor[0], f->slotValor[1]);
        case EXPR_NOT:
            return !f->slotValor[0];
        case EXPR_A_FIXO:
            return calcPorta(f->porta, f->valFixo[0], f->slotValor[0]);
        case EXPR_B_FIXO:
            return calcPorta(f->porta, f->slotValor[0], f->valFixo[1]);
        case EXPR_ESCOLHER_OP:
            return calcPorta(f->slotOp, f->valFixo[0], f->valFixo[1]);
    }
    return false;
}

/* ── acerto / erro ───────────────────────────────────────────────────────────── */
static void acertarRodada(FaseLogica* f, Boss* boss) {
    f->combo++;
    if (f->combo > 20) f->combo = 20;
    f->nivelLaser = f->combo / 3;
    if (f->nivelLaser > 3) f->nivelLaser = 3;

    int dano = LOGIC_DANO_BASE + f->nivelLaser * 2;
    boss->hp -= dano;
    boss->tempoPiscandoDano = 0.35f;
    if (boss->hp <= 0) { boss->hp = 0; boss->ativa = false; }

    f->laserAtivo = true;
    f->timerLaser = LASER_DUR + (float)f->nivelLaser * 0.15f;
    f->timerComboTexto = 1.8f;

    if      (f->combo >= 8) { setFeedback(f, "MODO FOCUS! x8!", (Color){80,220,255,255}); setFlash(f, (Color){80,255,200,55}, 0.30f); }
    else if (f->combo >= 5) { setFeedback(f, "COMBO x5! MENTAL!", YELLOW);               setFlash(f, (Color){255,255,80,45}, 0.25f); }
    else if (f->combo >= 3) { setFeedback(f, "COMBO x3!", GREEN);                         setFlash(f, (Color){80,255,120,40}, 0.20f); }
    else                    { setFeedback(f, "ACERTO!", GREEN);                            setFlash(f, (Color){80,255,120,30}, 0.15f); }
}

static void errarRodada(FaseLogica* f, Player* jogador) {
    if (f->shieldAtivo) {
        f->shieldAtivo = false;
        setFeedback(f, "ESCUDO ABSORVEU!", GOLD);
        setFlash(f, (Color){255,215,0,60}, 0.25f);
    } else {
        aplicarDanoPlayer(jogador, 1);
        setFeedback(f, "ERRO! COMBO PERDIDO", RED);
        setFlash(f, (Color){255,50,50,80}, 0.40f);
        f->shakeTimer = 0.45f;
    }
    f->combo      = 0;
    f->nivelLaser = 0;
    f->curtoAtivo = true;
    f->timerCurto = CURTO_DUR;
}

static void resolverRodada(FaseLogica* f, Boss* boss, Player* jogador) {
    bool resultado = resolverExpressao(f);
    if (resultado == f->objetivo) acertarRodada(f, boss);
    else                          errarRodada(f, jogador);
    f->pausaRodada = true;
    f->timerPausa  = PAUSA_DUR;
    limparSlots(f);
}

/* ── spawn de drops ──────────────────────────────────────────────────────────── */
static TipoDrop sortearTipoDrop(const FaseLogica* f) {
    int roll = GetRandomValue(0, 99);

    /* 7 % chance de power-up */
    if (roll >= 93) {
        TipoDrop pws[] = { DROP_BOMB, DROP_FREEZE, DROP_AUTOCOMPLETE, DROP_SLOW, DROP_SHIELD };
        return pws[GetRandomValue(0, 4)];
    }

    if (f->eventoAtual == EVENTO_NOT_MODE)
        return (GetRandomValue(0,1) == 0) ? DROP_V : DROP_F;

    /* expressão precisa de operador */
    if (f->tipoExpr == EXPR_ESCOLHER_OP) {
        TipoDrop ops[] = { DROP_OP_AND, DROP_OP_OR, DROP_OP_XOR };
        return ops[GetRandomValue(0, 2)];
    }

    /* demais: valores V/F */
    return (GetRandomValue(0,1) == 0) ? DROP_V : DROP_F;
}

static void spawnarDrop(FaseLogica* f) {
    int idx = buscarDropLivre(f);
    if (idx < 0) return;
    f->drops[idx].tipo  = sortearTipoDrop(f);
    f->drops[idx].x     = (float)GetRandomValue((int)DROP_W, SCREEN_W - (int)DROP_W);
    f->drops[idx].y     = -DROP_H;
    f->drops[idx].ativo = true;
}

static void iniciarNovaRodada(FaseLogica* f, Boss* boss) {
    for (int i = 0; i < LOGIC_MAX_DROPS; i++) f->drops[i].ativo = false;
    limparSlots(f);
    gerarExpressao(f, boss);
    f->timerSpawn = 0.6f;
    spawnarDrop(f);
}

/* ── autocomplete inteligente ────────────────────────────────────────────────── */
static void autocompletarSlot(FaseLogica* f, int s, const Boss* boss) {
    if (f->slotPreenchido[s]) return;
    f->slotPreenchido[s] = true;

    if (f->slotEhValor[s]) {
        /* tenta encontrar valor que satisfaz objetivo */
        for (int v = 0; v <= 1; v++) {
            f->slotValor[s] = (bool)v;
            /* se for o único slot, verifica direto */
            if (f->slotsTotal == 1) {
                bool res = resolverExpressao(f);
                if (res == f->objetivo) return;
            } else {
                /* chuta o melhor valor possível */
                bool res = resolverExpressao(f);
                if (res == f->objetivo) return;
            }
        }
        f->slotValor[s] = (bool)GetRandomValue(0, 1);
    } else {
        f->slotOp = (PortaLogica)GetRandomValue(0, 2);
    }
    (void)boss;
}

/* ── coleta de drop ──────────────────────────────────────────────────────────── */
static void coletarDrop(FaseLogica* f, Boss* boss, Player* jogador,
                        BossBullet balasBoss[], int maxBoss, TipoDrop tipo) {
    switch (tipo) {
        case DROP_BOMB:
            for (int i = 0; i < maxBoss; i++) balasBoss[i].ativa = false;
            setFeedback(f, "BOMBA! TIROS LIMPOS!", GREEN);
            setFlash(f, (Color){255,255,0,60}, 0.35f);
            return;
        case DROP_FREEZE:
            f->timerFreeze = 4.0f;
            setFeedback(f, "DROPS CONGELADOS!", SKYBLUE);
            return;
        case DROP_AUTOCOMPLETE:
            for (int s = 0; s < f->slotsTotal; s++) {
                if (!f->slotPreenchido[s]) {
                    autocompletarSlot(f, s, boss);
                    setFeedback(f, "AUTO-COMPLETE!", YELLOW);
                    break;
                }
            }
            /* verifica resolução */
            {
                bool todos = true;
                for (int s = 0; s < f->slotsTotal; s++)
                    if (!f->slotPreenchido[s]) { todos = false; break; }
                if (todos && !f->pausaRodada) resolverRodada(f, boss, jogador);
            }
            return;
        case DROP_SLOW:
            f->timerSlow = 6.0f;
            setFeedback(f, "SLOW MOTION!", PURPLE);
            return;
        case DROP_SHIELD:
            f->shieldAtivo = true;
            f->timerShield = 10.0f;
            setFeedback(f, "ESCUDO ATIVO!", GOLD);
            return;
        default: break;
    }

    bool ehValor = (tipo == DROP_V || tipo == DROP_F);
    bool ehOp    = (tipo == DROP_OP_AND || tipo == DROP_OP_OR || tipo == DROP_OP_XOR);

    for (int s = 0; s < f->slotsTotal; s++) {
        if (f->slotPreenchido[s]) continue;
        if ( f->slotEhValor[s] && !ehValor) continue;
        if (!f->slotEhValor[s] && !ehOp)   continue;

        f->slotPreenchido[s] = true;
        if (ehValor) {
            f->slotValor[s] = (tipo == DROP_V);
        } else {
            if (tipo == DROP_OP_AND) f->slotOp = PORTA_AND;
            if (tipo == DROP_OP_OR)  f->slotOp = PORTA_OR;
            if (tipo == DROP_OP_XOR) f->slotOp = PORTA_XOR;
        }
        break;
    }

    bool todos = true;
    for (int s = 0; s < f->slotsTotal; s++)
        if (!f->slotPreenchido[s]) { todos = false; break; }
    if (todos && !f->pausaRodada) resolverRodada(f, boss, jogador);
}

/* ── eventos especiais ───────────────────────────────────────────────────────── */
static void dispararEvento(FaseLogica* f, BossBullet balasBoss[], int maxBoss) {
    int r = GetRandomValue(0, 3);
    const TipoEvento tab[] = { EVENTO_CHUVA, EVENTO_RELAMPAGO, EVENTO_NOT_MODE, EVENTO_FREEZE_TIROS };
    f->eventoAtual = tab[r];

    switch (f->eventoAtual) {
        case EVENTO_CHUVA:
            f->timerEvento    = 8.0f;
            f->intervaloSpawn = 0.5f;
            setFeedback(f, "CHUVA DE DROPS!", SKYBLUE);
            break;
        case EVENTO_RELAMPAGO:
            f->timerEvento = 15.0f;
            setFeedback(f, "RELAMPAGO! TEMPO CURTO!", RED);
            setFlash(f, (Color){255,100,0,60}, 0.5f);
            break;
        case EVENTO_NOT_MODE:
            f->timerEvento = 12.0f;
            setFeedback(f, "MODO NOT ATIVADO!", PURPLE);
            break;
        case EVENTO_FREEZE_TIROS:
            f->timerEvento = 6.0f;
            for (int i = 0; i < maxBoss; i++) balasBoss[i].ativa = false;
            setFeedback(f, "TIROS CONGELADOS!", SKYBLUE);
            setFlash(f, (Color){100,200,255,50}, 0.5f);
            break;
        default: break;
    }
    f->timerProxEvento = (float)GetRandomValue(30, 60);
}

/* ── inicialização ───────────────────────────────────────────────────────────── */
void inicializarFaseLogica(FaseLogica* f, Boss* boss, Player* jogador) {
    (void)jogador;
    SetRandomSeed((unsigned int)time(NULL) ^ (unsigned int)GetTime());

    f->ultimoTipo     = EXPR_AB;
    f->ultimoPorta    = PORTA_AND;
    f->ultimoObjetivo = false;
    f->ultimoVal0     = false;
    f->ultimoVal1     = false;
    f->indiceRodada   = 0;

    limparSlots(f);

    for (int i = 0; i < LOGIC_MAX_DROPS; i++) f->drops[i].ativo = false;

    f->timerSpawn   = 1.0f;
    f->intervaloSpawn = SPAWN_BASE;
    f->timerLaser   = 0.0f;
    f->timerCurto   = 0.0f;
    f->timerPausa   = 0.0f;
    f->laserAtivo   = false;
    f->curtoAtivo   = false;
    f->pausaRodada  = false;
    f->ativa        = true;

    f->combo           = 0;
    f->timerComboTexto = 0.0f;
    f->nivelLaser      = 0;

    f->timerSlow    = 0.0f;
    f->timerShield  = 0.0f;
    f->timerFreeze  = 0.0f;
    f->shieldAtivo  = false;

    f->flashTimer       = 0.0f;
    f->shakeTimer       = 0.0f;
    f->shakeX = f->shakeY = 0.0f;
    f->textoFeedback[0] = '\0';
    f->timerFeedback    = 0.0f;

    f->eventoAtual    = EVENTO_NENHUM;
    f->timerEvento    = 0.0f;
    f->timerProxEvento = (float)GetRandomValue(30, 50);

    f->faseExpr = calcFaseExpr(boss);
    iniciarNovaRodada(f, boss);
    spawnarDrop(f);
}

/* ── atualização principal ───────────────────────────────────────────────────── */
void atualizarFaseLogica(FaseLogica* f, Boss* boss, Player* jogador,
                         BossBullet balasBoss[], int maxBoss, float deltaTime) {
    if (!f->ativa || !boss->ativa || jogador->hp <= 0) {
        f->ativa = false;
        return;
    }

    float dt = f->timerSlow > 0.0f ? deltaTime * 0.5f : deltaTime;

    /* timers de feedback */
    if (f->flashTimer     > 0.0f) f->flashTimer     -= deltaTime;
    if (f->timerFeedback  > 0.0f) f->timerFeedback  -= deltaTime;
    if (f->timerComboTexto > 0.0f) f->timerComboTexto -= deltaTime;
    if (f->shakeTimer     > 0.0f) {
        f->shakeTimer -= deltaTime;
        f->shakeX = (f->shakeTimer > 0.0f) ? (float)GetRandomValue(-4,4) : 0.0f;
        f->shakeY = (f->shakeTimer > 0.0f) ? (float)GetRandomValue(-3,3) : 0.0f;
    }

    /* power-up timers */
    if (f->timerSlow   > 0.0f) f->timerSlow   -= deltaTime;
    if (f->timerShield > 0.0f) { f->timerShield -= deltaTime; if (f->timerShield <= 0.0f) f->shieldAtivo = false; }
    if (f->timerFreeze > 0.0f) f->timerFreeze -= deltaTime;

    /* timers de estado */
    if (f->timerLaser > 0.0f) { f->timerLaser -= deltaTime; if (f->timerLaser <= 0.0f) f->laserAtivo = false; }
    if (f->timerCurto > 0.0f) { f->timerCurto -= deltaTime; if (f->timerCurto <= 0.0f) f->curtoAtivo = false; }

    /* pausa entre rodadas */
    if (f->pausaRodada) {
        f->timerPausa -= deltaTime;
        if (f->timerPausa <= 0.0f) {
            f->pausaRodada = false;
            f->timerPausa  = PAUSA_DUR;
            iniciarNovaRodada(f, boss);
        }
        return;
    }

    /* timer de pressão */
    if (f->temTimer) {
        f->timerExpr -= dt;
        if (f->timerExpr <= 0.0f) {
            errarRodada(f, jogador);
            f->pausaRodada = true;
            f->timerPausa  = PAUSA_DUR;
            limparSlots(f);
            f->timerExpr = 0.0f;
        }
    }

    /* eventos especiais */
    f->timerProxEvento -= deltaTime;
    if (f->timerProxEvento <= 0.0f && f->eventoAtual == EVENTO_NENHUM)
        dispararEvento(f, balasBoss, maxBoss);

    if (f->eventoAtual != EVENTO_NENHUM) {
        f->timerEvento -= deltaTime;
        if (f->timerEvento <= 0.0f) {
            f->eventoAtual    = EVENTO_NENHUM;
            f->intervaloSpawn = SPAWN_BASE;
        }
    }

    /* spawn de drops */
    if (f->timerFreeze <= 0.0f) {
        f->timerSpawn -= deltaTime;
        if (f->timerSpawn <= 0.0f) {
            spawnarDrop(f);
            float pressao     = 1.0f - pctVida(boss);
            f->intervaloSpawn = (f->eventoAtual == EVENTO_CHUVA) ? 0.5f
                                : (SPAWN_BASE - pressao * 0.8f);
            if (f->intervaloSpawn < SPAWN_MIN) f->intervaloSpawn = SPAWN_MIN;
            f->timerSpawn = f->intervaloSpawn;
        }
    }

    /* mover e coletar drops */
    float velDrop = (DROP_VEL + (1.0f - pctVida(boss)) * 45.0f) * dt;
    if (f->timerFreeze > 0.0f) velDrop = 0.0f;

    for (int i = 0; i < LOGIC_MAX_DROPS; i++) {
        if (!f->drops[i].ativo) continue;
        f->drops[i].y += velDrop;
        if (f->drops[i].y > SCREEN_H + DROP_H) { f->drops[i].ativo = false; continue; }

        float dx   = f->drops[i].x - jogador->posicaoX;
        float dy   = f->drops[i].y - jogador->posicaoY;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < PLAYER_RADIUS + DROP_W * 0.55f) {
            TipoDrop tipo = f->drops[i].tipo;
            f->drops[i].ativo = false;
            coletarDrop(f, boss, jogador, balasBoss, maxBoss, tipo);
        }
    }

    if (!boss->ativa || jogador->hp <= 0) f->ativa = false;
}

/* ── renderização ────────────────────────────────────────────────────────────── */

static const char* portaTxt(PortaLogica p) {
    switch (p) { case PORTA_AND: return "AND"; case PORTA_OR: return "OR"; case PORTA_XOR: return "XOR"; }
    return "?";
}

static void drawHudExpressao(const FaseLogica* f) {
    char hud[96];
    const char* obj = f->objetivo ? "V" : "F";
    Color corObj    = f->objetivo ? GREEN : RED;

    switch (f->tipoExpr) {
        case EXPR_AB:
            snprintf(hud, sizeof(hud), "[ %s ] %s [ %s ]  =  %s",
                f->slotPreenchido[0] ? (f->slotValor[0] ? "V" : "F") : "?",
                portaTxt(f->porta),
                f->slotPreenchido[1] ? (f->slotValor[1] ? "V" : "F") : "?",
                obj);
            break;
        case EXPR_NOT:
            snprintf(hud, sizeof(hud), "NOT [ %s ]  =  %s",
                f->slotPreenchido[0] ? (f->slotValor[0] ? "V" : "F") : "?",
                obj);
            break;
        case EXPR_A_FIXO:
            snprintf(hud, sizeof(hud), "%s %s [ %s ]  =  %s",
                f->valFixo[0] ? "V" : "F",
                portaTxt(f->porta),
                f->slotPreenchido[0] ? (f->slotValor[0] ? "V" : "F") : "?",
                obj);
            break;
        case EXPR_B_FIXO:
            snprintf(hud, sizeof(hud), "[ %s ] %s %s  =  %s",
                f->slotPreenchido[0] ? (f->slotValor[0] ? "V" : "F") : "?",
                portaTxt(f->porta),
                f->valFixo[1] ? "V" : "F",
                obj);
            break;
        case EXPR_ESCOLHER_OP: {
            const char* opColetado = f->slotPreenchido[0] ? portaTxt(f->slotOp) : "?";
            snprintf(hud, sizeof(hud), "%s  [ %s ]  %s  =  %s",
                f->valFixo[0] ? "V" : "F",
                opColetado,
                f->valFixo[1] ? "V" : "F",
                obj);
            break;
        }
    }

    int fonteHud = 28;
    DrawRectangle(0, 0, SCREEN_W, 60, (Color){8, 8, 22, 225});

    /* pisca quando pressão temporal está baixa */
    bool mostrar = true;
    if (f->temTimer && f->timerExpr < 3.0f)
        mostrar = ((int)(f->timerExpr * 6.0f) % 2 == 0);

    if (mostrar) {
        int w = MeasureText(hud, fonteHud);
        DrawText(hud, (SCREEN_W - w) / 2, 16, fonteHud, corObj);
    }

    DrawLine(0, 60, SCREEN_W, 60, (Color){100,100,180,255});

    /* barra de timer */
    if (f->temTimer && f->timerMaxExpr > 0.0f) {
        float pct = f->timerExpr / f->timerMaxExpr;
        if (pct < 0.0f) pct = 0.0f;
        Color barCor = (pct > 0.5f) ? GREEN : (pct > 0.25f) ? ORANGE : RED;
        DrawRectangle(0, 58, (int)(SCREEN_W * pct), 3, barCor);
    }
}

static void drawCombo(const FaseLogica* f) {
    if (f->combo < 2 || f->timerComboTexto <= 0.0f) return;
    char txt[24];
    snprintf(txt, sizeof(txt), "x%d COMBO", f->combo);
    Color c = (f->combo >= 8) ? (Color){80,220,255,255}
            : (f->combo >= 5) ? YELLOW
            : GREEN;
    DrawText(txt, SCREEN_W - MeasureText(txt, 22) - 16, 70, 22, c);
}

static void drawShieldIndicator(const FaseLogica* f, const Player* jogador) {
    if (!f->shieldAtivo) return;
    DrawCircleLines((int)jogador->posicaoX, (int)jogador->posicaoY, PLAYER_RADIUS + 8, GOLD);
    DrawCircleLines((int)jogador->posicaoX, (int)jogador->posicaoY, PLAYER_RADIUS + 10, (Color){255,215,0,120});
}

static void drawSlotNave(float x, float y, bool preenchido, bool ehValor, bool valor, PortaLogica op) {
    Rectangle slot = { x, y, 36, 22 };
    DrawRectangleRec(slot, (Color){18, 18, 32, 255});
    DrawRectangleLinesEx(slot, 2.0f, RAYWHITE);

    if (!preenchido) {
        DrawText("?", (int)x + 14, (int)y + 3, 17, GRAY);
        return;
    }
    if (ehValor) {
        Color c = valor ? BLUE : RED;
        DrawRectangleRec(slot, c);
        DrawText(valor ? "V" : "F", (int)x + 13, (int)y + 3, 17, RAYWHITE);
    } else {
        DrawRectangleRec(slot, (Color){40,120,40,255});
        DrawText(portaTxt(op), (int)x + 4, (int)y + 3, 13, RAYWHITE);
    }
}

static Color corDrop(TipoDrop t) {
    switch (t) {
        case DROP_V:            return BLUE;
        case DROP_F:            return RED;
        case DROP_OP_AND:       return (Color){20,140,20,255};
        case DROP_OP_OR:        return (Color){60,200,60,255};
        case DROP_OP_XOR:       return ORANGE;
        case DROP_BOMB:         return GOLD;
        case DROP_FREEZE:       return SKYBLUE;
        case DROP_AUTOCOMPLETE: return (Color){0, 180, 210, 255};
        case DROP_SLOW:         return PURPLE;
        case DROP_SHIELD:       return (Color){220,180,20,255};
    }
    return (Color){80, 80, 80, 255};
}

static Color corTextoDrop(TipoDrop t) {
    switch (t) {
        case DROP_BOMB:
        case DROP_FREEZE:
        case DROP_AUTOCOMPLETE:
            return BLACK;
        default:
            return RAYWHITE;
    }
}

static const char* labelDrop(TipoDrop t) {
    switch (t) {
        case DROP_V:            return "V";
        case DROP_F:            return "F";
        case DROP_OP_AND:       return "AND";
        case DROP_OP_OR:        return "OR";
        case DROP_OP_XOR:       return "XOR";
        case DROP_BOMB:         return "BOM";
        case DROP_FREEZE:       return "FRZ";
        case DROP_AUTOCOMPLETE: return "AUTO";
        case DROP_SLOW:         return "SLO";
        case DROP_SHIELD:       return "SCU";
    }
    return "?";
}

static void drawDropItem(float cx, float cy, TipoDrop tipo) {
    Rectangle r = { cx - DROP_W / 2.0f, cy - DROP_H / 2.0f, DROP_W, DROP_H };
    const char* lbl = labelDrop(tipo);
    int fs = (strlen(lbl) <= 1) ? 22 : 13;
    Color fundo = corDrop(tipo);
    Color texto = corTextoDrop(tipo);

    DrawRectangleRec(r, fundo);
    DrawRectangleLinesEx(r, 2.0f, RAYWHITE);
    DrawText(lbl,
             (int)(r.x + (r.width  - MeasureText(lbl, fs)) / 2.0f),
             (int)(r.y + (r.height - fs) / 2.0f),
             fs, texto);
}

static void drawDrops(const FaseLogica* f) {
    for (int i = 0; i < LOGIC_MAX_DROPS; i++) {
        if (!f->drops[i].ativo) continue;
        drawDropItem(f->drops[i].x, f->drops[i].y, f->drops[i].tipo);
    }
}

static void drawLaser(const FaseLogica* f, const Boss* boss, const Player* jogador) {
    if (!f->laserAtivo) return;
    float grossura = 4.0f + (float)f->nivelLaser * 2.5f;
    Color c = (f->nivelLaser >= 3) ? (Color){255,220,80,255}
            : (f->nivelLaser >= 2) ? (Color){80,255,220,255}
            :                        (Color){80,180,255,255};
    DrawLineEx(
        (Vector2){jogador->posicaoX, jogador->posicaoY - PLAYER_RADIUS},
        (Vector2){boss->posicaoX, boss->posicaoY},
        grossura, c);
    DrawCircle((int)boss->posicaoX, (int)boss->posicaoY, 10 + f->nivelLaser * 4, (Color){c.r, c.g, c.b, 160});
}

static void drawFeedbackTexto(const FaseLogica* f) {
    if (f->timerFeedback <= 0.0f) return;
    float alpha = (f->timerFeedback > 0.5f) ? 1.0f : f->timerFeedback / 0.5f;
    Color c = { f->corFeedback.r, f->corFeedback.g, f->corFeedback.b, (unsigned char)(240 * alpha) };
    DrawText(f->textoFeedback,
             (SCREEN_W - MeasureText(f->textoFeedback, 26)) / 2,
             SCREEN_H / 2 - 20, 26, c);
}

static void drawEventoBanner(const FaseLogica* f) {
    if (f->eventoAtual == EVENTO_NENHUM) return;
    const char* labels[] = { "", "CHUVA!", "RELAMPAGO!", "MODO NOT!", "FREEZE TIROS!" };
    const char* lbl = labels[f->eventoAtual];
    Color c = (f->eventoAtual == EVENTO_NOT_MODE)   ? PURPLE
            : (f->eventoAtual == EVENTO_RELAMPAGO)   ? ORANGE
            : SKYBLUE;
    DrawText(lbl, 10, 64, 18, c);
}

static void drawSlowIndicator(const FaseLogica* f) {
    if (f->timerSlow <= 0.0f) return;
    DrawText(TextFormat("SLOW %.0fs", f->timerSlow), 10, 86, 16, PURPLE);
}

void desenharFaseLogica(const FaseLogica* f, const Boss* boss, const Player* jogador) {
    int ox = (int)f->shakeX;
    int oy = (int)f->shakeY;

    if (ox || oy) BeginScissorMode(0, 0, SCREEN_W, SCREEN_H);

    for (int i = 0; i < LOGIC_MAX_DROPS; i++) {
        if (!f->drops[i].ativo) continue;
        drawDropItem(f->drops[i].x + ox, f->drops[i].y + oy, f->drops[i].tipo);
    }

    drawLaser(f, boss, jogador);
    drawShieldIndicator(f, jogador);

    {
        float nx = jogador->posicaoX + ox;
        float ny = jogador->posicaoY + oy;
        float slotY = ny + PLAYER_RADIUS + 4.0f;
        if (f->slotsTotal == 1) {
            drawSlotNave(nx - 18.0f, slotY,
                         f->slotPreenchido[0], f->slotEhValor[0], f->slotValor[0], f->slotOp);
        } else {
            drawSlotNave(nx - 40.0f, slotY,
                         f->slotPreenchido[0], f->slotEhValor[0], f->slotValor[0], f->slotOp);
            drawSlotNave(nx + 4.0f, slotY,
                         f->slotPreenchido[1], f->slotEhValor[1], f->slotValor[1], f->slotOp);
        }
    }

    if (ox || oy) EndScissorMode();

    /* HUD fixo */
    drawHudExpressao(f);
    drawCombo(f);
    drawFeedbackTexto(f);
    drawEventoBanner(f);
    drawSlowIndicator(f);

    /* flash de tela */
    if (f->flashTimer > 0.0f) {
        float a = f->flashTimer / 0.4f;
        if (a > 1.0f) a = 1.0f;
        Color fc = { f->flashCor.r, f->flashCor.g, f->flashCor.b,
                     (unsigned char)(f->flashCor.a * a) };
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, fc);
    }
}
