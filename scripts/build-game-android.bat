@echo off

set CURR_DIR=%CD%
set PROJECT_DIR=%~dp0..
set ROM=%PROJECT_DIR%\release\ROM.zip

if not exist %ROM% (
    call %~dp0\build-rom.bat
)

SET APP_BUILDER=%APPDATA%\indigames\igeAppBuilder

if not exist "%APP_BUILDER%" (
    md "%APPDATA%\indigames"
    cd "%APPDATA%\indigames"
    git clone https://github.com/indigames/igeAppBuilder --recursive
)

C:
cd %APP_BUILDER%

git checkout master
git pull
git submodule update --init --remote

xcopy /q /y %ROM% %APP_BUILDER%

call scripts\build-pc.bat

if not exist %PROJECT_DIR%\release (
    mkdir %PROJECT_DIR%\release
)
xcopy /q /y %APP_BUILDER%\igeLauncher\project\PC\*.zip %PROJECT_DIR%\release

%CURR_DIR:~0,2%
cd %CURR_DIR%
