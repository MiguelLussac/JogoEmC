# Documentacao de Design: Fase Logica Proposicional

Projeto: MindDrop  
Motor/Tecnologia: Linguagem C e Raylib

## 1. Visao Geral do Modulo

A Fase Logica e um modo de jogo isolado ("Game State" alternativo) implementado no universo de MindDrop. O objetivo deste modulo e integrar os conceitos de Logica Proposicional de forma organica e frenetica a jogabilidade arcade.

Em vez de responder a questionarios estaticos (quizzes), o jogador deve realizar calculos logicos mentalmente em tempo real enquanto desvia e coleta elementos na tela, culminando em uma batalha estrategica contra um Boss.

## 2. Mecanica Central (Core Gameplay)

A mecanica baseia-se na coleta e combinacao de valores logicos (Premissas) para satisfazer uma condicao (Porta Logica + Objetivo) gerada pelo sistema.

1. A Nave e os Slots: O jogador controla uma nave na base da tela, que possui dois espacos vazios (slots).
2. A Condicao de Disparo: A cada rodada, a nave do jogador e configurada com:
   - Uma Porta Logica (`AND` ou `OR`).
   - Um Resultado Esperado (`Verdadeiro` ou `Falso`).
3. Chuva de Premissas (Drops): Do topo da tela, caem caixas representando valores booleanos: `V` (Verdadeiro, na cor Azul) e `F` (Falso, na cor Vermelha).
4. Resolucao: O jogador deve movimentar a nave para coletar dois blocos. Assim que os dois slots sao preenchidos, o "Motor Logico" do jogo avalia a expressao.

## 3. Logica Proposicional Explicita (Motor Logico)

A logica esta 100% explicita na tela e nas regras fisicas do jogo. A avaliacao ocorre da seguinte forma:

- Acerto (Sucesso Logico): Se a avaliacao da Porta Logica com os dois blocos coletados for igual ao Resultado Esperado, a nave dispara um laser que causa dano ao Boss.
- Erro (Falha Logica): Se o resultado obtido for diferente do Resultado Esperado (exemplo: o jogador precisava de um `Falso` numa porta `OR`, mas pegou `V` e `F`, resultando em `Verdadeiro`), a nave entra em curto-circuito e o jogador perde HP.

Exemplos praticos de gameplay (Tabela Verdade aplicada):

- Situacao A: Porta `AND` | Objetivo `VERDADEIRO`
  - Unica jogada valida: coletar `[ V ]` e `[ V ]`.
- Situacao B: Porta `OR` | Objetivo `FALSO`
  - Unica jogada valida: coletar `[ F ]` e `[ F ]`.
- Situacao C: Porta `AND` | Objetivo `FALSO`
  - Jogadas validas: coletar `[ V ]` e `[ F ]`, ou `[ F ]` e `[ V ]`, ou `[ F ]` e `[ F ]`.
  - Exige raciocinio rapido para notar que qualquer `F` resolve o problema.

## 4. Curva de Dificuldade e Progressao

Para garantir que o jogo seja justo e apresente uma curva de aprendizado escalavel, a inteligencia do Boss altera o comportamento do sorteio de objetivos com base na sua Vida (HP):

- Fase 1: Treinamento (HP do Boss de 100% a 60%)
  - O jogo sorteia aleatoriamente as portas `AND` e `OR`, mas o Objetivo sera sempre `Verdadeiro (V)`.
  - Proposito: condicionar o jogador a mecanica de movimentacao e coleta, ensinando o funcionamento basico da tabela verdade para resultados positivos.
- Fase 2: Sobrecarga Mental (HP do Boss abaixo de 60%)
  - O jogo continua sorteando as portas logicas, mas agora o Objetivo alterna aleatoriamente entre `Verdadeiro (V)` e `Falso (F)`.
  - Proposito: quebrar a expectativa do jogador. Pedir que o jogador alcance um resultado `Falso` forca a inversao do raciocinio logico em milissegundos, gerando o desafio principal (criatividade e jogabilidade).

## 5. Interface de Usuario e Feedback Visual (UI/UX)

O design visual foi planejado para minimizar a poluicao visual e focar na leitura rapida das variaveis logicas:

- HUD Superior (O Desafio): O topo da tela exibe em fonte grande a expressao atual. Cores reforcam o objetivo (Verde para buscar Verdade, Vermelho para buscar Falsidade).
  - Exemplo: `PORTA: AND | OBJETIVO: V`
- HUD da Nave (Os Slots): A nave exibe visualmente o que ja foi coletado.
  - Exemplo: se o jogador pegou o primeiro bloco verdadeiro, a nave exibe `[ V ] [ ? ]`, informando que falta apenas uma premissa para a resolucao.
- Drops Distintos: Cores de alto contraste distinguem facilmente os blocos que caem (Azul para V, Vermelho para F), permitindo decisoes baseadas em visao periferica.

## 6. Arquitetura Tecnica Integrada

A Fase Logica foi desenvolvida seguindo o principio de Single Responsibility (Responsabilidade Unica) e encapsulamento em C.

- Isolamento de Estado: Construida em arquivos separados (`logic_phase.h` e `logic_phase.c`), a mecanica opera como uma State Machine (Maquina de Estados) dentro da `main.c`.
- Seguranca do Projeto: O codigo original do MindDrop permanece intocado. O jogador pode transitar livremente entre o Jogo Principal e a Fase Logica mediante comandos de teclado (exemplo: tecla `L`), garantindo a estabilidade da apresentacao.
