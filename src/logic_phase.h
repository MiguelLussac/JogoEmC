#ifndef LOGIC_PHASE_H
#define LOGIC_PHASE_H

#include "raylib.h"
#include "game_state.h"
#include <stdbool.h>

typedef enum LogicPhaseGate {
    GATE_AND,
    GATE_OR
} LogicPhaseGate;

typedef struct LogicDrop {
    Rectangle rect;
    bool value_is_true;
    float speed;
    float swayOffset;
    float swaySpeed;
    bool active;
} LogicDrop;

typedef struct LogicMinion {
    Rectangle rect;
    bool active;
    float speed;
} LogicMinion;

typedef struct LogicPlayer {
    Rectangle rect;
    int hp;
    float animatedHp;
    LogicPhaseGate current_gate;
    bool expected_result;
    bool slot1_filled;
    bool slot1_val;
    bool slot2_filled;
    bool slot2_val;
} LogicPlayer;

typedef struct LogicBoss {
    Rectangle rect;
    int hp;
    float animatedHp;
    int max_hp;
} LogicBoss;

void InitLogicPhase(void);
void UpdateLogicPhase(GameState* currentState);
void DrawLogicPhase(void);
void UpdateLogicTutorial(GameState* currentState);
void DrawLogicTutorial(void);
void UpdateLogicReport(GameState* currentState);
void DrawLogicReport(void);
bool LogicPhaseHasEnded(void);

#endif
