#include "questions.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define DESAFIO_RESULTADO_TEMPO 2.0f
#define DESAFIO_CONTAGEM_TEMPO 4.0f

// Desenha a estrela coletavel que abre o desafio de adivinhacao.
static void desenharEstrelaForma(float cx, float cy, float rExterno, float rInterno, int pontas, Color cor) {
    float angulo = -PI / 2.0f;
    float passo  = PI / pontas;

    for (int i = 0; i < pontas * 2; i++) {
        float r  = (i % 2 == 0) ? rExterno : rInterno;
        float x1 = cx + cosf(angulo) * r;
        float y1 = cy + sinf(angulo) * r;
        angulo += passo;

        float r2 = (( i + 1) % 2 == 0) ? rExterno : rInterno;
        float x2 = cx + cosf(angulo) * r2;
        float y2 = cy + sinf(angulo) * r2;

        DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2.5f, cor);
        DrawTriangle(
            (Vector2){cx, cy},
            (Vector2){x1, y1},
            (Vector2){x2, y2},
            cor
        );
    }
}

static void limparEntradaDesafio(DesafioPergunta* desafio) {
    desafio->entrada[0] = '\0';
    desafio->tamanhoEntrada = 0;
}

static void drawTextoCentralizado(const char* texto, int y, int tamanhoFonte, Color cor) {
    DrawText(texto, (GetScreenWidth() - MeasureText(texto, tamanhoFonte)) / 2, y, tamanhoFonte, cor);
}

static void drawTextoCentralizadoNoModal(const char* texto, Rectangle modal, int y, int tamanhoFonte, Color cor) {
    DrawText(texto, (int)(modal.x + (modal.width - MeasureText(texto, tamanhoFonte)) / 2), y, tamanhoFonte, cor);
}

// Centraliza caixas maiores sem deixar que saiam da tela em larguras menores.
static Rectangle getModalCentral(float largura, float altura) {
    if (largura > GetScreenWidth() - 100) largura = (float)GetScreenWidth() - 100.0f;
    return (Rectangle){
        ((float)GetScreenWidth() - largura) / 2.0f,
        ((float)GetScreenHeight() - altura) / 2.0f,
        largura,
        altura
    };
}

static void drawAvisoIcone(int x, int y) {
    DrawTriangleLines((Vector2){(float)x, (float)(y - 10)},
                      (Vector2){(float)(x - 8), (float)(y + 6)},
                      (Vector2){(float)(x + 8), (float)(y + 6)},
                      YELLOW);
    DrawText("!", x - 2, y - 5, 12, YELLOW);
}

static bool botaoClicado(Rectangle botao) {
    return CheckCollisionPointRec(GetMousePosition(), botao) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Traduz o status numerico da regra de chute para a mensagem exibida no modal.
static const char* textoDica(StatusChute status) {
    switch (status) {
        case passouLonge: return "Passou longe!!";
        case muitoPerto: return "VOCÊ ESTÁ MUITO PERTO!!";
        case perto: return "Está perto!!!";
        case estrategiaDezena: return "Tente outra estratégia!!";
        case muitoAlto: return "Muito alto!";
        case muitoBaixo: return "Muito baixo!";
        case correto:
        default: return "Correto!";
    }
}

static bool estaPerto(StatusChute status) {
    return status == muitoPerto || status == perto;
}

// Entra na tela de resultado antes da contagem para descongelar o jogo.
static void finalizarDesafio(DesafioPergunta* desafio, bool acertou) {
    desafio->acertou = acertou;
    desafio->estado = DESAFIO_RESULTADO;
    desafio->timerResultado = DESAFIO_RESULTADO_TEMPO;
    limparEntradaDesafio(desafio);
}

static void processarPalpite(DesafioPergunta* desafio, Player* jogador) {
    if (desafio->tamanhoEntrada <= 0) return;

    int palpite = atoi(desafio->entrada);
    desafio->ultimoPalpite = palpite;
    limparEntradaDesafio(desafio);

    if (palpite < NUMEROMIN || palpite > NUMEROMAX) {
        desafio->dica = "Digite 1 a 100.";
        return;
    }

    if (palpite % 10 == 0) {
        desafio->chutesTerminadosEmZero++;
    }

    executaQuestao(&desafio->questao, desafio->questao.numeroSecreto, palpite);

    if (desafio->questao.status == correto) {
        finalizarDesafio(desafio, true);
        return;
    }

    // Easter egg fica antes das dicas genericas de alto/baixo, mas nao cobre proximidade.
    if (palpite % 10 == 0 && desafio->chutesTerminadosEmZero >= ESTRATEGIA_MIN_DEZENAS &&
        (desafio->questao.status == muitoAlto || desafio->questao.status == muitoBaixo)) {
        desafio->questao.status = estrategiaDezena;
    }

    desafio->tentativasRestantes--;
    desafio->dica = textoDica(desafio->questao.status);

    // Ao zerar tentativas, oferece uma ultima chance que custa 1 vida.
    if (desafio->tentativasRestantes <= 0) {
        if (!desafio->usouUltimaChance && jogador->hp > 0) {
            desafio->estado = DESAFIO_CONFIRMAR_ULTIMA;
        } else {
            finalizarDesafio(desafio, false);
        }
    }
}

static void atualizarEntradaDesafio(DesafioPergunta* desafio, Player* jogador) {
    Rectangle modal = {(GetScreenWidth() - 360) / 2.0f, 92, 360, 270};
    Rectangle enviar = {modal.x + 27, modal.y + 164, 306, 42};
    int tecla = GetCharPressed();

    // Aceita apenas numeros para manter o chute entre 1 e 100.
    while (tecla > 0) {
        if (tecla >= '0' && tecla <= '9' && desafio->tamanhoEntrada < DESAFIO_TAMANHO_ENTRADA - 1) {
            desafio->entrada[desafio->tamanhoEntrada] = (char)tecla;
            desafio->tamanhoEntrada++;
            desafio->entrada[desafio->tamanhoEntrada] = '\0';
        }
        tecla = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && desafio->tamanhoEntrada > 0) {
        desafio->tamanhoEntrada--;
        desafio->entrada[desafio->tamanhoEntrada] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) || botaoClicado(enviar)) {
        processarPalpite(desafio, jogador);
    }
}

static void drawTentativas(const DesafioPergunta* desafio, Rectangle modal) {
    int inicioX = (int)(modal.x + modal.width - 88);
    int y = (int)(modal.y + 38);

    for (int i = 0; i < DESAFIO_MAX_TENTATIVAS; i++) {
        Color cor = i < desafio->tentativasRestantes ? GREEN : DARKGRAY;
        DrawCircle(inicioX + i * 13, y, 4, cor);
        DrawCircleLines(inicioX + i * 13, y, 4, GRAY);
    }
}

// Tela principal do desafio: entrada do chute, botao e indicadores de chance.
static void drawTelaChute(const DesafioPergunta* desafio) {
    Rectangle modal = {(GetScreenWidth() - 360) / 2.0f, 92, 360, 270};
    Rectangle entrada = {modal.x + 27, modal.y + 116, 306, 38};
    Rectangle enviar = {modal.x + 27, modal.y + 164, 306, 42};

    DrawRectangleRec(modal, (Color){18, 18, 18, 248});
    DrawRectangleLinesEx(modal, 2.0f, WHITE);
    drawAvisoIcone((int)modal.x + 34, (int)modal.y + 36);
    DrawText("DESAFIO CRITICO", (int)modal.x + 54, (int)modal.y + 29, 16, WHITE);
    DrawText("Adivinhe: 1 a 100", (int)modal.x + 27, (int)modal.y + 66, 14, WHITE);
    drawTentativas(desafio, modal);

    if (desafio->dica[0] != '\0') {
        DrawText(desafio->dica,
                 (GetScreenWidth() - MeasureText(desafio->dica, 14)) / 2,
                 (int)modal.y + 96, 14, RED);
    }

    DrawRectangleRec(entrada, BLACK);
    DrawRectangleLinesEx(entrada, 1.5f, YELLOW);
    DrawText(desafio->tamanhoEntrada > 0 ? desafio->entrada : "Digite um numero...",
             (int)entrada.x + 14, (int)entrada.y + 12, 14,
             desafio->tamanhoEntrada > 0 ? WHITE : GRAY);

    DrawRectangleRec(enviar, YELLOW);
    DrawText("ENVIAR", (int)enviar.x + 128, (int)enviar.y + 13, 14, BLACK);
}

// Tela exibida quando as 5 tentativas acabam e o player pode gastar 1 vida.
static void drawTelaConfirmacao(const DesafioPergunta* desafio, const Player* jogador) {
    Rectangle modal = {(GetScreenWidth() - 360) / 2.0f, 120, 360, 220};
    Rectangle sim = {modal.x + 30, modal.y + 158, 138, 42};
    Rectangle nao = {modal.x + 192, modal.y + 158, 138, 42};
    const char* mensagem = estaPerto(desafio->questao.status) ? "VOCÊ ESTÁ MUITO PERTO!!" : "QUASE LA!!";

    DrawRectangleRec(modal, (Color){18, 18, 18, 248});
    DrawRectangleLinesEx(modal, 2.0f, WHITE);
    drawAvisoIcone((int)modal.x + 34, (int)modal.y + 36);
    DrawText("TENTATIVAS ESGOTADAS", (int)modal.x + 54, (int)modal.y + 29, 16, WHITE);
    drawTextoCentralizado(mensagem, (int)modal.y + 72, 18, YELLOW);

    if (jogador->hp > 0) {
        DrawText("Quer gastar", (int)modal.x + 51, (int)modal.y + 108, 13, WHITE);
        DrawText("1 vida", (int)modal.x + 150, (int)modal.y + 108, 13, RED);
        DrawText("para uma ultima", (int)modal.x + 207, (int)modal.y + 108, 13, WHITE);
        drawTextoCentralizado("tentativa?", (int)modal.y + 126, 13, WHITE);
    } else {
        drawTextoCentralizado("Sem vidas para tentar de novo.", (int)modal.y + 112, 13, WHITE);
    }

    DrawRectangleRec(sim, jogador->hp > 0 ? YELLOW : DARKGRAY);
    DrawRectangleRec(nao, GRAY);
    DrawText("SIM", (int)sim.x + 54, (int)sim.y + 14, 14, BLACK);
    DrawText("NAO", (int)nao.x + 54, (int)nao.y + 14, 14, WHITE);
}

// Resultado final antes de voltar ao combate: acerto em verde, erro em vermelho.
static void drawTelaResultado(const DesafioPergunta* desafio) {
    Color borda = desafio->acertou ? GREEN : RED;
    Color fundo = desafio->acertou ? (Color){8, 45, 24, 215} : (Color){45, 8, 8, 215};
    Rectangle modal = getModalCentral(620.0f, 240.0f);

    DrawRectangleRec(modal, fundo);
    DrawRectangleLinesEx(modal, 3.0f, borda);

    drawTextoCentralizadoNoModal(desafio->acertou ? "*" : "X", modal, (int)modal.y + 34, 42, borda);
    drawTextoCentralizadoNoModal(desafio->acertou ? "Parabens voce acertou o numero!!!" : "Nao foi dessa vez!!", modal, (int)modal.y + 88, 16, borda);
    if (desafio->acertou) {
        drawTextoCentralizadoNoModal("Voce ganhara um bonus de Dano Aumentado!!", modal, (int)modal.y + 132, 15, WHITE);
    } else {
        drawTextoCentralizadoNoModal(TextFormat("O numero era %d.", desafio->questao.numeroSecreto), modal, (int)modal.y + 126, 16, WHITE);
        drawTextoCentralizadoNoModal("Procure o proximo atributo e tente de", modal, (int)modal.y + 170, 15, WHITE);
    }
    if (!desafio->acertou) {
        drawTextoCentralizadoNoModal("novo!!!", modal, (int)modal.y + 192, 15, WHITE);
    }
}

// Contagem de retorno seguindo o video de referencia: 3, 2, 1, VAI.
static void drawTelaContagem(const DesafioPergunta* desafio) {
    Rectangle modal = getModalCentral(620.0f, 180.0f);
    float tempoDecorrido = DESAFIO_CONTAGEM_TEMPO - desafio->timerContagem;
    const char* texto = "VAI!";

    if (tempoDecorrido < 1.0f) texto = "3";
    else if (tempoDecorrido < 2.0f) texto = "2";
    else if (tempoDecorrido < 3.0f) texto = "1";

    DrawRectangleRec(modal, (Color){45, 8, 8, 185});
    DrawRectangleLinesEx(modal, 3.0f, RED);
    drawTextoCentralizadoNoModal(texto, modal, (int)modal.y + 51, 72, (Color){100, 0, 0, 120});
    drawTextoCentralizadoNoModal(texto, modal, (int)modal.y + 48, 72, (Color){160, 20, 20, 170});
    drawTextoCentralizadoNoModal(texto, modal, (int)modal.y + 45, 72, RED);
}

int gerarNumeroSecreto(void) {
    return GetRandomValue(NUMEROMIN, NUMEROMAX);
}

// Avalia a distancia absoluta seguindo a ordem de prioridade das dicas.
void executaQuestao(Questao *questao, int numeroSecreto, int chute) {
    questao->numeroSecreto = numeroSecreto;
    int diferenca = abs(chute - numeroSecreto);

    if (chute == numeroSecreto) {
        questao->status = correto;
    } else if (diferenca > DISTANCIA_LONGE) {
        questao->status = passouLonge;
    } else if (diferenca <= DISTANCIA_MUITO_PERTO) {
        questao->status = muitoPerto;
    } else if (diferenca <= DISTANCIA_PERTO) {
        questao->status = perto;
    } else if (chute > numeroSecreto) {
        questao->status = muitoAlto;
    } else {
        questao->status = muitoBaixo;
    }
}

void inicializarEstrela(Estrela* estrela) {
    estrela->x            = 0;
    estrela->y            = 0;
    estrela->ativa        = false;
    estrela->timerAparição = 0.0f;
}

void inicializarDesafio(DesafioPergunta* desafio) {
    desafio->estado = DESAFIO_INATIVO;
    desafio->questao.numeroSecreto = 0;
    desafio->questao.status = correto;
    desafio->tentativasRestantes = DESAFIO_MAX_TENTATIVAS;
    desafio->ultimoPalpite = 0;
    desafio->chutesTerminadosEmZero = 0;
    limparEntradaDesafio(desafio);
    desafio->dica = "";
    desafio->acertou = false;
    desafio->usouUltimaChance = false;
    desafio->timerResultado = 0.0f;
    desafio->timerContagem = 0.0f;
}

void iniciarDesafio(DesafioPergunta* desafio) {
    inicializarDesafio(desafio);
    desafio->estado = DESAFIO_CHUTANDO;
    desafio->questao.numeroSecreto = gerarNumeroSecreto();
    desafio->dica = "";
}

bool atualizarDesafio(DesafioPergunta* desafio, Player* jogador, float deltaTime) {
    Rectangle modalConfirmacao = {(GetScreenWidth() - 360) / 2.0f, 120, 360, 220};
    Rectangle botaoSim = {modalConfirmacao.x + 30, modalConfirmacao.y + 158, 138, 42};
    Rectangle botaoNao = {modalConfirmacao.x + 192, modalConfirmacao.y + 158, 138, 42};

    // Maquina de estados do pop-up: entrada, confirmacao, resultado e retorno.
    switch (desafio->estado) {
        case DESAFIO_CHUTANDO:
            atualizarEntradaDesafio(desafio, jogador);
            break;

        case DESAFIO_CONFIRMAR_ULTIMA:
            // SIM/Enter/S gastam 1 vida e liberam exatamente mais um chute.
            if (jogador->hp > 0 && (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_ENTER) || botaoClicado(botaoSim))) {
                aplicarDanoPlayer(jogador, 1);
                desafio->estado = DESAFIO_CHUTANDO;
                desafio->tentativasRestantes = 1;
                desafio->usouUltimaChance = true;
                desafio->dica = "Ultima tentativa!";
                limparEntradaDesafio(desafio);
            } else if (IsKeyPressed(KEY_N) || botaoClicado(botaoNao) || jogador->hp <= 0) {
                finalizarDesafio(desafio, false);
            }
            break;

        case DESAFIO_RESULTADO:
            desafio->timerResultado -= deltaTime;
            if (desafio->timerResultado <= 0.0f) {
                desafio->estado = DESAFIO_CONTAGEM;
                desafio->timerContagem = DESAFIO_CONTAGEM_TEMPO;
            }
            break;

        case DESAFIO_CONTAGEM:
            desafio->timerContagem -= deltaTime;
            if (desafio->timerContagem <= 0.0f) {
                inicializarDesafio(desafio);
                return false;
            }
            break;

        case DESAFIO_INATIVO:
        default:
            return false;
    }

    return true;
}

void drawDesafio(const DesafioPergunta* desafio, const Player* jogador) {
    if (desafio->estado == DESAFIO_INATIVO) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 150});

    // O jogo ja foi desenhado; aqui so renderizamos a camada modal por cima.
    switch (desafio->estado) {
        case DESAFIO_CHUTANDO:
            drawTelaChute(desafio);
            break;

        case DESAFIO_CONFIRMAR_ULTIMA:
            drawTelaConfirmacao(desafio, jogador);
            break;

        case DESAFIO_RESULTADO:
            drawTelaResultado(desafio);
            break;

        case DESAFIO_CONTAGEM:
            drawTelaContagem(desafio);
            break;

        case DESAFIO_INATIVO:
        default:
            break;
    }
}

bool atualizarEstrela(Estrela* estrela, const Boss* boss, const Player* jogador, float deltaTime) {
    if (!estrela->ativa) {
        estrela->timerAparição += deltaTime;
        if (estrela->timerAparição >= ESTRELA_INTERVALO) {
            estrela->x = boss->posicaoX + boss->largura / 2.0f;
            estrela->y = boss->posicaoY + boss->altura / 2.0f;
            estrela->ativa = true;
            estrela->timerAparição = 0.0f;
        }
        return false;
    }

    estrela->y += ESTRELA_VELOCIDADE * deltaTime;

    if (estrela->y - ESTRELA_RAIO > GetScreenHeight()) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return false;
    }

    float dx  = estrela->x - jogador->posicaoX;
    float dy  = estrela->y - jogador->posicaoY;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < ESTRELA_RAIO + PLAYER_RADIUS) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return true;
    }

    return false;
}

void drawEstrela(const Estrela* estrela) {
    if (!estrela->ativa) return;

    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 6, (Color){255, 230, 50, 60});
    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 3, (Color){255, 220, 80, 100});

    desenharEstrelaForma(estrela->x, estrela->y,
                         ESTRELA_RAIO, ESTRELA_RAIO * 0.45f,
                         ESTRELA_PONTAS, YELLOW);

    DrawCircle((int)estrela->x, (int)estrela->y, 4, WHITE);
}
