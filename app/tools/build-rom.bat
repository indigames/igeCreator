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

SET PROJECT_TMP=!PROJECT_DIR!\.tmp
SET ROM_PATH=!PROJECT_TMP!\rom
SET DST_PATH=!PROJECT_DIR!\release

cd !PROJECT_DIR!

echo Updating igeCore ...
python -m pip install igeCore --upgrade --user
if %ERRORLEVEL% NEQ 0 goto ERROR

echo Creating temp project...
if exist !PROJECT_TMP! (
    rmdir /s /q !PROJECT_TMP!
)
mkdir !PROJECT_TMP!

if exist "!DST_PATH!" (
    rmdir /s/ q "!DST_PATH!"
)
mkdir !DST_PATH!

for /d %%i in (!PROJECT_DIR!\*) do (
    ECHO %%i| FINDSTR /C:"!PROJECT_TMP!" > nul && (
        echo Excluded: %%i
    ) || (
        xcopy /q /s /y %%i\* !PROJECT_TMP!\%%~nxi\
    )
)
xcopy /q /y !PROJECT_DIR!\* !PROJECT_TMP!\

if exist "!PROJECT_TMP!\config" (
    rmdir /s /q "!PROJECT_TMP!\config"
)

mkdir !ROM_PATH!

echo Building ROMs ...

cd !PROJECT_TMP!
python %~dp0\build.py -p %PLATFORM_PC% -i . -o !ROM_PATH!\pc -c False
if %ERRORLEVEL% NEQ 0 goto ERROR
if exist "!ROM_PATH!\pc\config" (
    rmdir /s /q  "!ROM_PATH!\pc\config"
)

python %~dp0\build.py -p %PLATFORM_ANDROID% -i . -o !ROM_PATH!\android -c False
if %ERRORLEVEL% NEQ 0 goto ERROR
if exist "!ROM_PATH!\android\config" (
    rmdir /s /q  "!ROM_PATH!\android\config"
)

echo Compressing the result...
cd !PROJECT_DIR!
if exist !DST_PATH!\ROM.zip (
    del /f /q !DST_PATH!\ROM.zip
)
%TOOLS_DIR%\7z.exe a !DST_PATH!\ROM.zip !ROM_PATH!
if exist "!PROJECT_DIR!\config" (
    %TOOLS_DIR%\7z.exe a !DST_PATH!\ROM.zip !PROJECT_DIR!\config
    %TOOLS_DIR%\7z.exe a !DST_PATH!\ROM.zip !PROJECT_DIR!\config\app.yaml
)
if %ERRORLEVEL% NEQ 0 goto ERROR

@rem Skip deploy
goto ALL_DONE

:ERROR
    echo Script returning with error %ERRORLEVEL%

:ALL_DONE
    echo Cleaning up...
    if exist "!PROJECT_TMP!" (
        rmdir /s /q !PROJECT_TMP!
    )
    cd %CURR_DIR%
    echo ALL DONE!
