@echo off
rem Executa o teste headless (xvfb) dentro do container
powershell -ExecutionPolicy Bypass -NoProfile -File "%~dp0scripts\start_game.ps1" -Headless
