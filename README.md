# 🎮 MindDrop

<div align="center">

![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge)
![Disciplina](https://img.shields.io/badge/Disciplina-FDS-orange?style=for-the-badge)
![Projeto](https://img.shields.io/badge/MindDrop-red)

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
- **Compilador:** Compatível com compiladores C11
- **Gerenciamento de Build:** Makefile
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

---

## 🚀 Como Compilar e Executar

### Pré-requisitos

- Windows com uma das opções abaixo:
- MinGW-w64 com `gcc` e `mingw32-make` no `PATH`
- Visual Studio 2022 ou Visual Studio 2026 com suporte a C/C++
- `premake5.exe` já incluído na pasta `build/`

> Observação: na primeira geração dos arquivos de build, o `premake` pode baixar automaticamente o código-fonte do `raylib` para `build/external/`.

### Compilação

#### Opção 1: MinGW-w64

```bash
# Instale MinGW / Compilador GCC/G++

Instalação do compilador GCC/G++

ALERTA: NAO COPIE OS ASTERISCOS!

1. No terminal (powershell do computador) digite: **winget install MSYS2.MSYS2**
2. Após instalação, abra o **MSYS2 UCRT64**
3. No MSYS2 UCRT64, digite o comando:  **pacman -S mingw-w64-ucrt-x86_64-gcc** e aceite os termos de instalação digitando **Y**
4. Inclua o **C:\msys64\ucrt64\bin** no **PATH** do sistema
5. Feche e abra o Terminal e verifique a instalação digitando: **gcc --version** e depois **g++ --version**
6. **Se por alguma razão você não consiga instalar o MSYS2 via gerenciador de pacotes winget, baixe o instalador oficial e instale o compilador, depois siga com os passos 2 a 5.**

### macOS e Linux

Dispositivos com macOS e Linux já vem de fábrica com o compilador GCC instalado.

# Clonar o repositório já com o nome local do projeto
git clone https://github.com/MiguelLussac/JogoEmC.git MindDrop
cd MindDrop

# Gerar os makefiles com o Premake
cd build
premake5.exe gmake
cd ..

# Compilar o projeto completo em modo debug usando o Makefile raiz
mingw32-make config=debug_x64

# Aperta `F5` (no vs-code)
```

Ou, no Windows, basta executar:

```bat
build-MinGW-W64.bat
```

#### Opção 2: Visual Studio

```bash
# Clonar o repositório já com o nome local do projeto
git clone https://github.com/MiguelLussac/JogoEmC.git MindDrop
cd MindDrop

# Gerar a solution do Visual Studio 2022
build-VisualStudio2022.bat
```

Se preferir Visual Studio 2026, use `build-VisualStudio2026.bat`.

Depois disso, abra `MindDrop.sln` na raiz do projeto e compile pela IDE.

### Execução

```bash
bin\Debug\MindDrop.exe
```

---

## 📜 Licença

Este projeto foi desenvolvido para fins acadêmicos. Todos os direitos reservados aos integrantes da equipe.

---

<div align="center">

Desenvolvido com 💻 pela equipe · Projeto Interdisciplinar · 2025

</div>
