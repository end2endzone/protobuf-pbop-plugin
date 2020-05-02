@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set GTEST_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install
set rapidassist_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\RapidAssist\install
echo rapidassist_DIR=%rapidassist_DIR%

echo ============================================================================
echo Cloning RapidAssist into %APPVEYOR_BUILD_FOLDER%\third_parties\RapidAssist
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/end2endzone/RapidAssist.git"
cd RapidAssist
echo.

echo Checking out version v0.8.1...
git -c advice.detachedHead=false checkout 0.8.1
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%rapidassist_DIR% -DCMAKE_PREFIX_PATH="%GTEST_ROOT%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %rapidassist_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
