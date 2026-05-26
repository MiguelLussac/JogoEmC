@echo off
rem Inicia VcXsrv (se necessário) e executa o jogo com GUI via Docker
powershell -ExecutionPolicy Bypass -NoProfile -File "%~dp0scripts\start_game.ps1"
