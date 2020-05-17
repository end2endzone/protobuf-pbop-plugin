@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set zlib_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\zlib\install
echo zlib_DIR=%zlib_DIR%

echo ============================================================================
echo Cloning zlib into %APPVEYOR_BUILD_FOLDER%\third_parties\zlib
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/madler/zlib.git"
cd zlib
echo.

echo Checking out version v1.2.11...
git -c advice.detachedHead=false checkout v1.2.11
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build_dir >NUL 2>NUL
cd build_dir
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%zlib_DIR% ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %zlib_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
