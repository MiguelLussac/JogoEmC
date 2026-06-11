FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    git \
    libasound2-dev \
    libgl1-mesa-dev \
    libwayland-dev \
    libx11-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libxkbcommon-dev \
    libxrandr-dev \
    make \
    mesa-common-dev \
    pkg-config \
    unzip \
    xauth \
    xvfb \
 && rm -rf /var/lib/apt/lists/*

COPY . /app

RUN git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib && \
    cmake -S /tmp/raylib -B /tmp/raylib/build -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF -DBUILD_GAMES=OFF && \
    cmake --build /tmp/raylib/build -j && \
    cmake --install /tmp/raylib/build && \
    ldconfig && \
    mkdir -p ./bin/Debug && \
    gcc -std=c17 \
    -I./src \
    -I./include \
    ./src/main.c \
    ./src/player/player.c \
    ./src/boss/boss.c \
    ./src/game/questions.c \
    ./src/game/partida.c \
    ./src/game/game.c \
    ./src/game/history.c \
    ./src/analysis/analise.c \
    ./src/logic/logic_phase.c \
    ./src/audio/audio.c \
    ./src/visual/vfx.c \
    -o ./bin/Debug/JogoEmC \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CMD ["./bin/Debug/JogoEmC"]
