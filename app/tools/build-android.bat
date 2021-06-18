@echo off
setlocal enabledelayedexpansion

set CURR_DIR=%CD%
SET PROJECT_DIR=%1

if [%PROJECT_DIR%]==[] (
    SET PROJECT_DIR=%~dp0\..
)

call %~dp0\build-rom.bat !PROJECT_DIR!
set ROM=!PROJECT_DIR!\release\ROM.zip

SET GAME_BUILDER=%APPDATA%\indigames\igeGameBuilder
set GAME_BUILDER_DRIVE=%GAME_BUILDER:~0,2%

%GAME_BUILDER_DRIVE%
if not exist "%GAME_BUILDER%" (
    if not exist "%APPDATA%\indigames" (
        md "%APPDATA%\indigames"
    )
    cd "%APPDATA%\indigames"
    git clone https://github.com/indigames/igeGameBuilder --recursive
)

cd %GAME_BUILDER%
git clean -xfdf
git reset --hard
git pull origin master

xcopy /q /y %ROM% %GAME_BUILDER%
scripts\build-android.bat
if %ERRORLEVEL% NEQ 0 goto ERROR

if not exist !PROJECT_DIR!\release (
    mkdir !PROJECT_DIR!\release
)
xcopy /q /y %GAME_BUILDER%\release\*.apk !PROJECT_DIR!\release
xcopy /q /y %GAME_BUILDER%\release\*.aab !PROJECT_DIR!\release

:ERROR
    echo Script returning with error %ERRORLEVEL%

:ALL_DONE
    set CURR_DRIVE=%CURR_DIR:~0,2%
    %CURR_DRIVE%
    cd %CURR_DIR%
    echo ALL DONE!

