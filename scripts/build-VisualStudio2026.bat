@echo off
cd /d "%~dp0..\build"
premake5.exe vs2026 || pause
