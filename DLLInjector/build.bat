@echo off
echo ========================================
echo InfiniteGUI Injector - Build Script
echo ========================================
echo.

cd /d "%~dp0"

if not exist "DLLInjector.csproj" (
    echo Error: Project file DLLInjector.csproj not found
    pause
    exit /b 1
)

echo Building project...
dotnet build -c Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo.
    echo Executable location:
    echo bin\Release\net8.0-windows\InfiniteGUIInjector.exe
    echo.
    pause
) else (
    echo.
    echo ========================================
    echo Build failed!
    echo ========================================
    echo.
    pause
    exit /b 1
)