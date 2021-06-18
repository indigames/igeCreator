@echo off
setlocal enabledelayedexpansion

set CURR_DIR=%CD%
SET PROJECT_DIR=%1

if [%PROJECT_DIR%]==[] (
    SET PROJECT_DIR=%~dp0\..
)

set GAME_BUILDER=%APPDATA%\indigames\igeGameBuilder
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

scripts\build-rom.bat !PROJECT_DIR! !PROJECT_DIR!\release
if %ERRORLEVEL% NEQ 0 goto ERROR


:ERROR
    echo Script returning with error %ERRORLEVEL%

:ALL_DONE
    set CURR_DRIVE=%CURR_DIR:~0,2%
    %CURR_DRIVE%
    cd %CURR_DIR%
    echo ALL DONE!
