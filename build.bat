@echo off
REM Command-line build (alternative to opening the project in CLion).
REM Uses the MinGW-w64 installed via winget, which supports full static linking.
setlocal
set GPP="C:\Users\Usuario\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\g++.exe"
cd /d "%~dp0"

%GPP% -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++ -o sort_practice.exe src\main.cpp
echo sort_practice EXITCODE=%ERRORLEVEL%

%GPP% -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++ -o generate_dataset.exe src\generate_dataset.cpp
echo generate_dataset EXITCODE=%ERRORLEVEL%
endlocal
