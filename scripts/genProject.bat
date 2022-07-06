@echo off
setlocal enabledelayedexpansion

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

cd %PROJECT_DIR%

if exist project (
  del /s /q project
)
mkdir project
cd project
cmake .. -A X64
cd %CALL_DIR%
