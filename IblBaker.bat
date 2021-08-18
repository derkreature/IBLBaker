@echo off
setlocal
set PROJECT=IBLBaker

mkdir Build64
cd Build64

cmake.exe -Wno-dev -G "Visual Studio 16 2019" -A x64 ../

endlocal

pause
