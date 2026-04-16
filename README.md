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

| Integrante | Papel | Histórias de Usuário |
|---|---|---|
| **Miguel Victor Lussac Barboza** |  |  |
| **Boniek Araujo dos Santos Junior** |  |  |
| **Luan Ventura Ferreira de Moura** |  |  |
| **Pedro Augusto Carvalho Araujo** |  |  |
| **Allan Max de Jesus Rodrigues de Lima** |  |  |
| **Caio Cesar Leandro Amorim** |  |  |

---

📝 Histórias de Usuário (User Stories)

    10 histórias de usuário focadas no valor para o jogador:

    Movimentação: Eu, como jogador, quero conseguir movimentar o personagem pela ilha para explorar o cenário.

    Interação: Eu, como jogador, quero interagir com NPCs ou objetos para iniciar uma rodada de perguntas.

    Desafio de Perguntas: Eu, como jogador, quero visualizar uma pergunta e múltiplas escolhas para testar meus conhecimentos.

    Feedback de Erro/Acerto: Eu, como jogador, quero receber um feedback visual imediato se acertei ou errei a questão.

    Coleta de Itens: Eu, como jogador, quero que, ao acertar uma sequência de perguntas, um componente da canoa seja adicionado ao meu inventário.

    Visualização de Progresso: Eu, como jogador, quero ver o status da construção da canoa para saber quanto falta para terminar.

    Sistema de Vidas/Tentativas: Eu, como jogador, quero ter um limite de erros para que o jogo tenha um desafio real.

    Menu Principal: Eu, como jogador, quero um menu inicial para começar o jogo ou ver as instruções.

    Salvar Progresso (Opcional): Eu, como jogador, gostaria de ter meu progresso salvo para não perder os itens já coletados.

    Condição de Vitória: Eu, como jogador, quero uma tela de conclusão ao finalizar a canoa e conseguir escapar da ilha.

---

📊 Board e Backlog

(Espaço reservado para as imagens solicitadas no seu print)
Quadro Kanban (Print)
Backlog Priorizado (Print)
