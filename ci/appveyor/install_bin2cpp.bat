@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set GTEST_ROOT=%APPVEYOR_BUILD_FOLDER%\third_parties\googletest\install
set rapidassist_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\RapidAssist\install
set bin2cpp_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\bin2cpp\install
echo bin2cpp_DIR=%bin2cpp_DIR%

echo ============================================================================
echo Cloning bin2cpp into %APPVEYOR_BUILD_FOLDER%\third_parties\bin2cpp
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/end2endzone/bin2cpp.git"
cd bin2cpp
echo.

echo Checking out version v2.3.0...
git -c advice.detachedHead=false checkout 2.3.0
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >NUL 2>NUL
cd build
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%bin2cpp_DIR% -DCMAKE_PREFIX_PATH="%GTEST_ROOT%;%rapidassist_DIR%" ..
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %bin2cpp_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
