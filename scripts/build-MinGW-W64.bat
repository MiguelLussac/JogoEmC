@echo off
cd /d "%~dp0..\build"
premake5.exe gmake
cd /d "%~dp0.."
mingw32-make clean
pause
