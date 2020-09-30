@echo off

set CURR_DIR=%CD%
set PROJECT_DIR=%~dp0..
set ROM=%PROJECT_DIR%\release\ROM.zip

if not exist %ROM% (
    call %~dp0\build-rom.bat
)

SET GAME_BUILDER=%APPDATA%\indigames\igeGameBuilder

C:
if not exist "%GAME_BUILDER%" (
    if not exist "%APPDATA%\indigames" (
        md "%APPDATA%\indigames"
    )
    cd "%APPDATA%\indigames"
    git clone https://github.com/indigames/igeGameBuilder --recursive
)

cd %GAME_BUILDER%

git checkout master
git pull
git submodule update --init --remote

xcopy /q /y %ROM% %GAME_BUILDER%

call scripts\build-pc.bat

if not exist %PROJECT_DIR%\release (
    mkdir %PROJECT_DIR%\release
)
xcopy /q /y %GAME_BUILDER%\release\*.zip %PROJECT_DIR%\release

%CURR_DIR:~0,2%
cd %CURR_DIR%
