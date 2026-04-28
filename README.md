# 🎮 Jogo de Ação e Adivinhação em C

<div align="center">

![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge)
![Disciplina](https://img.shields.io/badge/Disciplina-FDS-orange?style=for-the-badge)
![Universidade](https://img.shields.io/badge/Projeto-Faculdade-purple?style=for-the-badge)

</div>

---

## 📋 Sobre o Projeto

Este projeto é um jogo em C inspirado em clássicos como Space Invaders, combinando combate em arena com mecânicas de adivinhação.

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

```
raylib-quickstart/
├── src/
│   ├── main.c                   # Ponto de entrada e loop principal do jogo
│   ├── application.rc           # Recursos da aplicação no Windows
│   └── icon.ico                 # Ícone da aplicação
├── include/
│   └── resource_dir.h           # Utilitário para localizar a pasta de recursos
├── resources/                   # Pasta com imagens, sprites e recursos do projeto
|                                  
|
├── build/
│   ├── premake5                 # Binário/script de build
│   ├── premake5.exe             # Executável do Premake para Windows
│   ├── premake5.lua             # Configuração de geração de projeto
│   ├── premake5.osx             # Script de build para macOS
│   └── ecc/                     # Dependências auxiliares do sistema de build
├── build-MinGW-W64.bat          # Build para MinGW no Windows
├── build-VisualStudio2022.bat   # Build para Visual Studio 2022
├── build-VisualStudio2026.bat   # Build para Visual Studio 2026
├── raylib.make                  # Configuração de build baseada em make
├── raylib-quickstart.make       # Makefile do template base
└── README.md
```

---

## 👥 Equipe e Responsabilidades

| Integrante | Papel | Histórias de Usuário |
|---|---|---|
| **Vinicius Pessoa de Albuquerque** | Tech Lead / Arquiteto | [UH3] Ciclo de Jogo com Feedback · [UH10] Cálculos Base (Recursão) |
| **Pedro Pessoa de Albuquerque Neto** | Desenvolvedor / QA | [UH8] Tratamento de Erros |
| **Roberto Henrique Cavalcanti Freitas** | Desenvolvedor Back-end | [UH7] Persistência de Dados · [UH11] Recorde Local (High Score) |
| **Saulo Eduardo Almeida dos Santos** | Desenvolvedor | [UH12] Estatísticas Agregadas |
| **Thayna Vercosa de Andrade** | Desenvolvedora / Product Owner | [UH6] Limite de Tentativas |
| **Thiago Cardozo da Conceição** | Desenvolvedor / Analista | [UH13] Heurística de Estratégia |
| **Vinicius Wagner Gomes Germano** | Desenvolvedor Full Stack | [UH1] Interface Principal (Menu) · [UH2] Geração de Número |
| **Vitoria Gabrielly Gomes da Silva** | Desenvolvedora / QA | [UH5] Condição de Vitória | 
| **Wesley Yuri da Silva** | Desenvolvedor / Analista | [UH9] Reinicialização · [UH14] Leitura e Parsing do Histórico |
| **Yasmin Karolina Silva de M. Godinho** | Desenvolvedora / QA | [UH4] Palpite Básico |

---

## 📊 Quadro de Atividades / Backlog (Kanban)

<div align="center">

<!-- ✅ SUBSTITUA A LINHA ABAIXO PELO PRINT DO QUADRO -->
<img width="1913" height="964" alt="image" src="https://github.com/user-attachments/assets/3bd919ed-8366-46d0-a0b0-fee3c7881860" />

*Print do quadro de atividades da equipe — atualizado em: `09/04/2026`*

</div>
---
## ✍️ Protótipo de Baixa Fidelidade
<div align="center">
  <img width="1600" height="742" alt="image" src="https://github.com/user-attachments/assets/da304619-05c3-4cac-b06d-e33b453837b7" />
  
  *Print do protótipo de baixa fidelidade do Figma — atualizado em: `16/04/2026`*
</div>

---
## 🎥 Demo

[![Demo](./assets/thumb.png)](./assets/demo.mp4)
---
## 🚀 Como Compilar e Executar

### Pré-requisitos

- GCC instalado (ou um compilador equivalente)  
- Sistema operacional: Linux, macOS ou Windows (com MinGW)

### Compilação

```bash
# Clonar o repositório
git clone https://github.com/seu-usuario/jogo-adivinhacao.git
cd jogo-adivinhacao

# Compilar com Makefile
make

# Ou compilar manualmente
gcc src/*.c -o jogo -Wall -Wextra -std=c99
```

### Execução

```bash
./jogo
```

---

## 📜 Licença

Este projeto foi desenvolvido para fins acadêmicos. Todos os direitos reservados aos integrantes da equipe.

---

<div align="center">

Desenvolvido com 💻 pela equipe · Projeto Interdisciplinar · 2025

</div>
