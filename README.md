

# 🎮 MindDrop

<div align="center">

![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge)
![Disciplina](https://img.shields.io/badge/Disciplina-FDS-orange?style=for-the-badge)

</div>

---

## 📋 Sobre o Projeto

MindDrop é um jogo em C inspirado em clássicos como Space Invaders, combinando combate em arena com mecânicas de adivinhação.

O jogador enfrenta fases e um **Boss** final, desviando de ataques e usando projéteis para sobreviver ao confronto. Durante a partida, surgem **power-ups** baseados em desafios de adivinhação; o sistema de dicas informa se o palpite está acima ou abaixo do número secreto, influenciando diretamente o desempenho em combate.

Ao final, o jogo gera um relatório com médias de acertos e sugestões de estratégia para ajudar o jogador a encontrar o número mais rápido e melhorar sua tomada de decisão durante as batalhas.

### 🎮 Funcionalidades Principais

- Combate inspirado em clássicos como Space Invaders
- Enfrentamento de boss como objetivo principal da partida
- Sistema de adivinhação ativado por power-ups
- Feedback a cada tentativa com dicas de maior ou menor
- Disparo de projéteis
- Sistema de vidas
- Fases e bosses com padrões de ataque variados
- Sistema de dano e hitbox
- Drop de atributos
- Buffs temporários
- Tempo limite para derrotar o chefe
- Relatório final com médias de acerto e sugestões de estratégia

---

## 🛠️ Tecnologias Utilizadas

- **Linguagem:** C
- **Execução Oficial:** Docker / Docker Compose
- **Controle de Versão:** Git / GitHub

---

## 📁 Estrutura do Projeto

```text
MindDrop/
├── src/                         # Código-fonte do jogo
├── include/                     # Headers compartilhados
├── resources/                   # Assets (sprites, áudio, histórico)
├── build/                       # Premake, ECC e artefatos de build
├── docker/                      # Dockerfile e docker-compose.yml
├── docs/                        # Documentação do projeto
│   ├── CONTRIBUTING.md.docx
│   └── DOCUMENTACAO_DESIGN_FASE_LOGICA.md
├── scripts/                     # Automação (build, execução, Docker)
│   ├── build-MinGW-W64.bat
│   ├── build-VisualStudio2022.bat
│   ├── build-VisualStudio2026.bat
│   ├── run-docker.bat
│   ├── run-game.bat
│   ├── run-headless.bat
│   ├── stop-vcxsrv.bat
│   ├── start_game.ps1
│   └── stop_vcxsrv.ps1
└── README.md
```

---

## 👥 Equipe e Responsabilidades

| Integrante                               | Papel         | Histórias de Usuário                                                                               |
| ---------------------------------------- | ------------- | -------------------------------------------------------------------------------------------------- |
| **Allan Max de Jesus Rodrigues de Lima** | Desenvolvedor | [UH1] Movimentação (Base essencial) · [UH7] Regras de Tentativa (Controle da mecânica)             |
| **Boniek Araujo dos Santos Junior**      | Desenvolvedor | [UH2] Disparo de Projéteis (Interação principal) · [UH8] Drop de Atributos (Recompensa/Progressão) |
| **Caio César Leandro Amorim**            | Desenvolvedor | [UH3] Sistema de Vidas (Estado do jogador) · [UH9] Aplicação de Buffs (Efeito dos drops)           |
| **Luan Ventura Ferreira de Moura**       | Desenvolvedor | [UH4] Boss/Vilão Principal · [UH5] Sistema de Dano ao Boss (Objetivo de combate)                   |
| **Miguel Victor Lussac Barboza**         | Desenvolvedor | [UH6] Sistema de Advinhação (Mecânica específica)                                                  |
| **Pedro Augusto Carvalho Araujo**        | Desenvolvedor | [UH10] Sistema de Tempo e Vitória (Contexto da partida)                                            |

Todas as histórias de usuário de `UH1` a `UH10` foram incluídas e associadas a um integrante.

## 🤝 Programação em Par (Pair Programming)

Na Entrega 03 a equipe trabalhou em duplas, com uma pessoa codando (*driver*) e a
outra revisando e dando ideia (*navigator*), revezando os papéis ao longo da sprint.

<details>
  <summary>Programando em par</summary>

  https://github.com/user-attachments/assets/004eabb1-de91-4398-83bb-431249215578

</details>

| Dupla | Funcionalidades em par | Como foi |
|-------|------------------------|----------|
| **Luan & Pedro** | [UH4] Boss · [UH5] Dano ao Boss · [UH10] Tempo e Vitória | Luan focou no boss e no dano, Pedro no tempo e na vitória. O difícil foi acertar a hitbox e parar o cronômetro na hora certa. Aprendemos a usar struct e checar colisão. |
| **Allan & Boniek** | [UH1] Movimentação · [UH2] Disparo · [UH7] Regras de Tentativa · [UH8] Drop | Allan na movimentação e nas regras de tentativa, Boniek nos tiros e nos drops. O chato foi controlar vários tiros sem travar. Aprendemos a usar vetor pros projéteis. |
| **Caio & Miguel** | [UH3] Vidas · [UH6] Adivinhação · [UH9] Buffs | Miguel na adivinhação, Caio nas vidas e buffs. Confundiu ligar o palpite ao buff e à vida. Aprendemos a comparar o palpite com o número sorteado e aplicar o efeito. |
| **Luan & Pedro** | [UH4] Boss · [UH2] Disparo de Projéteis | Luan trabalhou no design do boss e do cenário enquanto Pedro ajustou os visuais dos projéteis. O desafio foi manter uma identidade visual consistente entre os elementos do jogo. Aprendemos a integrar melhor os assets gráficos ao projeto. |
| **Allan & Boniek** | [UH4] Boss · [UH2] Disparo de Projéteis | Allan focou na movimentação do boss e Boniek nos padrões de disparo. O difícil foi equilibrar o comportamento do inimigo para deixá-lo desafiador sem ficar injusto. Aprendemos a estruturar comportamentos mais complexos utilizando condições e estados. |
| **Caio & Miguel** | [UH8] Drop de Atributos · [UH9] Aplicação de Buffs | Caio implementou melhorias no sistema de drops enquanto Miguel realizou testes e ajustes nos efeitos recebidos pelo jogador. O desafio foi manter uma distribuição equilibrada dos atributos durante a partida. Aprendemos a validar mecânicas de recompensa e progressão. |
| **Luan & Pedro** | [UH10] Sistema de Tempo e Vitória | Luan ajustou os controles de áudio enquanto Pedro revisou os testes. O desafio foi garantir que o volume da música fosse aplicado corretamente durante toda a partida. Aprendemos a organizar melhor as configurações de som do jogo. |
| **Allan & Boniek** | [UH1] Movimentação | Allan trabalhou na movimentação e Boniek ajudou na validação dos cenários. O difícil foi identificar por que o personagem não conseguia alcançar um dos cantos da tela. Aprendemos a revisar os limites de movimentação e as condições aplicadas ao jogador. |
| **Caio & Miguel** | [UH6] Sistema de Adivinhação · [UH7] Regras de Tentativa | Miguel revisou as condições existentes e Caio implementou os ajustes necessários. O desafio foi encontrar expressões lógicas que não representavam corretamente as regras do jogo. Aprendemos a simplificar condições para deixar o código mais claro e confiável. |

---

<details>
  <summary>[UH1] Movimentação (Base essencial)</summary>
  <img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/44784067-98fd-4562-bc89-a39a38212d9a" />
  
  https://github.com/user-attachments/assets/5bb0b310-bd51-45e4-b018-b2a005043705
  
</details>

<details>
  <summary>[UH2] Disparo de Projéteis (Interação principal)</summary>
  <img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/da935e4e-ea50-49d3-b1ae-e8d50aa6a2b9" />

  https://github.com/user-attachments/assets/4af2258e-7847-4851-94d2-a9d6195d88a4

</details>

<details>
  <summary>[UH3] Sistema de Vidas (Estado do jogador)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/a4d5e108-14e3-4dc6-adf1-bec800510df7" />

  https://github.com/user-attachments/assets/420a8bcd-2deb-42c5-ab94-56ac1ba3b7e5

</details>

<details>
  <summary>[UH4] Boss/Vilão Principal</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/af8ee370-d0fb-4268-92f4-a40d037c0d46" />

  https://github.com/user-attachments/assets/105c4c23-512f-44b0-8fef-fd71507647f1

</details>

<details>
  <summary>[UH5] Sistema de Dano ao Boss (Objetivo de combate)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/562d2961-41b0-4e1d-80a0-b39926abcf17" />

  https://github.com/user-attachments/assets/b46b356c-cb24-4294-ad86-4ac342ebb3c4

</details>

<details>
  <summary>[UH6] Sistema de Advinhação (Mecânica específica)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/021d4c0f-eb63-45b8-9b92-a7eecf4e2f4b" />
</details>

<details>
  <summary>[UH7] Regras de Tentativa (Controle da mecânica)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/d7fc5581-c036-4c9f-a050-d3f302324b48" />
</details>

<details>
  <summary>[UH8] Drop de Atributos (Recompensa/Progressão)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/b2120d64-e3d0-4aa7-9292-8ace5483c959" />
</details>

<details>
  <summary>[UH9] Aplicação de Buffs (Efeito dos drops)</summary>
  <img width="655" height="412" alt="image" src="https://github.com/user-attachments/assets/5884a30b-f8b8-453a-9f47-a55a412ac026" />
</details>

<details>
  <summary>[UH10] Sistema de Tempo e Vitória (Contexto da partida)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/cbf03a45-ae3d-4ea4-9ceb-85d514654590" />
</details>

---

## 📊 Quadro de Atividades / Backlog (Kanban)

<div align="center">

<img width="1360" height="630" alt="Captura de tela 2026-06-09 212458" src="https://github.com/user-attachments/assets/21045409-5c1b-4a95-b582-9fb11e8678db" />

## </div>

## ✍️ Protótipo de Baixa Fidelidade

<div align="center">
  <img width="543" height="898" alt="image" src="https://github.com/user-attachments/assets/6e439fc7-cb4d-4a33-8126-c64436384351" />

</div>

---

## 🐛 Bug Tracking

Durante o desenvolvimento, a equipe identificou e corrigiu os seguintes problemas críticos do sistema:

### 1. Limites de Tela do Jogador (Commit `3cf03bd`)
* **Problema:** O jogador conseguia "fugir" da tela na lateral direita ou ficava invisível, limitando sua área útil a apenas uma parte da janela do jogo.
* **Causa:** A janela oficial do jogo foi atualizada para a resolução `1280x720`. No entanto, a lógica de restrição de movimento em `src/player/player.c` continuava usando constantes estáticas baseadas no layout antigo de `800x600` (`SCREEN_WIDTH = 800`), impedindo o jogador de aproveitar os novos 480 pixels à direita.
* **Solução:** Substituição das constantes físicas no cálculo de limites por chamadas dinâmicas a `GetScreenWidth()` e `GetScreenHeight()` da biblioteca Raylib. Isso corrigiu tanto a área de movimentação quanto o posicionamento proporcional da barra de vida.

### 2. Contador de Buffs no Histórico (Commit `bd63632`)
* **Problema:** Mesmo após coletar múltiplos power-ups/buffs na partida, o feed de histórico no painel do jogo sempre exibia o valor de buffs como zero.
* **Causa:** O parser em `src/main.c` (`parseLinhaHistorico`) usava a expressão `lidosArcade >= 10` para validar se a leitura do arquivo tinha sido correta. Porém, o campo que continha os buffs era o 11º parâmetro adicionado na evolução do projeto, fazendo com que ele nunca fosse considerado na verificação principal de sucesso do parser. O histórico também ordenava os dados incorretamente, jogando as partidas mais recentes no fundo da pilha.
* **Solução:** Correção do limitador de leitura para exigir pelo menos 11 campos e implementação de um fallback que define buffs como `0` em arquivos de histórico antigos. A ordenação também foi corrigida para exibir o histórico em formato decrescente (partidas mais recentes no topo).

### 3. Exibição de Estatísticas no Relatório Analítico (Commit `044ba22`)
* **Problema:** O relatório analítico de desempenho exibia apenas traços estáticos (`"--"`) e heurísticas genéricas de placeholder, não refletindo o real histórico do jogador.
* **Causa:** A função `renderizarRelatorioAnalitico` era alimentada com o valor `NULL` em `src/main.c` (fazendo com que a tela nunca recebesse a struct de dados). Além disso, a implementação interna em `src/analysis/analise.c` utilizava um array estático de strings e heurísticas pré-definidas para renderização, em vez de tratar as variáveis numéricas processadas do jogador.
* **Solução:** Criação da rotina `recarregarAnaliseHistorico()` no fluxo principal do jogo para compilar as estatísticas em tempo de execução, passagem do ponteiro do relatório estruturado à tela e reformulação da renderização em `src/analysis/analise.c` para mapear dinamicamente as métricas com `snprintf` e processar a lista de heurísticas inteligentes geradas a partir do comportamento do usuário.

---

## 🎥 Screencast

https://github.com/user-attachments/assets/3c0d756f-70e4-45d3-825e-4c217d2ece6c

---

## Apresentação MindDrop!

https://canva.link/j5a81oh0nxyvdzt

---

# Vem testar o MindDrop!

https://mind-drop.netlify.app/

---

## 🚀 Como Rodar o Projeto

### Forma oficial (única): Docker

Para evitar problemas de ambiente local (compilador, dependências gráficas, PATH e versões), a execução oficial do projeto é **exclusivamente via Docker**.

### Pré-requisitos

- Docker Desktop

#### 1) Build da imagem

```bash
docker compose -f docker/docker-compose.yml build
```

#### 2) Rodar o jogo

```bash
docker compose -f docker/docker-compose.yml run --rm minddrop
```

### Executar com interface gráfica no Windows (passo a passo completo)

Para a janela do jogo aparecer via Docker no Windows, siga exatamente estes passos:

#### Passo 1) Instalar o VcXsrv

```powershell
winget install --id marha.VcXsrv -e
```

#### Passo 2) Iniciar o servidor X

Opção por comando (recomendada):

```powershell
"C:\Program Files\VcXsrv\vcxsrv.exe" :0 -multiwindow -ac -nowgl -silent-dup-error
```

Opção por interface (`XLaunch`):
- `Multiple windows`
- `Start no client`
- marcar `Disable access control`
- finalizar o assistente

#### Passo 3) Liberar no Firewall

Quando o Windows pedir permissão para o VcXsrv, permita em rede **Privada** (e **Pública**, se necessário).

#### Passo 4) Confirmar que o VcXsrv está rodando

No PowerShell:

```powershell
Get-Process vcxsrv
```

Se listar processo, está ativo.

#### Passo 5) Rodar o jogo via Docker com GUI

```bash
docker compose -f docker/docker-compose.yml run --rm minddrop
```

#### Passo 6) Se der erro de display

Se aparecer `Failed to open display host.docker.internal:0.0`:
- feche e abra o VcXsrv novamente;
- confirme permissão no firewall;
- repita o comando de execução.

#### Passo 7) Validação alternativa (sem GUI)

Se quiser validar rapidamente que o jogo iniciou no container:

```bash
docker compose -f docker/docker-compose.yml run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
```

Procure no log: `INFO: Initializing raylib`.

#### 2.1) Atalho para Windows (build + teste + run)

```bat
scripts\run-docker.bat
```

O script executa automaticamente:
- build da imagem Docker
- teste headless no container (`xvfb`)
- inicialização do jogo via Docker

Se nao houver servidor X ativo no host, a etapa de janela pode falhar, mas o script
vai concluir com sucesso caso build + teste headless estejam OK.

#### 3) Teste headless (validação rápida no container)

Esse teste confirma que o binário inicia corretamente sem depender de janela interativa:

```bash
docker compose -f docker/docker-compose.yml run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
```

> Procure no log mensagens como `INFO: Initializing raylib` para confirmar que o jogo iniciou no container.

#### Observações sobre display (janela gráfica)

- **Linux**: exporte `DISPLAY` e permita acesso X11 se necessário.
- **Windows (Docker Desktop)**: use um servidor X (ex.: VcXsrv) e mantenha `DISPLAY=host.docker.internal:0.0`.
- Se quiser apenas validar build/start no CI, use o modo headless com `xvfb-run`.

---

## 📜 Licença

Este projeto foi desenvolvido para fins acadêmicos. Todos os direitos reservados aos integrantes da equipe.

---

<div align="center">

Desenvolvido com 💻 pela equipe · Projeto Interdisciplinar · 2025

</div>
