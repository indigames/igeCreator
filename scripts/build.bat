@echo off
setlocal enabledelayedexpansion

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

if [%CONAN_USER_HOME%]==[] (
    set CONAN_REVISIONS_ENABLED=1
    set CONAN_USER_HOME=D:\DevTools\.conan_igeCreator
    conan config set storage.download_cache="D:\DevTools\.conan_cache"
    
    conan remote disable conancenter
    conan remote disable conan-center
    conan remote add ige-center http://10.1.0.222:8081/artifactory/api/conan/conan --force
) else (
    REM This should be build machine environment
    python -m pip install pdoc3 --user --upgrade
    python -m pip install -i https://test.pypi.org/simple/ igeCore igeVmath --user --upgrade
    
    python -m pip install %PROJECT_DIR%\libs\igeScene --user
    
    python -m pdoc --config show_inherited_members=True --html igeVmath -o %PROJECT_DIR%\app\doc --skip-errors --force
    python -m pdoc --config show_inherited_members=True --html igeCore -o %PROJECT_DIR%\app\doc --skip-errors --force
    python -m pdoc --config show_inherited_members=True --html igeScene -o %PROJECT_DIR%\app\doc --skip-errors --force
)

cd %PROJECT_DIR%
python %PROJECT_DIR%/build.py

cd %CALL_DIR%
