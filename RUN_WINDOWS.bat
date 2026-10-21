@echo off
setlocal
cd /d "%~dp0backend"
echo ==================================================
echo DrainWatch - Storm Drain Telemetry System
echo ==================================================
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ was not found.
  echo Open the MSYS2 UCRT64 terminal and install GCC, or add UCRT64 bin to PATH.
  echo Command: pacman -S --needed mingw-w64-ucrt-x86_64-gcc
  pause
  exit /b 1
)
echo Compiling C++ backend...
g++ -std=c++17 -O2 -Wall -Wextra main.cpp server.cpp WeatherApiClient.cpp Graph.cpp HashTable.cpp CircularQueue.cpp MaxHeap.cpp MaintenanceLog.cpp -o storm_server.exe -lws2_32 -lwinhttp
if errorlevel 1 (
  echo.
  echo Build failed. Read the compiler errors above.
  pause
  exit /b 1
)
echo.
echo Build successful. Starting backend...
echo Public dashboard: http://localhost:8088/
echo Admin portal:     http://localhost:8088/admin.html
echo Demo login: admin / Drain@2026
echo Keep this window open while using the webpages.
echo Press Ctrl+C to stop the server.
echo.
storm_server.exe
pause
