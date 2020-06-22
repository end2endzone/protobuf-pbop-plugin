@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set rapidassist_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\RapidAssist\install
set protobuf_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\install
set libPBOP_DIR=%APPVEYOR_BUILD_FOLDER%\install
set zlib_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\zlib\install

:: Add protoc.exe in PATH
set PATH=%protobuf_DIR%\bin;%PATH%

:: Add protobuf-pbop-plugin.exe in PATH
set PATH=%libPBOP_DIR%\bin;%PATH%

echo ============================================================================
echo Generating...
echo ============================================================================
cd /d %APPVEYOR_BUILD_FOLDER%\demo\Greetings
if %errorlevel% neq 0 exit /b %errorlevel%
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_PREFIX_PATH="%rapidassist_DIR%;%protobuf_DIR%;%libPBOP_DIR%;%zlib_DIR%" -DBUILD_SHARED_LIBS=OFF ..
if %errorlevel% neq 0 exit /b %errorlevel%

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

::Return to launch folder
cd /d %~dp0
