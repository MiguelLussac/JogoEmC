#include "audio.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

#define TRILHA_VOLUME 0.45f

static Music trilha;
static bool audioPronto = false;
static bool trilhaCarregada = false;

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

void inicializarAudio(void) {
    InitAudioDevice();
    audioPronto = IsAudioDeviceReady();
    if (!audioPronto) return;

    trilhaCarregada = tentarCarregarTrilha();
}

void encerrarAudio(void) {
    if (trilhaCarregada) {
        StopMusicStream(trilha);
        UnloadMusicStream(trilha);
        trilhaCarregada = false;
    }

    if (audioPronto) {
        CloseAudioDevice();
        audioPronto = false;
    }
}

void atualizarAudio(void) {
    if (!audioPronto || !trilhaCarregada) return;

    UpdateMusicStream(trilha);

    if (!IsMusicStreamPlaying(trilha)) {
        PlayMusicStream(trilha);
    }
}

void iniciarTrilhaSonora(void) {
    if (!audioPronto || !trilhaCarregada) return;
    if (!IsMusicStreamPlaying(trilha)) {
        PlayMusicStream(trilha);
    }
}

void pausarTrilhaSonora(bool pausar) {
    if (!audioPronto || !trilhaCarregada) return;

    if (pausar) {
        PauseMusicStream(trilha);
    } else if (!IsMusicStreamPlaying(trilha)) {
        ResumeMusicStream(trilha);
    }
}

bool trilhaSonoraAtiva(void) {
    return audioPronto && trilhaCarregada && IsMusicStreamPlaying(trilha);
}
