@echo off

set CURR_DIR=%CD%

C:

SET APP_BUILDER=%APPDATA%\indigames\igeAppBuilder

if not exist "%APP_BUILDER%" (
    md "%APPDATA%\indigames"
    cd "%APPDATA%\indigames"
    git clone https://github.com/indigames/igeAppBuilder --recursive
)

cd %APP_BUILDER%
git checkout master
git pull
git submodule sync
git submodule update --init --recursive
git submodule update --init --remote

call scripts\build-pc.bat

cd %CURR_DIR%
