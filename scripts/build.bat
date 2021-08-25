@echo off

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

set CONAN_REVISIONS_ENABLED=1

conan config set storage.download_cache="D:\DevTools\.conan_cache"
set CONAN_USER_HOME=D:\DevTools\.conan_igeCreator

conan remote add ige-center http://10.1.0.222:8081/artifactory/api/conan/conan --force

cd %PROJECT_DIR%
python %PROJECT_DIR%/build.py

cd %CALL_DIR%
