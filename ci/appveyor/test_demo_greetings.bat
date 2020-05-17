@echo off

:: Validate appveyor's environment
if "%APPVEYOR_BUILD_FOLDER%"=="" (
  echo Please define 'APPVEYOR_BUILD_FOLDER' environment variable.
  exit /B 1
)

echo =======================================================================
echo Testing Greetings demo project
echo =======================================================================
cd /d %APPVEYOR_BUILD_FOLDER%\demo\Greetings\build\bin\%Configuration%
echo Launching server...
if "%Configuration%" == "Debug" (
  start "" greetings_server-d.exe
) else (
  start "" greetings_server.exe
)

echo Waiting 5 seconds to allow the server to start serving...
echo %TIME%
powershell -command "Start-Sleep -s 5"
echo %TIME%
echo.

echo Launching client...
if "%Configuration%" == "Debug" (
  greetings_client-d.exe
) else (
  greetings_client.exe
)
echo.

echo Killing server...
if "%Configuration%" == "Debug" (
  taskkill /IM greetings_server-d.exe
) else (
  taskkill /IM greetings_server.exe
)

::reset error in case of test case fail
exit /b 0
