@echo off

SET PLATFORM_PC=0
SET PLATFORM_IOS=1
SET PLATFORM_ANDROID=2

SET WORKSPACE_DIR=%~dp0..
SET PROJECT_DIR=%~dp0..\app
SET TOOLS_DIR=%~dp0\tools

SET ROM_PATH=%WORKSPACE_DIR%\rom
SET DST_PATH=%WORKSPACE_DIR%\release
SET PROJECT_TMP=%WORKSPACE_DIR%\.proj

cd %WORKSPACE_DIR%

echo Updating igeCore ...
python -m pip install igeCore --upgrade --user
if %ERRORLEVEL% NEQ 0 goto ERROR

echo Creating temp project...
if exist %PROJECT_TMP% (
    rmdir /s /q %PROJECT_TMP%
)
mkdir %PROJECT_TMP%
xcopy /q /s /y %PROJECT_DIR%\* %PROJECT_TMP%\

echo Building ROMs ...
cd %PROJECT_TMP%
if exist %ROM_PATH% (
    rmdir /s /q %ROM_PATH%
)
mkdir %ROM_PATH%
python  %~dp0\build.py -p %PLATFORM_PC% -i . -o %ROM_PATH%\pc -c False
if %ERRORLEVEL% NEQ 0 goto ERROR

python  %~dp0\build.py -p %PLATFORM_ANDROID% -i . -o %ROM_PATH%\android -c False
if %ERRORLEVEL% NEQ 0 goto ERROR

echo Compressing the result...
cd %WORKSPACE_DIR%
if not exist %DST_PATH% (
    mkdir %DST_PATH%
)
if exist %DST_PATH%\ROM.zip (
    del /f /q %DST_PATH%\ROM.zip
)
%TOOLS_DIR%\7z.exe a %DST_PATH%\ROM.zip %ROM_PATH%
%TOOLS_DIR%\7z.exe a %DST_PATH%\ROM.zip %WORKSPACE_DIR%\config
%TOOLS_DIR%\7z.exe a %DST_PATH%\ROM.zip %WORKSPACE_DIR%\project\app.yaml
if %ERRORLEVEL% NEQ 0 goto ERROR

@rem Skip deploy
goto ALL_DONE

:ERROR
    echo Script returning with error %ERRORLEVEL%

:ALL_DONE
    echo Cleaning up...
    if exist %PROJECT_TMP% (
        rmdir /s /q %PROJECT_TMP%
    )
    rmdir /s /q %ROM_PATH%
    cd %WORKSPACE_DIR%
    echo ALL DONE!
