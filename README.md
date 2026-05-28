

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
├── src/
│   ├── main.c                   # Ponto de entrada e loop principal do jogo
│   ├── application.rc           # Recursos da aplicação no Windows
│   └── icon.ico                 # Ícone da aplicação
├── include/
│   └── resource_dir.h           # Utilitário para localizar a pasta de recursos
├── resources/
│   ├── coracao.png              # Recurso gráfico adicional
│   ├── heart.png                # Sprite carregada no projeto
│   └── wabbit_alpha.png         # Imagem de exemplo do template
├── build/
│   ├── premake5                 # Binário/script de build
│   ├── premake5.exe             # Executável do Premake para Windows
│   ├── premake5.lua             # Configuração de geração de projeto
│   ├── premake5.osx             # Script de build para macOS
│   └── ecc/                     # Dependências auxiliares do sistema de build
├── build-MinGW-W64.bat          # Build para MinGW no Windows
├── build-VisualStudio2022.bat   # Build para Visual Studio 2022
├── build-VisualStudio2026.bat   # Build para Visual Studio 2026
├── Makefile                     # Orquestra a build completa do workspace
├── raylib.make                  # Configuração de build da biblioteca raylib
├── MindDrop.make                # Makefile gerado para o executável do jogo
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

| Dupla | Funcionalidades em par | Como foi |
|-------|------------------------|----------|
| **Luan & Pedro** | [UH4] Boss · [UH5] Dano ao Boss · [UH10] Tempo e Vitória | Luan focou no boss e no dano, Pedro no tempo e na vitória. O difícil foi acertar a hitbox e parar o cronômetro na hora certa. Aprendemos a usar struct e checar colisão. |
| **Allan & Boniek** | [UH1] Movimentação · [UH2] Disparo · [UH7] Regras de Tentativa · [UH8] Drop | Allan na movimentação e nas regras de tentativa, Boniek nos tiros e nos drops. O chato foi controlar vários tiros sem travar. Aprendemos a usar vetor pros projéteis. |
| **Caio & Miguel** | [UH3] Vidas · [UH6] Adivinhação · [UH9] Buffs | Miguel na adivinhação, Caio nas vidas e buffs. Confundiu ligar o palpite ao buff e à vida. Aprendemos a comparar o palpite com o número sorteado e aplicar o efeito. |


<details>
  <summary>[UH1] Movimentação (Base essencial)</summary>
  <img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/44784067-98fd-4562-bc89-a39a38212d9a" />
</details>

<details>
  <summary>[UH2] Disparo de Projéteis (Interação principal)</summary>
  <img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/da935e4e-ea50-49d3-b1ae-e8d50aa6a2b9" />
</details>

<details>
  <summary>[UH3] Sistema de Vidas (Estado do jogador)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/a4d5e108-14e3-4dc6-adf1-bec800510df7" />
</details>

<details>
  <summary>[UH4] Boss/Vilão Principal</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/af8ee370-d0fb-4268-92f4-a40d037c0d46" />
</details>

<details>
  <summary>[UH5] Sistema de Dano ao Boss (Objetivo de combate)</summary>
  <img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/562d2961-41b0-4e1d-80a0-b39926abcf17" />
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

<img width="1910" height="786" alt="image" src="https://github.com/user-attachments/assets/78fa20ae-4eaa-4425-b599-b8022b91e339" />

## </div>

## ✍️ Protótipo de Baixa Fidelidade

<div align="center">
  <img width="543" height="898" alt="image" src="https://github.com/user-attachments/assets/6e439fc7-cb4d-4a33-8126-c64436384351" />

</div>

---

## 🎥 Demo

https://github.com/user-attachments/assets/a79b4fae-119b-4530-aed5-d6dbb97d5350

---

## 🚀 Como Rodar o Projeto

### Forma oficial (única): Docker

Para evitar problemas de ambiente local (compilador, dependências gráficas, PATH e versões), a execução oficial do projeto é **exclusivamente via Docker**.

### Pré-requisitos

- Docker Desktop

#### 1) Build da imagem

```bash
docker compose build
```

#### 2) Rodar o jogo

```bash
docker compose run --rm minddrop
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
docker compose run --rm minddrop
```

#### Passo 6) Se der erro de display

Se aparecer `Failed to open display host.docker.internal:0.0`:
- feche e abra o VcXsrv novamente;
- confirme permissão no firewall;
- repita o comando de execução.

#### Passo 7) Validação alternativa (sem GUI)

Se quiser validar rapidamente que o jogo iniciou no container:

```bash
docker compose run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
```

Procure no log: `INFO: Initializing raylib`.

#### 2.1) Atalho para Windows (build + teste + run)

```bat
run-docker.bat
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
docker compose run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
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
