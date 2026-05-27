#include "audio.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

#define TRILHA_VOLUME 0.45f
#define SFX_TIRO_VOLUME 0.55f

static Music trilha;
static Sound sfxTiro;
static bool audioPronto = false;
static bool trilhaCarregada = false;
static bool sfxTiroCarregado = false;
static bool trilhaPermitida = false;

static void sinalizarAudioHost(const char* evento) {
    printf("[MINDDROP] %s\n", evento);
    fflush(stdout);
}

static bool carregarTrilhaDeArquivo(const char* caminho) {
    if (caminho == NULL || !FileExists(caminho)) return false;

    trilha = LoadMusicStream(caminho);
    SetMusicVolume(trilha, TRILHA_VOLUME);
    trilhaCarregada = true;
    return true;
}

static bool tentarCarregarTrilha(void) {
    const char* appDir = GetApplicationDirectory();
    char caminho[520];

    if (carregarTrilhaDeArquivo("musica/trilha.mp3")) return true;
    if (carregarTrilhaDeArquivo("resources/musica/trilha.mp3")) return true;

    snprintf(caminho, sizeof(caminho), "%smusica/trilha.mp3", appDir);
    if (carregarTrilhaDeArquivo(caminho)) return true;

    snprintf(caminho, sizeof(caminho), "%sresources/musica/trilha.mp3", appDir);
    if (carregarTrilhaDeArquivo(caminho)) return true;

    snprintf(caminho, sizeof(caminho), "%s../musica/trilha.mp3", appDir);
    if (carregarTrilhaDeArquivo(caminho)) return true;

    snprintf(caminho, sizeof(caminho), "%s../../musica/trilha.mp3", appDir);
    if (carregarTrilhaDeArquivo(caminho)) return true;

    if (carregarTrilhaDeArquivo("../musica/trilha.mp3")) return true;
    if (carregarTrilhaDeArquivo("../musica/THE FLIPSIDE - boggio (youtube).mp3")) return true;

    return false;
}

static bool tentarCarregarSfxTiro(void) {
    const char* caminhos[] = {
        "musica/freesound_community-shoot-5-102360.mp3",
        "resources/musica/freesound_community-shoot-5-102360.mp3",
        "../musica/freesound_community-shoot-5-102360.mp3"
    };

    for (int i = 0; i < 3; i++) {
        if (!FileExists(caminhos[i])) continue;
        sfxTiro = LoadSound(caminhos[i]);
        SetSoundVolume(sfxTiro, SFX_TIRO_VOLUME);
        sfxTiroCarregado = true;
        return true;
    }

    return false;
}

void inicializarAudio(void) {
    InitAudioDevice();
    audioPronto = IsAudioDeviceReady();
    if (!audioPronto) return;

    trilhaCarregada = tentarCarregarTrilha();
    sfxTiroCarregado = tentarCarregarSfxTiro();
}

void encerrarAudio(void) {
    pararTrilhaSonora();

    if (trilhaCarregada) {
        StopMusicStream(trilha);
        UnloadMusicStream(trilha);
        trilhaCarregada = false;
    }

    if (sfxTiroCarregado) {
        UnloadSound(sfxTiro);
        sfxTiroCarregado = false;
    }

    if (audioPronto) {
        CloseAudioDevice();
        audioPronto = false;
    }
}

void atualizarAudio(void) {
    if (!trilhaPermitida || !audioPronto || !trilhaCarregada) return;

    UpdateMusicStream(trilha);

    if (!IsMusicStreamPlaying(trilha)) {
        PlayMusicStream(trilha);
    }
}

void iniciarTrilhaSonora(void) {
    trilhaPermitida = true;
    sinalizarAudioHost("MUSIC:START");

    if (audioPronto && trilhaCarregada && !IsMusicStreamPlaying(trilha)) {
        PlayMusicStream(trilha);
    }
}

void pararTrilhaSonora(void) {
    if (!trilhaPermitida) return;

    trilhaPermitida = false;
    sinalizarAudioHost("MUSIC:STOP");

    if (audioPronto && trilhaCarregada && IsMusicStreamPlaying(trilha)) {
        StopMusicStream(trilha);
    }
}

void pausarTrilhaSonora(bool pausar) {
    if (!audioPronto || !trilhaCarregada || !trilhaPermitida) return;

    if (pausar) {
        PauseMusicStream(trilha);
    } else if (!IsMusicStreamPlaying(trilha)) {
        ResumeMusicStream(trilha);
    }
}

bool trilhaSonoraAtiva(void) {
    return trilhaPermitida && audioPronto && trilhaCarregada && IsMusicStreamPlaying(trilha);
}

void tocarSfxTiro(void) {
    sinalizarAudioHost("SFX:SHOOT");

    if (audioPronto && sfxTiroCarregado) {
        PlaySound(sfxTiro);
    }
}
