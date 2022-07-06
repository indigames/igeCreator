@echo off
setlocal enabledelayedexpansion

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

if [%WORKSPACE%]==[] (
    echo Building on local machine...
) else (
    python -m pip install pdoc3 --user --upgrade
    python -m pip install -i https://test.pypi.org/simple/ igeCore igeVmath igeSdk --user --upgrade
    
    python -m pip install %PROJECT_DIR%\libs\igeScene --user
    
    python -m pdoc --config show_inherited_members=True --html igeVmath -o %PROJECT_DIR%\app\doc --skip-errors --force
    python -m pdoc --config show_inherited_members=True --html igeCore -o %PROJECT_DIR%\app\doc --skip-errors --force
    python -m pdoc --config show_inherited_members=True --html igeScene -o %PROJECT_DIR%\app\doc --skip-errors --force
    python -m pdoc --config show_inherited_members=True --html igeSdk -o %PROJECT_DIR%\app\doc --skip-errors --force
)

cd %PROJECT_DIR%
python %PROJECT_DIR%/build.py

cd %CALL_DIR%
