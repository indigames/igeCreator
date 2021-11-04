@echo off
setlocal enabledelayedexpansion

set CURR_DIR=%CD%
SET PROJECT_DIR=%1

if [%PROJECT_DIR%]==[] (
    SET PROJECT_DIR=%~dp0\..
)

python -m pip install -i https://test.pypi.org/simple/ igeCore --user --upgrade

python "%~dp0..\convert.py" "%PROJECT_DIR%" "%PROJECT_DIR%"

pause

if %ERRORLEVEL% NEQ 0 goto ERROR


:ERROR
    echo Script returning with error %ERRORLEVEL%
    echo Please make sure you have Python 3.9 64bit installed and added to PATH environment variable
    exit 1

:ALL_DONE
    set CURR_DRIVE=%CURR_DIR:~0,2%
    %CURR_DRIVE%
    cd %CURR_DIR%
    echo ALL DONE!
