@echo off
setlocal enabledelayedexpansion

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

if [%CONAN_USER_HOME%]==[] (
    set CONAN_REVISIONS_ENABLED=1
    set CONAN_USER_HOME=%PROJECT_DIR%
    conan config set storage.download_cache="%APPDATA%\indigames\.conan_cache"
    conan remote add ige-center http://10.1.0.222:8081/artifactory/api/conan/conan --force
    conan remote disable "*"
    conan remote enable ige-center
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

if exist project (
	del /s /q project
)
mkdir project
cd project
conan install --update .. --profile ../cmake/profiles/windows_x86_64
cmake .. -A X64

cd %CALL_DIR%
