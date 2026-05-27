#include "logic_phase.h"
#include <math.h>
#include <string.h>

#define LOGIC_DROP_COUNT 10
#define LOGIC_MINION_COUNT 4

static Camera2D camera;
static LogicPlayer player;
static LogicBoss boss;
static LogicDrop drops[LOGIC_DROP_COUNT];
static LogicMinion minions[LOGIC_MINION_COUNT];

static float tempoDeJogo = 0.0f;
static int portasResolvidas = 0;
static bool jogadorVenceu = false;
static bool phaseComplete = false;
static float shakeTimer = 0.0f;
static float bossHitFlash = 0.0f;
static float playerHitFlash = 0.0f;
static float bossAuraPulse = 0.0f;
static float playerGlowPulse = 0.0f;
static float backgroundPulse = 0.0f;
static float bossBaseY = 160.0f;
static float tutorialAlpha = 0.0f;
static bool tutorialTransition = false;
static float tutorialTransitionProgress = 0.0f;
static float minionSpawnTimer = 0.0f;

static bool EvaluateLogicGate(LogicPhaseGate gate, bool a, bool b) {
    return gate == GATE_AND ? (a && b) : (a || b);
}

static void ResetLogicRound(void) {
    player.slot1_filled = false;
    player.slot2_filled = false;
    player.slot1_val = false;
    player.slot2_val = false;
    player.current_gate = GetRandomValue(0, 1) == 0 ? GATE_AND : GATE_OR;
    player.expected_result = GetRandomValue(0, 1) == 1;
}

static void SpawnMinion(int screenWidth) {
    for (int i = 0; i < LOGIC_MINION_COUNT; i++) {
        if (!minions[i].active) {
            minions[i].active = true;
            minions[i].speed = (float)GetRandomValue(120, 200) / 10.0f;
            minions[i].rect.width = 28;
            minions[i].rect.height = 28;
            minions[i].rect.x = (float)GetRandomValue(40, screenWidth - 40 - (int)minions[i].rect.width);
            minions[i].rect.y = -minions[i].rect.height;
            return;
        }
    }
}

void InitLogicPhase(void) {
    memset(drops, 0, sizeof(drops));
    memset(minions, 0, sizeof(minions));
    tempoDeJogo = 0.0f;
    portasResolvidas = 0;
    jogadorVenceu = false;
    phaseComplete = false;
    shakeTimer = 0.0f;
    bossHitFlash = 0.0f;
    playerHitFlash = 0.0f;
    bossAuraPulse = 0.0f;
    playerGlowPulse = 0.0f;
    backgroundPulse = 0.0f;
    minionSpawnTimer = 0.6f;
    tutorialAlpha = 0.0f;
    tutorialTransition = false;
    tutorialTransitionProgress = 0.0f;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    player.rect = (Rectangle){screenWidth * 0.5f - 28.0f, screenHeight - 170.0f, 56.0f, 56.0f};
    player.hp = 5;
    player.animatedHp = 5.0f;
    player.current_gate = GATE_AND;
    player.expected_result = true;
    player.slot1_filled = false;
    player.slot2_filled = false;
    player.slot1_val = false;
    player.slot2_val = false;

    boss.rect = (Rectangle){screenWidth * 0.5f - 84.0f, bossBaseY, 168.0f, 92.0f};
    boss.hp = 10;
    boss.max_hp = 10;
    boss.animatedHp = 10.0f;

    camera = (Camera2D){0};
    camera.target = (Vector2){screenWidth * 0.5f, screenHeight * 0.5f};
    camera.offset = (Vector2){screenWidth * 0.5f, screenHeight * 0.5f};
    camera.zoom = 1.0f;

    ResetLogicRound();
}

void UpdateLogicPhase(GameState* currentState) {
    float deltaTime = GetFrameTime();
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float playerSpeed = 380.0f;

    float diffFactor = 1.0f - ((float)boss.hp / (float)boss.max_hp);
    tempoDeJogo += deltaTime;

    shakeTimer -= deltaTime;
    bossHitFlash -= deltaTime;
    playerHitFlash -= deltaTime;
    bossAuraPulse += deltaTime * 3.0f;
    playerGlowPulse += deltaTime * 4.0f;
    backgroundPulse += deltaTime * 1.4f;

    if (shakeTimer < 0.0f) shakeTimer = 0.0f;
    if (bossHitFlash < 0.0f) bossHitFlash = 0.0f;
    if (playerHitFlash < 0.0f) playerHitFlash = 0.0f;

    if (shakeTimer > 0.0f) {
        camera.target = (Vector2){
            (float)screenWidth * 0.5f + (float)GetRandomValue(-5, 5),
            (float)screenHeight * 0.5f + (float)GetRandomValue(-5, 5)
        };
    } else {
        camera.target = (Vector2){(float)screenWidth * 0.5f, (float)screenHeight * 0.5f};
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player.rect.x -= playerSpeed * deltaTime;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player.rect.x += playerSpeed * deltaTime;
    }

    if (player.rect.x < 0.0f) player.rect.x = 0.0f;
    if (player.rect.x + player.rect.width > screenWidth) player.rect.x = screenWidth - player.rect.width;

    boss.rect.x = (screenWidth * 0.5f - boss.rect.width * 0.5f) + sinf(GetTime() * (2.0f + diffFactor * 2.0f)) * (screenWidth * 0.5f - 120.0f);
    boss.rect.y = bossBaseY + cosf(GetTime() * (2.5f + diffFactor * 0.8f)) * 10.0f;
    if (boss.rect.x < 20.0f) boss.rect.x = 20.0f;
    if (boss.rect.x + boss.rect.width > screenWidth - 20.0f) boss.rect.x = screenWidth - boss.rect.width - 20.0f;
    if (boss.rect.y < 130.0f) boss.rect.y = 130.0f;

    minionSpawnTimer -= deltaTime;
    if (minionSpawnTimer <= 0.0f) {
        SpawnMinion(screenWidth);
        minionSpawnTimer = 0.9f - diffFactor * 0.35f + GetRandomValue(0, 40) / 100.0f;
    }

    for (int i = 0; i < LOGIC_DROP_COUNT; i++) {
        if (!drops[i].active) {
            if (GetRandomValue(0, 1000) < 14) {
                drops[i].rect = (Rectangle){(float)GetRandomValue(50, screenWidth - 100), -40.0f, 34.0f, 34.0f};
                drops[i].value_is_true = GetRandomValue(0, 1) == 1;
                drops[i].speed = (float)GetRandomValue(20, 35) / 10.0f + diffFactor * 3.5f;
                drops[i].swayOffset = (float)GetRandomValue(0, 100);
                drops[i].swaySpeed = (float)GetRandomValue(20, 50) / 10.0f + diffFactor * 1.8f;
                drops[i].active = true;
            }
        } else {
            drops[i].rect.y += drops[i].speed * deltaTime * 60.0f;
            drops[i].rect.x += sinf(GetTime() * drops[i].swaySpeed + drops[i].swayOffset) * 2.0f;
            if (drops[i].rect.x < 0.0f) drops[i].rect.x = 0.0f;
            if (drops[i].rect.x + drops[i].rect.width > screenWidth) drops[i].rect.x = screenWidth - drops[i].rect.width;

            if (drops[i].rect.y > screenHeight) {
                drops[i].active = false;
            } else if (CheckCollisionRecs(player.rect, drops[i].rect)) {
                drops[i].active = false;
                if (!player.slot1_filled) {
                    player.slot1_filled = true;
                    player.slot1_val = drops[i].value_is_true;
                } else if (!player.slot2_filled) {
                    player.slot2_filled = true;
                    player.slot2_val = drops[i].value_is_true;
                }
            }
        }
    }

    for (int i = 0; i < LOGIC_MINION_COUNT; i++) {
        if (!minions[i].active) continue;
        minions[i].rect.y += minions[i].speed * deltaTime;
        if (minions[i].rect.y > screenHeight + 50.0f) {
            minions[i].active = false;
            continue;
        }
        if (CheckCollisionRecs(player.rect, minions[i].rect)) {
            minions[i].active = false;
            player.hp -= 1;
            if (player.hp < 0) player.hp = 0;
            shakeTimer = 0.4f;
            playerHitFlash = 0.2f;
            if (player.hp <= 0) {
                phaseComplete = true;
                jogadorVenceu = false;
            }
        }
    }

    if (player.slot1_filled && player.slot2_filled && !phaseComplete) {
        bool result = EvaluateLogicGate(player.current_gate, player.slot1_val, player.slot2_val);
        if (result == player.expected_result) {
            boss.hp -= 1;
            if (boss.hp < 0) boss.hp = 0;
            portasResolvidas += 1;
            shakeTimer = 0.2f;
            bossHitFlash = 0.2f;
            if (boss.hp <= 0) {
                phaseComplete = true;
                jogadorVenceu = true;
            }
        } else {
            player.hp -= 1;
            if (player.hp < 0) player.hp = 0;
            shakeTimer = 0.4f;
            playerHitFlash = 0.2f;
            if (player.hp <= 0) {
                phaseComplete = true;
                jogadorVenceu = false;
            }
        }
        ResetLogicRound();
    }

    if (phaseComplete) {
        *currentState = STATE_LOGIC_REPORT;
    }

    boss.animatedHp += (boss.hp - boss.animatedHp) * 0.1f;
    player.animatedHp += (player.hp - player.animatedHp) * 0.1f;
}

void UpdateLogicTutorial(GameState* currentState) {
    float deltaTime = GetFrameTime();
    if (tutorialAlpha < 1.0f) tutorialAlpha += deltaTime * 2.0f;
    if (tutorialAlpha > 1.0f) tutorialAlpha = 1.0f;

    if (IsKeyPressed(KEY_ENTER)) {
        InitLogicPhase();
        *currentState = STATE_LOGIC_PHASE;
        tutorialAlpha = 0.0f;
        tutorialTransition = false;
        tutorialTransitionProgress = 0.0f;
    }
}

void DrawLogicTutorial(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float panelWidth = screenWidth * 0.72f;
    float panelHeight = screenHeight * 0.58f;
    float panelX = (screenWidth - panelWidth) / 2.0f;
    float panelY = (screenHeight - panelHeight) / 2.0f;

    ClearBackground(DARKGRAY);
    DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.14f, 14, Fade(RAYWHITE, 0.16f * tutorialAlpha));
    DrawRectangleRoundedLines((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.14f, 14, Fade(RAYWHITE, 0.32f * tutorialAlpha));

    const char* title = "COMO JOGAR A FASE LÓGICA";
    int titleSize = 34;
    float titleX = (screenWidth - MeasureText(title, titleSize)) / 2.0f;
    DrawText(title, (int)titleX, (int)(panelY + 28.0f), titleSize, Fade(YELLOW, tutorialAlpha));

    const char* lines[3] = {
        "1. Colete os blocos [ V ] e [ F ] para preencher a sua nave.",
        "2. Cumpra o OBJETIVO da PORTA LÓGICA pedida no topo da tela.",
        "3. Acertar dá dano no Boss. Errar tira a sua vida."
    };

    for (int i = 0; i < 3; i++) {
        float lineX = (screenWidth - MeasureText(lines[i], 24)) / 2.0f;
        DrawText(lines[i], (int)lineX, (int)(panelY + 96.0f + i * 52.0f), 24, Fade(RAYWHITE, tutorialAlpha));
    }

    const char* footer = "Pressione [ ENTER ] para Começar!";
    float footerX = (screenWidth - MeasureText(footer, 26)) / 2.0f;
    DrawText(footer, (int)footerX, (int)(panelY + panelHeight - 62.0f), 26, Fade(LIGHTGRAY, tutorialAlpha));
}

void DrawLogicPhase(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    ClearBackground((Color){10, 12, 24, 255});
    BeginMode2D(camera);

    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){8, 14, 30, 255});
    for (int i = 0; i < 20; i++) {
        float x = fmodf((GetTime() * (50.0f + i * 5.0f)) + i * 33.0f, screenWidth + 120.0f) - 60.0f;
        float y = 90.0f + fmodf(i * 34.0f + sinf(backgroundPulse + i) * 12.0f, screenHeight - 240.0f);
        DrawCircle((int)x, (int)y, 1.4f + (i % 3), (Color){135, 180, 255, 60});
    }

    Color bossColor = bossHitFlash > 0.0f ? WHITE : MAROON;
    DrawRectangleRec(boss.rect, bossColor);
    DrawRectangleLinesEx(boss.rect, 3.0f, RED);
    DrawText("BOSS LOGICO", (int)(boss.rect.x + (boss.rect.width - MeasureText("BOSS LOGICO", 20)) / 2.0f), (int)boss.rect.y + 16, 20, WHITE);

    Color playerColor = playerHitFlash > 0.0f ? WHITE : SKYBLUE;
    DrawRectangleRec(player.rect, playerColor);
    DrawRectangleLinesEx(player.rect, 2.0f, WHITE);

    for (int i = 0; i < LOGIC_DROP_COUNT; i++) {
        if (!drops[i].active) continue;
        Color dropColor = drops[i].value_is_true ? BLUE : RED;
        DrawRectangleRec(drops[i].rect, dropColor);
        DrawRectangleLinesEx(drops[i].rect, 2.0f, WHITE);
        DrawText(drops[i].value_is_true ? "V" : "F", (int)(drops[i].rect.x + (drops[i].rect.width - MeasureText(drops[i].value_is_true ? "V" : "F", 22)) / 2.0f), (int)drops[i].rect.y + 6, 22, WHITE);
    }

    for (int i = 0; i < LOGIC_MINION_COUNT; i++) {
        if (!minions[i].active) continue;
        DrawRectangleRec(minions[i].rect, ORANGE);
        DrawRectangleLinesEx(minions[i].rect, 2.0f, RED);
        DrawText("!", (int)(minions[i].rect.x + (minions[i].rect.width - MeasureText("!", 28)) / 2.0f), (int)minions[i].rect.y + 2, 28, BLACK);
    }

    DrawRectangle(0, 0, screenWidth, 70, Fade(BLACK, 0.85f));
    DrawText("PORTA ATUAL:", 20, 22, 20, LIGHTGRAY);
    DrawText(player.current_gate == GATE_AND ? "[ AND ]" : "[ OR ]", 160, 22, 22, SKYBLUE);
    DrawText("OBJETIVO:", screenWidth - 340, 22, 20, LIGHTGRAY);
    DrawText(player.expected_result ? "[ VERDADEIRO ]" : "[ FALSO ]", screenWidth - 170, 22, 22, player.expected_result ? GREEN : RED);

    int bossBarWidth = 520;
    int bossBarX = (screenWidth - bossBarWidth) / 2;
    int bossBarY = 75;
    DrawRectangle(bossBarX, bossBarY, bossBarWidth, 18, DARKGRAY);
    DrawRectangle(bossBarX, bossBarY, (int)((boss.animatedHp / boss.max_hp) * bossBarWidth), 18, RED);
    DrawRectangle(bossBarX, bossBarY, (int)(((float)boss.hp / boss.max_hp) * bossBarWidth), 18, GREEN);
    DrawText(TextFormat("BOSS: %d / %d", boss.hp, boss.max_hp), bossBarX + 14, bossBarY + 1, 18, WHITE);

    int playerBarWidth = 180;
    int playerBarX = 20;
    int playerBarY = screenHeight - 38;
    DrawRectangle(playerBarX, playerBarY, playerBarWidth, 16, DARKGRAY);
    DrawRectangle(playerBarX, playerBarY, (int)((player.animatedHp / 5.0f) * playerBarWidth), 16, SKYBLUE);
    DrawText(TextFormat("PLAYER: %d / 5", player.hp), playerBarX + 8, playerBarY - 2, 16, WHITE);
    DrawText(TextFormat("Tempo: %.1fs", tempoDeJogo), 20, 100, 20, WHITE);
    DrawText(TextFormat("Portas: %d", portasResolvidas), 20, 126, 20, WHITE);
    DrawText("Pressione M para voltar ao menu principal", 20, screenHeight - 60, 20, LIGHTGRAY);

    EndMode2D();
}

bool LogicPhaseHasEnded(void) {
    return phaseComplete;
}

void UpdateLogicReport(GameState* currentState) {
    if (IsKeyPressed(KEY_ENTER)) {
        InitLogicPhase();
        *currentState = STATE_LOGIC_PHASE;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        *currentState = STATE_MENU;
    }
}

void DrawLogicReport(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

    float pWidth = screenWidth * 0.7f;
    float pHeight = screenHeight * 0.6f;
    float pX = (screenWidth - pWidth) / 2.0f;
    float pY = (screenHeight - pHeight) / 2.0f;

    DrawRectangleRounded((Rectangle){pX, pY, pWidth, pHeight}, 0.1f, 10, Fade(RAYWHITE, 0.04f));
    DrawRectangleRoundedLines((Rectangle){pX, pY, pWidth, pHeight}, 0.1f, 10, Fade(RAYWHITE, 0.2f));

    const char* titleWin = "VITÓRIA LÓGICA!";
    const char* titleLose = "FALHA DE SISTEMA";
    int titleSize = 36;
    const char* titleToDraw = jogadorVenceu ? titleWin : titleLose;
    float titleX = (screenWidth - MeasureText(titleToDraw, titleSize)) / 2.0f;
    DrawText(titleToDraw, (int)titleX, (int)(pY + 28.0f), titleSize, jogadorVenceu ? GREEN : RED);

    int bodySize = 22;
    float statX = pX + 36.0f;
    float y = pY + 80.0f;
    DrawText(TextFormat("Tempo de Sobrevivência: %.1f s", tempoDeJogo), (int)statX, (int)y, bodySize, RAYWHITE);
    y += 36.0f;
    DrawText(TextFormat("Portas Lógicas Resolvidas: %d", portasResolvidas), (int)statX, (int)y, bodySize, RAYWHITE);
    y += 36.0f;
    DrawText(TextFormat("Vida Restante do Boss: %d", boss.hp), (int)statX, (int)y, bodySize, RAYWHITE);

    float blink = (sinf(GetTime() * 4.0f) + 1.0f) * 0.5f;
    Color optCol = Fade(LIGHTGRAY, 0.5f + blink * 0.5f);
    const char* footer = "[ ENTER ] JOGAR NOVAMENTE   |   [ ESC ] MENU PRINCIPAL";
    float footerX = (screenWidth - MeasureText(footer, 20)) / 2.0f;
    DrawText(footer, (int)footerX, (int)(pY + pHeight - 56.0f), 20, optCol);
}
