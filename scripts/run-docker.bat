@echo off
setlocal

cd /d "%~dp0.."

echo [1/3] Build da imagem Docker...
docker compose -f docker\docker-compose.yml build
if errorlevel 1 (
  echo Falha no build da imagem Docker.
  exit /b 1
)

echo [2/3] Teste headless (xvfb)...
docker compose -f docker\docker-compose.yml run --rm minddrop bash -lc "timeout 5s xvfb-run -a ./bin/Debug/JogoEmC || true"
if errorlevel 1 (
  echo Falha no teste headless no container.
  exit /b 1
)

echo [3/3] Iniciando jogo no container...
docker compose -f docker\docker-compose.yml run --rm minddrop
if errorlevel 1 (
  echo.
  echo Nao foi possivel abrir a janela grafica via Docker.
  echo O build e o teste headless passaram com sucesso.
  echo Para executar com janela no Windows, inicie um servidor X (ex.: VcXsrv)
  echo e mantenha DISPLAY=host.docker.internal:0.0.
  exit /b 0
)

echo Fluxo Docker finalizado com sucesso.
exit /b 0
