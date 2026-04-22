# Jogo de Adivinhação 

![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge)
![Disciplina](https://img.shields.io/badge/Disciplina-FDS-orange?style=for-the-badge)
![Universidade](https://img.shields.io/badge/Projeto-Faculdade-purple?style=for-the-badge)

---

## Nome do Jogo

Esta aplicação consiste em um Jogo de Adivinhação Numérica elaborado em linguagem C, integrando as competências de diversas disciplinas do curso. O objetivo é descobrir um valor aleatório estipulado pelo sistema dentro de um limite pré-estabelecido. Durante a partida, o usuário é orientado com dicas sobre a magnitude do seu palpite em relação ao número oculto, prosseguindo até a conclusão do acerto ou o término das chances permitidas.

---

## 🎮 Funcionalidades Principais

- Menu interativo com opções de jogo
- Geração de número aleatório
- Feedback a cada tentativa (maior/menor)
- Limite de tentativas configurável
- Registro de recordes (High Score)
- Estatísticas de desempenho
- Persistência de dados entre sessões
- Tratamento robusto de erros de entrada
- Reinicialização de partida

---

## 🛠️ Ferramentas Utilizadas

- **Linguagem:** C

- **Gráficos/Interface:** Raylib

- **Prototipagem:** Figma

- **Editor de Código:** VS Code

- **Gestão de Tarefas:** Trello

- **Controle de Versão:** Git & GitHub

---

## 📁 Estrutura do Projeto

```
jogo-adivinhacao/
├── src/
│   ├── main.c           # Ponto de entrada e menu principal
│   ├── game.c           # Lógica central do jogo
│   ├── score.c          # Sistema de pontuação e recordes
│   ├── stats.c          # Estatísticas agregadas
│   ├── utils.c          # Funções utilitárias e recursão
│   └── io.c             # Tratamento de entrada/saída
├── include/
│   ├── game.h
│   ├── score.h
│   ├── stats.h
│   └── utils.h
├── data/
│   └── scores.dat       # Arquivo de persistência
├── Makefile
└── README.md
```

---

## Integrantes e Papéis

| Integrante | Contato | Papel | Histórias de Usuário |
|---|---|---|---|
| **Miguel Victor Lussac Barboza** | www.linkedin.com/in/miguellussac | ScrumMaster/Desenvolvedor | **UH1** - Movimentação (Base essencial) |
| **Boniek Araujo dos Santos Junior** | www.linkedin.com/in/boniek-junior-561448281 | Desenvovedor | **UH2** - Disparo de Projéteis (Interação principal) & **UH7** - Regras de Tentativa (Controle da mecânica) |
| **Luan Ventura Ferreira de Moura** | www.linkedin.com/in/luanven | Desenvolvedor/ QA | **UH3** - Sistema de Vidas (Estado do jogador) |
| **Pedro Augusto Carvalho Araujo** | www.linkedin.com/in/pedro-carvalho-429945257 | Desenvolvedor/ QA | **UH4** - Boss/Vilão Principal & **UH8** - Drop de Atributos (Recompensa/Progressão) |
| **Allan Max de Jesus Rodrigues de Lima** | www.linkedin.com/in/allan-max-jrl | Desenvolvedor | **UH5** - Sistema de Dano ao Boss (Objetivo de combate) & **UH9** - Aplicação de Buffs (Efeito dos drops)|
| **Caio Cesar Leandro Amorim** | www.linkedin.com/in/caio-cesar-b11b3b373 | Desenvolvedor/ Analista | **UH6** - Sistema de Advinhação (Mecânica específica) & **UH10** - Sistema de Tempo e Vitória(Contexto da partida)|

---

📊 Board e Backlog

(Espaço reservado para as imagens solicitadas no seu print)
Quadro Kanban (Print)
Backlog Priorizado (Print)

<img width="1144" height="567" alt="Captura de tela 2026-04-22 153128" src="https://github.com/user-attachments/assets/af142648-743a-47bd-963a-fe20cb71b9c3" />
