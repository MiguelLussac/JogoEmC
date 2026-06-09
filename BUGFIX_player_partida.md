# Bugfix: player + partida

**Branch:** `fix/player-partida-bugs`  
**Data:** 2026-06-08  
**Arquivos alterados:** `src/player/player.h`, `src/player/player.c`, `src/game/partida.h`, `src/game/partida.c`, `src/main.c`

---

## Bug #1 — `PLAYER_RADIUS` definido duas vezes

**Severidade:** Baixa (warning de compilação, comportamento silenciosamente inconsistente)

**Causa:** O valor `#define PLAYER_RADIUS 20` estava declarado tanto em `player.h` quanto redeclarado na linha 9 de `player.c`. Qualquer alteração em um dos dois arquivos sem atualizar o outro criaria uma divergência silenciosa.

**Correção:** Removida a redefinição de `player.c`. O `#define` em `player.h` é a única fonte de verdade.

```diff
// player.c — linha removida
-#define PLAYER_RADIUS 20
```

---

## Bug #2 — Velocidade da bala (`500.0f`) hardcoded em dois lugares

**Severidade:** Média (risco de divergência entre a inicialização e o disparo)

**Causa:** O valor `500.0f` aparecia literalmente em dois pontos distintos:
- `partida.c` — ao inicializar o pool de balas
- `player.c` — ao ativar uma bala no momento do tiro

Qualquer ajuste de balanceamento exigiria lembrar de alterar ambos.

**Correção:** Adicionada constante `PLAYER_BULLET_SPEED` em `player.h` e substituído em todos os pontos de uso.

```diff
// player.h
+#define PLAYER_BULLET_SPEED  500.0f

// partida.c
-balasJogador[i].velocidade = 500.0f;
+balasJogador[i].velocidade = PLAYER_BULLET_SPEED;

// player.c
-bullets[i].velocidade = 500.0f;
+bullets[i].velocidade = PLAYER_BULLET_SPEED;
```

---

## Bug #3 — `moverBalas` só checava o limite do topo

**Severidade:** Média (balas "fantasmas" em memória se o padrão de tiro mudar futuramente)

**Causa:** A função `moverBalas` desativava uma bala apenas quando `posicaoY < 0` (saída pelo topo). Com balas sempre retas para cima, não era um problema imediato — mas qualquer mudança futura de ângulo (tiro duplo, spread) deixaria balas ativas fora da tela consumindo slots do pool.

**Correção:** A condição agora verifica saída por qualquer borda.

```diff
// player.c
-if (bullets[i].posicaoY < 0) {
+if (bullets[i].posicaoY < 0 ||
+    bullets[i].posicaoY > limiteY ||
+    bullets[i].posicaoX < 0 ||
+    bullets[i].posicaoX > limiteX) {
     bullets[i].ativa = false;
 }
```

---

## Bug #4 — Pool de balas muito pequeno (`MAX_BULLETS 10`)

**Severidade:** Média (gargalo de gameplay com cooldowns menores)

**Causa:** Com `PLAYER_FIRE_COOLDOWN = 0.32f` e balas a `500 px/s` em uma tela de 720px, uma bala tem vida de ~1.44s — o que permite até ~4 balas simultâneas. O pool de 10 slots parecia suficiente, mas qualquer redução de cooldown (boost futuro) ou adição de tiro duplo/triplo esgotaria o pool, impedindo novos disparos silenciosamente.

**Correção:** Pool aumentado para 20 slots.

```diff
// partida.h
-#define MAX_BULLETS 10
+#define MAX_BULLETS 20
```

---

## Bug #5 — `inicializarPartida` com 10 parâmetros

**Severidade:** Alta (manutenibilidade e escalabilidade comprometidas)

**Causa:** A função recebia 10 ponteiros individuais. Adicionar qualquer nova entidade ao estado do jogo exigiria alterar a assinatura em `partida.h`, `partida.c` e em todos os pontos de chamada em `main.c`. O risco de errar a ordem dos argumentos era alto.

**Correção:** Criado o struct `ContextoPartida` em `partida.h` que agrega todos os ponteiros de estado. A função passa a receber apenas `ContextoPartida*`.

```diff
// partida.h — novo struct
+typedef struct {
+    Player*              jogador;
+    Bullet*              balasJogador;
+    Boss*                boss;
+    BossBullet*          balasBoss;
+    Estrela*             estrela;
+    DesafioPergunta*     desafio;
+    bool*                perguntaAtiva;
+    bool*                jogoEncerrado;
+    MotivoFimJogo*       motivoFimJogo;
+    EstatisticasPartida* stats;
+} ContextoPartida;

// partida.h — nova assinatura
-void inicializarPartida(Player* jogador, Bullet balasJogador[], Boss* boss, BossBullet balasBoss[],
-                        Estrela* estrela, DesafioPergunta* desafio, bool* perguntaAtiva,
-                        bool* jogoEncerrado, MotivoFimJogo* motivoFimJogo, EstatisticasPartida* stats);
+void inicializarPartida(ContextoPartida* ctx);

// main.c — chamada atualizada
+ContextoPartida ctx = { .jogador = &jogador, .balasJogador = bala, ... };
+inicializarPartida(&ctx);
```

---

## Resultado

| # | Bug | Severidade | Status |
|---|---|---|---|
| 1 | `PLAYER_RADIUS` duplicado | Baixa | ✅ Corrigido |
| 2 | `PLAYER_BULLET_SPEED` hardcoded | Média | ✅ Corrigido |
| 3 | `moverBalas` sem checagem completa de bordas | Média | ✅ Corrigido |
| 4 | Pool de balas pequeno (`MAX_BULLETS 10`) | Média | ✅ Corrigido |
| 5 | `inicializarPartida` com 10 parâmetros | Alta | ✅ Corrigido |

Build final: **0 erros, 0 warnings**.
