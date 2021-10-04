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
)

cd %PROJECT_DIR%
python %PROJECT_DIR%/build.py

cd %CALL_DIR%
