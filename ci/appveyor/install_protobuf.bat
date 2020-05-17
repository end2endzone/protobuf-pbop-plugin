@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

set zlib_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\zlib\install
set protobuf_DIR=%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\install
echo protobuf_DIR=%protobuf_DIR%

echo ============================================================================
echo Cloning protobuf into %APPVEYOR_BUILD_FOLDER%\third_parties\protobuf
echo ============================================================================
mkdir %APPVEYOR_BUILD_FOLDER%\third_parties >NUL 2>NUL
cd %APPVEYOR_BUILD_FOLDER%\third_parties
git clone "https://github.com/protocolbuffers/protobuf.git"
cd protobuf
echo.

echo Checking out version v3.5.1.1...
git -c advice.detachedHead=false checkout v3.5.1.1
echo.

echo ============================================================================
echo Patching for a bug on Windows... See https://github.com/protocolbuffers/protobuf/issues/5200
echo ============================================================================
REM https://github.com/protocolbuffers/protobuf/pull/5203/files
REM https://github.com/protocolbuffers/protobuf/pull/4016/files
echo Step 1/5
cscript //Nologo %~dp0replace_inplace.vbs "%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\src\google\protobuf\stubs\io_win32.cc" "static const wstring dotdot(L&quot;..&quot;);\r\n\r\n" "static const wstring dotdot(L&quot;..&quot;);\r\n  const wchar_t *p = path.c_str();\r\n\r\n"
if %errorlevel% neq 0 exit /b %errorlevel%
echo Step 2/5
cscript //Nologo %~dp0replace_inplace.vbs "%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\src\google\protobuf\stubs\io_win32.cc" "if (!is_separator(path[i]) && path[i] != L'\0') {" "if (!is_separator(p[i]) && p[i] != L'\0') {"
if %errorlevel% neq 0 exit /b %errorlevel%
echo Step 3/5
cscript //Nologo %~dp0replace_inplace.vbs "%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\src\google\protobuf\stubs\io_win32.cc" "wstring segment(path, segment_start, i - segment_start);" "wstring segment(p, segment_start, i - segment_start);"
if %errorlevel% neq 0 exit /b %errorlevel%
echo Step 4/5
cscript //Nologo %~dp0replace_inplace.vbs "%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\src\google\protobuf\stubs\io_win32.cc" "if (path[i] == L'\0') {" "if (p[i] == L'\0') {"
if %errorlevel% neq 0 exit /b %errorlevel%
echo Step 5/5
cscript //Nologo %~dp0replace_inplace.vbs "%APPVEYOR_BUILD_FOLDER%\third_parties\protobuf\src\google\protobuf\stubs\io_win32.cc" "if (!path.empty() && is_separator(path[path.size() - 1])) {" "if (!path.empty() && is_separator(p[path.size() - 1])) {"
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build_dir >NUL 2>NUL
cd build_dir
cmake -Wno-dev -DCMAKE_GENERATOR_PLATFORM=%Platform% -T %PlatformToolset% -DCMAKE_INSTALL_PREFIX=%protobuf_DIR% -DCMAKE_PREFIX_PATH="%zlib_DIR%" -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF -Dprotobuf_WITH_ZLIB=ON ..\cmake
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build . --config %Configuration% -- -maxcpucount /m
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

echo ============================================================================
echo Installing into %protobuf_DIR%
echo ============================================================================
cmake --build . --config %Configuration% --target INSTALL
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
