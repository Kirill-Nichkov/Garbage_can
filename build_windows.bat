@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%build-windows"
set "PROJECT_FILE=%PROJECT_DIR%GarbagePanel.pro"

cd /d "%PROJECT_DIR%"

echo Building Garbage Panel for Windows...

where qmake >nul 2>nul
if errorlevel 1 (
    echo Error: qmake was not found in PATH.
    echo Open a Qt MinGW command prompt or add the Qt bin directory to PATH.
    exit /b 1
)

where mingw32-make >nul 2>nul
if errorlevel 1 (
    echo Error: mingw32-make was not found in PATH.
    echo Install the Qt MinGW toolchain or add it to PATH.
    exit /b 1
)

if "%NUMBER_OF_PROCESSORS%"=="" (
    set "NUMBER_OF_PROCESSORS=1"
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    if errorlevel 1 exit /b 1
)

pushd "%BUILD_DIR%"

qmake "%PROJECT_FILE%" -spec win32-g++ "CONFIG+=release"
if errorlevel 1 (
    popd
    exit /b 1
)

mingw32-make -j%NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    popd
    exit /b 1
)

if exist "release" (
    copy /Y "%PROJECT_DIR%background.png" "release\background.png" >nul
)

if exist "debug" (
    copy /Y "%PROJECT_DIR%background.png" "debug\background.png" >nul
)

if exist "release\garbage-panel.exe" (
    where windeployqt >nul 2>nul
    if not errorlevel 1 (
        windeployqt "release\garbage-panel.exe"
        if errorlevel 1 (
            popd
            exit /b 1
        )
        copy /Y "%PROJECT_DIR%background.png" "release\background.png" >nul
    ) else (
        echo Warning: windeployqt was not found, Qt DLLs were not copied.
    )
)

popd

echo Build complete.
echo Executable: build-windows\release\garbage-panel.exe
