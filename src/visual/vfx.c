#include "vfx.h"
#include <math.h>
#include <stdlib.h>

static VfxEstado g_vfx;

static int buscarParticulaLivre(VfxEstado* vfx) {
    for (int i = 0; i < VFX_MAX_PARTICLES; i++)
        if (!vfx->lista[i].ativo) return i;
    return -1;
}

void vfxInicializar(void) {
    for (int i = 0; i < VFX_MAX_PARTICLES; i++)
        g_vfx.lista[i].ativo = false;
    g_vfx.shakeTimer = 0.0f;
    g_vfx.shakeIntensidade = 0.0f;
}

VfxEstado* vfxObter(void) {
    return &g_vfx;
}

void vfxExplosao(VfxEstado* vfx, float x, float y, Color cor, int quantidade, float forca) {
    if (quantidade > 24) quantidade = 24;
    for (int i = 0; i < quantidade; i++) {
        int idx = buscarParticulaLivre(vfx);
        if (idx < 0) break;
        float ang = ((float)i / (float)quantidade) * 6.28318f + (float)GetRandomValue(-30, 30) / 100.0f;
        float vel = forca * (0.4f + (float)GetRandomValue(0, 100) / 140.0f);
        vfx->lista[idx].x = x;
        vfx->lista[idx].y = y;
        vfx->lista[idx].vx = cosf(ang) * vel;
        vfx->lista[idx].vy = sinf(ang) * vel;
        vfx->lista[idx].vida = vfx->lista[idx].vidaMax = 0.25f + (float)GetRandomValue(0, 80) / 400.0f;
        vfx->lista[idx].tamanho = 2.0f + (float)GetRandomValue(0, 40) / 10.0f;
        vfx->lista[idx].cor = cor;
        vfx->lista[idx].ativo = true;
    }
}

void vfxRastro(VfxEstado* vfx, float x, float y, Color cor) {
    if (GetRandomValue(0, 100) > 35) return;
    int idx = buscarParticulaLivre(vfx);
    if (idx < 0) return;
    vfx->lista[idx].x = x + (float)GetRandomValue(-3, 3);
    vfx->lista[idx].y = y + (float)GetRandomValue(-3, 3);
    vfx->lista[idx].vx = (float)GetRandomValue(-20, 20) / 10.0f;
    vfx->lista[idx].vy = (float)GetRandomValue(10, 40) / 10.0f;
    vfx->lista[idx].vida = vfx->lista[idx].vidaMax = 0.18f;
    vfx->lista[idx].tamanho = 1.5f + (float)GetRandomValue(0, 20) / 10.0f;
    vfx->lista[idx].cor = cor;
    vfx->lista[idx].ativo = true;
}

void vfxTremer(VfxEstado* vfx, float intensidade, float duracao) {
    if (duracao > vfx->shakeTimer) {
        vfx->shakeTimer = duracao;
        vfx->shakeIntensidade = intensidade;
    }
}

Vector2 vfxOffsetShake(const VfxEstado* vfx) {
    if (vfx->shakeTimer <= 0.0f) return (Vector2){ 0.0f, 0.0f };
    return (Vector2){
        (float)GetRandomValue((int)(-vfx->shakeIntensidade), (int)vfx->shakeIntensidade),
        (float)GetRandomValue((int)(-vfx->shakeIntensidade), (int)vfx->shakeIntensidade)
    };
}

void vfxAtualizar(VfxEstado* vfx, float deltaTime) {
    if (vfx->shakeTimer > 0.0f) {
        vfx->shakeTimer -= deltaTime;
        if (vfx->shakeTimer < 0.0f) vfx->shakeTimer = 0.0f;
    }
    for (int i = 0; i < VFX_MAX_PARTICLES; i++) {
        if (!vfx->lista[i].ativo) continue;
        vfx->lista[i].x += vfx->lista[i].vx * deltaTime;
        vfx->lista[i].y += vfx->lista[i].vy * deltaTime;
        vfx->lista[i].vy += 120.0f * deltaTime;
        vfx->lista[i].vida -= deltaTime;
        if (vfx->lista[i].vida <= 0.0f) vfx->lista[i].ativo = false;
    }
}

void vfxDesenharGlowCirculo(float x, float y, float raio, Color cor, float intensidade) {
    if (intensidade < 0.05f) intensidade = 0.05f;
    for (int i = 3; i >= 1; i--) {
        float r = raio + (float)i * 3.5f * intensidade;
        Color c = { cor.r, cor.g, cor.b, (unsigned char)(cor.a * 0.12f * (float)i) };
        DrawCircle((int)x, (int)y, r, c);
    }
    DrawCircle((int)x, (int)y, raio, cor);
}

void vfxDesenharGlowLinha(Vector2 inicio, Vector2 fim, float grossura, Color cor, float intensidade) {
    DrawLineEx(inicio, fim, grossura + 4.0f * intensidade, (Color){ cor.r, cor.g, cor.b, (unsigned char)(cor.a * 0.25f) });
    DrawLineEx(inicio, fim, grossura, cor);
}

void vfxDesenhar(const VfxEstado* vfx) {
    for (int i = 0; i < VFX_MAX_PARTICLES; i++) {
        if (!vfx->lista[i].ativo) continue;
        float alpha = vfx->lista[i].vida / vfx->lista[i].vidaMax;
        if (alpha < 0.0f) alpha = 0.0f;
        Color c = {
            vfx->lista[i].cor.r,
            vfx->lista[i].cor.g,
            vfx->lista[i].cor.b,
            (unsigned char)(vfx->lista[i].cor.a * alpha)
        };
        float t = vfx->lista[i].tamanho * (0.5f + alpha * 0.5f);
        DrawCircle((int)vfx->lista[i].x, (int)vfx->lista[i].y, t, c);
    }
}
