@echo off
setlocal
set PROJECT=IBLBaker

mkdir Build64
cd Build64

cmake.exe -Wno-dev -G "Visual Studio 14 2015 Win64" ../

endlocal

pause
