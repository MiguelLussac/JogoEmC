#include "questions.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// Limpa o buffer digitado no modal sem alterar o restante do estado do desafio.
static void limparEntradaDesafio(DesafioPergunta* desafio) {
    desafio->entrada[0] = '\0';
    desafio->tamanhoEntrada = 0;
}

// Centraliza um texto na largura total da tela.
static void drawTextoCentralizado(const char* texto, int y, int tamanhoFonte, Color cor) {
    DrawText(texto, (GetScreenWidth() - MeasureText(texto, tamanhoFonte)) / 2, y, tamanhoFonte, cor);
}

// Centraliza um texto dentro de um retangulo de modal especifico.
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

// Desenha o icone triangular usado nos modais de alerta do desafio.
static void drawAvisoIcone(int x, int y) {
    DrawTriangleLines((Vector2){(float)x, (float)(y - 10)},
                      (Vector2){(float)(x - 8), (float)(y + 6)},
                      (Vector2){(float)(x + 8), (float)(y + 6)},
                      YELLOW);
    DrawText("!", x - 2, y - 5, 12, YELLOW);
}

// Encapsula a regra de clique nos botoes desenhados manualmente com raylib.
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

// Agrupa os status considerados proximos para reaproveitar em mensagens finais.
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

static void aplicarBoostAleatorio(DesafioPergunta* desafio, Player* jogador) {
    char mensagem[64];
    bool aplicado = aplicarBoostAleatorioPlayer(jogador, mensagem, sizeof(mensagem));
    desafio->buffAplicado = aplicado;
    
    if (aplicado || mensagem[0] != '\0') {
        desafio->bonus = TextFormat("%s", mensagem);
    } else {
        desafio->bonus = "Nenhum buff aplicado.";
    }

    if (strstr(mensagem, "VIDA")) desafio->corBonus = GREEN;
    else if (strstr(mensagem, "DANO")) desafio->corBonus = RED;
    else if (strstr(mensagem, "VELOCIDADE")) desafio->corBonus = SKYBLUE;
    else desafio->corBonus = GRAY;
}

static void processarPalpite(DesafioPergunta* desafio, Player* jogador) {
    if (desafio->tamanhoEntrada <= 0) return;

    int palpite = atoi(desafio->entrada);
    desafio->ultimoPalpite = palpite;
    limparEntradaDesafio(desafio);

    if (palpite < NUMEROMIN || palpite > NUMEROMAX) {
        strcpy(desafio->mensagemSistema, "Digite 1 a 100.");
        return;
    }

    if (palpite % 10 == 0) {
        desafio->chutesTerminadosEmZero++;
    }

    executaQuestao(&desafio->questao, desafio->questao.numeroSecreto, palpite);

    if (desafio->questao.status == correto) {
        aplicarBoostAleatorio(desafio, jogador);
        finalizarDesafio(desafio, true);
        return;
    }

    desafio->tentativasRestantes--;

    int diferenca = abs(palpite - desafio->questao.numeroSecreto);
    if (diferenca > 30) {
        strcpy(desafio->textoDistancia, "Muito distante");
        desafio->corDistancia = RED;
    } else if (diferenca > 15) {
        strcpy(desafio->textoDistancia, "Distante");
        desafio->corDistancia = ORANGE;
    } else if (diferenca > 7) {
        strcpy(desafio->textoDistancia, "Proximo");
        desafio->corDistancia = YELLOW;
    } else if (diferenca > 3) {
        strcpy(desafio->textoDistancia, "Muito proximo");
        desafio->corDistancia = GREEN;
    } else {
        strcpy(desafio->textoDistancia, "Extremamente proximo");
        desafio->corDistancia = RAYWHITE;
    }

    if (palpite < desafio->questao.numeroSecreto) {
        strcpy(desafio->textoDirecao, "O numero correto e MAIOR");
        if (palpite >= desafio->limiteInferior) desafio->limiteInferior = palpite + 1;
    } else {
        strcpy(desafio->textoDirecao, "O numero correto e MENOR");
        if (palpite <= desafio->limiteSuperior) desafio->limiteSuperior = palpite - 1;
    }

    snprintf(desafio->textoIntervalo, sizeof(desafio->textoIntervalo), "O valor esta entre %d e %d", desafio->limiteInferior, desafio->limiteSuperior);

    if (desafio->tentativasRestantes <= 2) {
        switch (desafio->tipoDicaAvancada) {
            case DICA_PAR: strcpy(desafio->textoDicaAvancada, "* Dica: O numero e PAR"); break;
            case DICA_IMPAR: strcpy(desafio->textoDicaAvancada, "* Dica: O numero e IMPAR"); break;
            case DICA_MULTIPLO_5: strcpy(desafio->textoDicaAvancada, "* Dica: O numero e MULTIPLO DE 5"); break;
            case DICA_MAIOR_50: strcpy(desafio->textoDicaAvancada, "* Dica: O numero e MAIOR QUE 50"); break;
            case DICA_MENOR_50: strcpy(desafio->textoDicaAvancada, "* Dica: O numero e MENOR QUE 50"); break;
            case DICA_SOMA_DIGITOS_MAIOR_10: strcpy(desafio->textoDicaAvancada, "* Dica: A soma dos digitos e MAIOR QUE 10"); break;
            default: break;
        }
    }

    // Ao zerar tentativas, oferece 2 tentativas extras que custam 1 vida.
    if (desafio->tentativasRestantes <= 0) {
        if (!desafio->usouUltimaChance && jogador->hp > 0) {
            desafio->estado = DESAFIO_CONFIRMAR_ULTIMA;
        } else {
            finalizarDesafio(desafio, false);
        }
    }
}

// Lida com digitacao, backspace, enter e clique no botao de envio do modal.
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

// Desenha os cinco indicadores visuais de tentativas restantes.
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
    Rectangle modal = {(GetScreenWidth() - 360) / 2.0f, 60, 360, 350};
    Rectangle entrada = {modal.x + 27, modal.y + 206, 306, 38};
    Rectangle enviar = {modal.x + 27, modal.y + 254, 306, 42};

    DrawRectangleRec(modal, (Color){18, 18, 18, 248});
    DrawRectangleLinesEx(modal, 2.0f, WHITE);
    drawAvisoIcone((int)modal.x + 34, (int)modal.y + 36);
    DrawText("DESAFIO CRITICO", (int)modal.x + 54, (int)modal.y + 29, 16, WHITE);
    DrawText("Adivinhe: 1 a 100", (int)modal.x + 27, (int)modal.y + 66, 14, WHITE);
    drawTentativas(desafio, modal);

    int textY = modal.y + 96;

    if (desafio->mensagemSistema[0] != '\0') {
        DrawText(desafio->mensagemSistema,
                 (GetScreenWidth() - MeasureText(desafio->mensagemSistema, 14)) / 2,
                 textY, 14, RED);
        textY += 18;
    }
    
    if (desafio->textoDistancia[0] != '\0') {
        DrawText(desafio->textoDistancia,
                 (GetScreenWidth() - MeasureText(desafio->textoDistancia, 16)) / 2,
                 textY, 16, desafio->corDistancia);
        textY += 20;
    }
    
    if (desafio->textoDirecao[0] != '\0') {
        DrawText(desafio->textoDirecao,
                 (GetScreenWidth() - MeasureText(desafio->textoDirecao, 14)) / 2,
                 textY, 14, LIGHTGRAY);
        textY += 18;
    }
    
    if (desafio->textoIntervalo[0] != '\0') {
        DrawText(desafio->textoIntervalo,
                 (GetScreenWidth() - MeasureText(desafio->textoIntervalo, 14)) / 2,
                 textY, 14, LIGHTGRAY);
        textY += 18;
    }
    
    if (desafio->textoDicaAvancada[0] != '\0') {
        DrawText(desafio->textoDicaAvancada,
                 (GetScreenWidth() - MeasureText(desafio->textoDicaAvancada, 14)) / 2,
                 textY, 14, SKYBLUE);
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
        DrawText("Sacrificar", (int)modal.x + 51, (int)modal.y + 108, 13, WHITE);
        DrawText("1 vida", (int)modal.x + 150, (int)modal.y + 108, 13, RED);
        DrawText("para receber", (int)modal.x + 207, (int)modal.y + 108, 13, WHITE);
        drawTextoCentralizado("mais 2 tentativas?", (int)modal.y + 126, 13, WHITE);
    } else {
        drawTextoCentralizado("Sem vidas para tentar de novo.", (int)modal.y + 112, 13, WHITE);
    }

    bool simSelecionado = (desafio->opcaoSelecionada == 0) && (jogador->hp > 0);
    bool naoSelecionado = (desafio->opcaoSelecionada == 1) || (jogador->hp <= 0);

    Color simBg = simSelecionado ? YELLOW : (Color){40, 40, 40, 255};
    Color simText = simSelecionado ? BLACK : RAYWHITE;
    Color simBorda = simSelecionado ? WHITE : DARKGRAY;

    Color naoBg = naoSelecionado ? YELLOW : (Color){40, 40, 40, 255};
    Color naoText = naoSelecionado ? BLACK : RAYWHITE;
    Color naoBorda = naoSelecionado ? WHITE : DARKGRAY;

    if (jogador->hp <= 0) {
        simBg = DARKGRAY;
        simText = GRAY;
        simBorda = DARKGRAY;
    }

    DrawRectangleRec(sim, simBg);
    DrawRectangleLinesEx(sim, simSelecionado ? 2.0f : 1.0f, simBorda);
    DrawText(simSelecionado ? "> SIM" : "  SIM", (int)sim.x + 40, (int)sim.y + 14, 14, simText);

    DrawRectangleRec(nao, naoBg);
    DrawRectangleLinesEx(nao, naoSelecionado ? 2.0f : 1.0f, naoBorda);
    DrawText(naoSelecionado ? "> NAO" : "  NAO", (int)nao.x + 40, (int)nao.y + 14, 14, naoText);
}

// Resultado final antes de voltar ao combate: acerto em verde, erro em vermelho.
static void drawTelaResultado(const DesafioPergunta* desafio) {
    Color borda = desafio->acertou ? GREEN : RED;
    Color fundo = desafio->acertou ? (Color){8, 45, 24, 215} : (Color){45, 8, 8, 215};
    Rectangle modal = getModalCentral(620.0f, 240.0f);

    DrawRectangleRec(modal, fundo);
    DrawRectangleLinesEx(modal, 3.0f, borda);

    if (desafio->acertou) {
        if (desafio->buffAplicado) {
             drawTextoCentralizadoNoModal("BUFF RECEBIDO!", modal, (int)modal.y + 60, 32, YELLOW);
             drawTextoCentralizadoNoModal(desafio->bonus, modal, (int)modal.y + 110, 26, desafio->corBonus);
        } else {
             drawTextoCentralizadoNoModal("NENHUM BUFF APLICADO", modal, (int)modal.y + 70, 24, LIGHTGRAY);
             drawTextoCentralizadoNoModal(desafio->bonus, modal, (int)modal.y + 110, 20, GRAY);
        }
    } else {
        drawTextoCentralizadoNoModal("X", modal, (int)modal.y + 34, 42, borda);
        drawTextoCentralizadoNoModal("Nao foi dessa vez!!", modal, (int)modal.y + 88, 16, borda);
        drawTextoCentralizadoNoModal(TextFormat("O numero era %d.", desafio->questao.numeroSecreto), modal, (int)modal.y + 126, 16, WHITE);
        drawTextoCentralizadoNoModal("Procure o proximo atributo e tente de", modal, (int)modal.y + 170, 15, WHITE);
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

// Sorteia o numero alvo do desafio dentro do intervalo configurado.
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

// API publica do modulo de estrela e desafio.

// Zera a estrela coletavel para que ela espere o proximo intervalo de spawn.
void inicializarEstrela(Estrela* estrela) {
    estrela->x            = 0;
    estrela->y            = 0;
    estrela->ativa        = false;
    estrela->timerAparição = 0.0f;
}

// Retorna o desafio para o estado inativo padrao, limpando entradas e timers.
static TipoDicaAvancada sortearDicaAvancada(int numero) {
    int sum = 0;
    int temp = numero;
    while (temp > 0) {
        sum += temp % 10;
        temp /= 10;
    }

    int opcoes[6];
    int count = 0;

    if (numero % 2 == 0) opcoes[count++] = DICA_PAR;
    if (numero % 2 != 0) opcoes[count++] = DICA_IMPAR;
    if (numero % 5 == 0) opcoes[count++] = DICA_MULTIPLO_5;
    if (numero > 50) opcoes[count++] = DICA_MAIOR_50;
    if (numero < 50) opcoes[count++] = DICA_MENOR_50;
    if (sum > 10) opcoes[count++] = DICA_SOMA_DIGITOS_MAIOR_10;

    if (count == 0) return DICA_NENHUMA;
    return opcoes[GetRandomValue(0, count - 1)];
}

void inicializarDesafio(DesafioPergunta* desafio) {
    desafio->estado = DESAFIO_INATIVO;
    desafio->questao.numeroSecreto = 0;
    desafio->questao.status = correto;
    desafio->tentativasRestantes = DESAFIO_MAX_TENTATIVAS;
    desafio->ultimoPalpite = 0;
    desafio->chutesTerminadosEmZero = 0;
    limparEntradaDesafio(desafio);
    
    desafio->mensagemSistema[0] = '\0';
    desafio->textoDirecao[0] = '\0';
    desafio->textoDistancia[0] = '\0';
    desafio->textoIntervalo[0] = '\0';
    desafio->textoDicaAvancada[0] = '\0';
    desafio->corDistancia = BLANK;
    desafio->limiteInferior = NUMEROMIN;
    desafio->limiteSuperior = NUMEROMAX;
    desafio->tipoDicaAvancada = DICA_NENHUMA;
    
    desafio->bonus = "";
    desafio->corBonus = BLANK;
    desafio->buffAplicado = false;
    
    desafio->acertou = false;
    desafio->usouUltimaChance = false;
    desafio->opcaoSelecionada = 1;
    desafio->timerResultado = 0.0f;
    desafio->timerContagem = 0.0f;
}

// Abre um novo desafio ativo e sorteia o numero secreto da rodada.
void iniciarDesafio(DesafioPergunta* desafio) {
    inicializarDesafio(desafio);
    desafio->estado = DESAFIO_CHUTANDO;
    desafio->questao.numeroSecreto = gerarNumeroSecreto();
    desafio->tipoDicaAvancada = sortearDicaAvancada(desafio->questao.numeroSecreto);
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
            if (jogador->hp <= 0) {
                finalizarDesafio(desafio, false);
                break;
            }

            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
                desafio->opcaoSelecionada = 0; // SIM
            }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                desafio->opcaoSelecionada = 1; // NAO
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (desafio->opcaoSelecionada == 0) {
                    aplicarDanoPlayer(jogador, 1);
                    desafio->estado = DESAFIO_CHUTANDO;
                    desafio->tentativasRestantes = 2;
                    desafio->usouUltimaChance = true;
                    strcpy(desafio->mensagemSistema, "Mais 2 tentativas!");
                    limparEntradaDesafio(desafio);
                } else {
                    finalizarDesafio(desafio, false);
                }
            } else if (botaoClicado(botaoSim)) {
                aplicarDanoPlayer(jogador, 1);
                desafio->estado = DESAFIO_CHUTANDO;
                desafio->tentativasRestantes = 2;
                desafio->usouUltimaChance = true;
                strcpy(desafio->mensagemSistema, "Mais 2 tentativas!");
                limparEntradaDesafio(desafio);
            } else if (botaoClicado(botaoNao)) {
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
        // Conta o timer ate a proxima aparicao
        estrela->timerAparição += deltaTime;
        if (estrela->timerAparição >= ESTRELA_INTERVALO) {
            // Spawna no centro do boss
            estrela->x = boss->posicaoX + boss->largura / 2.0f;
            estrela->y = boss->posicaoY + boss->altura / 2.0f;
            estrela->ativa = true;
            estrela->timerAparição = 0.0f;
        }
        return false;
    }

    // Move a estrela para baixo
    estrela->y += ESTRELA_VELOCIDADE * deltaTime;

    // Saiu da tela? Desativa e reinicia o timer
    if (estrela->y - ESTRELA_RAIO > GetScreenHeight()) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return false;
    }

    // Verifica colisao com o jogador (circulo vs circulo)
    float dx  = estrela->x - jogador->posicaoX;
    float dy  = estrela->y - jogador->posicaoY;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < ESTRELA_RAIO + PLAYER_RADIUS) {
        estrela->ativa = false;
        estrela->timerAparição = 0.0f;
        return true; // colidiu!
    }

    return false;
}

void drawEstrela(const Estrela* estrela) {
    if (!estrela->ativa) return;

    // Brilho externo (halo)
    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 6, (Color){255, 230, 50, 60});
    DrawCircle((int)estrela->x, (int)estrela->y,
               ESTRELA_RAIO + 3, (Color){255, 220, 80, 100});

    // Corpo da estrela
    desenharEstrelaForma(estrela->x, estrela->y,
                         ESTRELA_RAIO, ESTRELA_RAIO * 0.45f,
                         ESTRELA_PONTAS, YELLOW);

    // Centro brilhante
    DrawCircle((int)estrela->x, (int)estrela->y, 4, WHITE);
}
