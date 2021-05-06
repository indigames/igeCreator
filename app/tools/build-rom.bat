@echo off
setlocal enabledelayedexpansion

SET PLATFORM_PC=0
SET PLATFORM_IOS=1
SET PLATFORM_ANDROID=2

SET CURR_DIR=%CD%
SET PROJECT_DIR=%1

if [%PROJECT_DIR%]==[] (
    SET PROJECT_DIR=%~dp0\..
)

SET TOOLS_DIR=%~dp0\tools

SET ROM_PATH=!PROJECT_DIR!\.tmp\rom
SET DST_PATH=!PROJECT_DIR!\release

cd !PROJECT_DIR!

echo Updating igeCore ...
python -m pip install igeCore --upgrade --user
if %ERRORLEVEL% NEQ 0 goto ERROR

echo Building ROMs ...
if exist "!PROJECT_DIR!\.tmp" (
    rmdir /s /q  "!PROJECT_DIR!\.tmp"
)
if exist "!PROJECT_DIR!\release" (
    rmdir /s /q  "!PROJECT_DIR!\release"
)
mkdir %ROM_PATH%

cd !PROJECT_DIR!
python %~dp0\build.py -p %PLATFORM_PC% -i . -o %ROM_PATH%\pc -c False
if %ERRORLEVEL% NEQ 0 goto ERROR

python %~dp0\build.py -p %PLATFORM_ANDROID% -i . -o %ROM_PATH%\android -c False
if %ERRORLEVEL% NEQ 0 goto ERROR

echo Compressing the result...
cd !PROJECT_DIR!
if not exist %DST_PATH% (
    mkdir %DST_PATH%
)
if exist %DST_PATH%\ROM.zip (
    del /f /q %DST_PATH%\ROM.zip
)
%TOOLS_DIR%\7z.exe a %DST_PATH%\ROM.zip %ROM_PATH%
if exist "!PROJECT_DIR!\config" (
    %TOOLS_DIR%\7z.exe a %DST_PATH%\ROM.zip !PROJECT_DIR!\config
)
if %ERRORLEVEL% NEQ 0 goto ERROR

@rem Skip deploy
goto ALL_DONE

:ERROR
    echo Script returning with error %ERRORLEVEL%

:ALL_DONE
    echo Cleaning up...
    if exist "%ROM_PATH%" (
        rmdir /s /q %ROM_PATH%
    )
    if exist "%PROJECT_DIR%\.tmp" (
        rmdir /s /q %PROJECT_DIR%\.tmp
    )
    cd %CURR_DIR%
    echo ALL DONE!
