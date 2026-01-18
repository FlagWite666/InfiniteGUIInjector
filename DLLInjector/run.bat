@echo off
echo ========================================
echo InfiniteGUI Injector - Quick Start
echo ========================================
echo.

if exist "bin\Release\net8.0-windows\InfiniteGUIInjector.exe" (
    echo Starting InfiniteGUI Injector...
    start "" "bin\Release\net8.0-windows\InfiniteGUIInjector.exe"
) else (
    echo Error: Executable not found
    echo Please run build.bat first to build the project
    echo.
    pause
)