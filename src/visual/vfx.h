#ifndef VFX_H
#define VFX_H

#include "raylib.h"

#define VFX_MAX_PARTICLES 96

typedef struct {
    float x, y, vx, vy;
    float vida, vidaMax;
    float tamanho;
    Color cor;
    bool  ativo;
} Particula;

typedef struct {
    Particula lista[VFX_MAX_PARTICLES];
    float shakeTimer;
    float shakeIntensidade;
} VfxEstado;

void vfxInicializar(void);
VfxEstado* vfxObter(void);
void vfxAtualizar(VfxEstado* vfx, float deltaTime);
void vfxDesenhar(const VfxEstado* vfx);
void vfxExplosao(VfxEstado* vfx, float x, float y, Color cor, int quantidade, float forca);
void vfxRastro(VfxEstado* vfx, float x, float y, Color cor);
void vfxTremer(VfxEstado* vfx, float intensidade, float duracao);
Vector2 vfxOffsetShake(const VfxEstado* vfx);

void vfxDesenharGlowCirculo(float x, float y, float raio, Color cor, float intensidade);
void vfxDesenharGlowLinha(Vector2 inicio, Vector2 fim, float grossura, Color cor, float intensidade);

#endif
