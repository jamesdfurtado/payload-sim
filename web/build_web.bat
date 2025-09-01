@echo off
echo Building for web...

where emcc >nul 2>nul
if %errorlevel% neq 0 (
    echo Emscripten not found. Install and activate emsdk first.
    exit /b 1
)

if not exist "build_web" mkdir build_web
cd build_web

copy ..\web\CMakeLists_Web.txt ..\CMakeLists.txt.backup
copy ..\web\CMakeLists_Web.txt ..\CMakeLists.txt

rem Add ninja to PATH for this session
set "PATH=%cd%\..\;%PATH%"

emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -G "Ninja" -DCMAKE_MAKE_PROGRAM="%cd%\..\ninja.exe"
emmake ninja

if exist "payload-sim.html" (
    echo Build complete. Files: payload-sim.html, .js, .wasm
    echo Test with: emrun payload-sim.html
) else (
    echo Build failed.
)

cd ..\web
