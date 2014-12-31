@echo off

REM MinGW-w64 C++ mexopts.bat
REM by Jonathan Chappelow (chappjc)

set MINGWROOT=C:\ProgFiles\MinGW-w64\mingw64
set PATH=%MINGWROOT%\bin;%PATH%

set MATLAB=%MATLAB_ROOT%
set MW_TARGET_ARCH=win64

set COMPILER=x86_64-w64-mingw32-g++
set COMPFLAGS=-c -m64 -mwin32 -mdll -Wall -DMATLAB_MEX_FILE
REM Optionally add -std=c++11 to COMPFLAGS (-std=c99 for C)
set OPTIMFLAGS=-DNDEBUG -O3
set DEBUGFLAGS=-g
set NAME_OBJECT=-o

set LIBLOC=%MATLAB%\extern\lib\%MW_TARGET_ARCH%\microsoft
set LINKER=x86_64-w64-mingw32-g++
set LINKFLAGS=-shared -L"%LIBLOC%" -L"%MATLAB%\bin\win64"
REM Optionally add to LINKFLAGS after -shared to get standalone output: -static-libstdc++ -static-libgcc
set LINKFLAGSPOST=-lmx -lmex -leng -lmat -lmwlapack -lmwblas
set LINKOPTIMFLAGS=-O3
set LINKDEBUGFLAGS=-g
set LINK_FILE=
set LINK_LIB=
set NAME_OUTPUT=-o "%OUTDIR%%MEX_NAME%%MEX_EXT%"

set RC_COMPILER=
set RC_LINKER=
