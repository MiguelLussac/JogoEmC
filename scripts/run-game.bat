@echo off
rem Inicia VcXsrv (se necessario) e executa o jogo com GUI via Docker
powershell -ExecutionPolicy Bypass -NoProfile -File "%~dp0start_game.ps1"
