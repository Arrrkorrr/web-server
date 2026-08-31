@echo off
cls

if not exist "build" (mkdir "build")
cd build

if not exist "win64" (mkdir "win64")
cd win64

if not exist "out" (mkdir "out")
cd out

if not exist "website" (mkdir "website")
xcopy "../../../website" "./website" /E /I /H /Y

cd ..
if not exist "cmake" (mkdir "cmake")
cd cmake

cmake ../../../ -G "MinGW Makefiles" -DCMAKE_C_COMPILER=C:/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
C:/mingw64/bin/mingw32-make.exe

move web_server.exe ../out/
cd ../out

copy "..\..\..\config\server.config" "./"
pause

cls
start web_server.exe

cd ../../../
pause
