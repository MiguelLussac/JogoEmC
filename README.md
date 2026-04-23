# Jogo de Adivinhação 

![C](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-yellow?style=for-the-badge)
![Disciplina](https://img.shields.io/badge/Disciplina-FDS-orange?style=for-the-badge)
![Universidade](https://img.shields.io/badge/Projeto-Faculdade-purple?style=for-the-badge)

---

# MindDrop

Esta aplicação consiste em um Jogo de Adivinhação Numérica elaborado em linguagem C, integrando as competências de diversas disciplinas do curso. O objetivo é descobrir um valor aleatório estipulado pelo sistema dentro de um limite pré-estabelecido. Durante a partida, o usuário é orientado com dicas sobre a magnitude do seu palpite em relação ao número oculto, prosseguindo até a conclusão do acerto ou o término das chances permitidas.

---

# 🎮 Funcionalidades Principais

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

# 🛠️ Ferramentas Utilizadas

- **Linguagem:** C

- **Gráficos/Interface:** Raylib

- **Prototipagem:** Figma

- **Editor de Código:** VS Code

- **Gestão de Tarefas:** Trello

- **Controle de Versão:** Git & GitHub

---

# 📁 Estrutura do Projeto

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

# Integrantes e Papéis

| Integrante | Contato | Papel | Histórias de Usuário |
|---|---|---|---|
| **Miguel Victor Lussac Barboza** | www.linkedin.com/in/miguellussac | ScrumMaster/Desenvolvedor | **UH1** - Movimentação (Base essencial) |
| **Boniek Araujo dos Santos Junior** | www.linkedin.com/in/boniek-junior-561448281 | Desenvovedor | **UH2** - Disparo de Projéteis (Interação principal) & **UH7** - Regras de Tentativa (Controle da mecânica) |
| **Luan Ventura Ferreira de Moura** | www.linkedin.com/in/luanven | Desenvolvedor/ QA | **UH3** - Sistema de Vidas (Estado do jogador) |
| **Pedro Augusto Carvalho Araujo** | www.linkedin.com/in/pedro-carvalho-429945257 | Desenvolvedor/ QA | **UH4** - Boss/Vilão Principal & **UH8** - Drop de Atributos (Recompensa/Progressão) |
| **Allan Max de Jesus Rodrigues de Lima** | www.linkedin.com/in/allan-max-jrl | Desenvolvedor | **UH5** - Sistema de Dano ao Boss (Objetivo de combate) & **UH9** - Aplicação de Buffs (Efeito dos drops)|
| **Caio Cesar Leandro Amorim** | www.linkedin.com/in/caio-cesar-b11b3b373 | Desenvolvedor/ Analista | **UH6** - Sistema de Advinhação (Mecânica específica) & **UH10** - Sistema de Tempo e Vitória(Contexto da partida)|

---

# Diagramas de Atividades

## UH1 - Movimentação (Base essencial)
<img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/c07b0cb5-789c-4c59-85d5-2e4ca6bbf485" />

## UH2 - Disparo de Projéteis (Interação principal)
<img width="1535" height="1024" alt="image" src="https://github.com/user-attachments/assets/9001f8cb-5437-446a-9d17-e55256cdc876" />

## UH3 - Sistema de Vidas (Estado do jogador)
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/084541f4-4d52-45ab-8579-280de0b40717" />

## UH4 - Boss/Vilão Principal
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/c66ba0db-9d21-4512-91aa-40784cccf4c0" />

## UH5 - Sistema de Dano ao Boss (Objetivo de combate)
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/fc692b30-9f89-41fa-baa6-a78a493cf0f9" />

## UH6 - Sistema de Advinhação (Mecânica específica)
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/aa2f0698-d6a6-4beb-a112-1c32f64740c5" />

## UH7 - Regras de Tentativa (Controle da mecânica)
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/efaa3b32-5189-400f-b95f-ccd6a86b67ea" />

## UH8 - Drop de Atributos (Recompensa/Progressão)
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/c4a91183-41b3-4137-9076-ef9c826851ad" />

## UH9 - Aplicação de Buffs (Efeito dos drops)
[UH9 - Aplicação de Buffs (Efeito dos drops).pdf](https://github.com/user-attachments/files/27028423/UH9.-.Aplicacao.de.Buffs.Efeito.dos.drops.pdf)

## UH10 - Sistema de Tempo e Vitória(Contexto da partida)


# 📊 Board e Backlog (Kanban)

<img width="1176" height="570" alt="Captura de tela 2026-04-23 155714" src="https://github.com/user-attachments/assets/34e2bb94-0e8c-4dd3-9ce7-a7ef4d2ad1e0" />

# Protótipo Figma

<p align="center">
  <img width="45%" height="596" alt="Captura de tela 2026-04-23 155925" src="https://github.com/user-attachments/assets/b8948ca4-215b-4baf-946e-1248f2658c10" />
  <img width="45%" height="595" alt="Captura de tela 2026-04-23 160136" src="https://github.com/user-attachments/assets/4e3d46f4-ba1e-4ff5-b70f-1b40becc56f5" />
</p>

 ---
 
# ScreenCast (Figma)

https://github.com/user-attachments/assets/2edef130-cd48-4c52-86be-987653a0cf1e
