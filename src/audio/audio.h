#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

void inicializarAudio(void);
void encerrarAudio(void);
void atualizarAudio(void);
void iniciarTrilhaSonora(void);
void pausarTrilhaSonora(bool pausar);
bool trilhaSonoraAtiva(void);

#endif
