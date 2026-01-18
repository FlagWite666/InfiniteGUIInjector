@echo off
echo ========================================
echo InfiniteGUI Injector - Publish Single File
echo ========================================
echo.

cd /d "%~dp0"

if not exist "DLLInjector.csproj" (
    echo Error: Project file DLLInjector.csproj not found
    pause
    exit /b 1
)

echo Publishing single EXE file...
dotnet publish -c Release -r win-x64 --self-contained false /p:PublishSingleFile=true

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Publish successful!
    echo ========================================
    echo.
    echo Single EXE file location:
    echo bin\Release\net8.0-windows\win-x64\publish\InfiniteGUIInjector.exe
    echo.
    echo File size:
    dir "bin\Release\net8.0-windows\win-x64\publish\InfiniteGUIInjector.exe" | find "InfiniteGUIInjector.exe"
    echo.
    pause
) else (
    echo.
    echo ========================================
    echo Publish failed!
    echo ========================================
    echo.
    pause
    exit /b 1
)