@echo off
@echo off
setlocal enabledelayedexpansion

set CALL_DIR=%CD%
set CURR_DRIVE=!CALL_DIR:~0,2!

python -m pip install conan

set CONAN_REVISIONS_ENABLED=1
conan remote add ige-center http://10.1.0.222:8081/artifactory/api/conan/conan

if "%IGE_BUILDER%"=="" (
    set IGE_BUILDER=%APPDATA%\indigames\igeBuilder
)

if not exist "!IGE_BUILDER!\.git" (
    mkdir "!IGE_BUILDER!"
    git clone https://github.com/indigames/igeBuilder !IGE_BUILDER!
) else (
    set IGE_BUILDER_DRIVE=!IGE_BUILDER:~0,2!
    !IGE_BUILDER_DRIVE!
    cd !IGE_BUILDER!

    git fetch --all
    git checkout main
    git pull

    !CURR_DRIVE!
    cd !CALL_DIR!
)


if not exist "!IGE_BUILDER!\build-lib.bat" (
    echo ERROR: IGE_BUILDER was not found
    goto ERROR
)

if exist "%~dp0..\conanfile.py" (
    for /f "usebackq delims=" %%a in ("%~dp0..\conanfile.py") do (
        set ln=%%a
        for /f "tokens=1,2 delims='=' " %%b in ("!ln!") do (
                set currkey=%%b
                set currval=%%c
                
                if "!currkey!"=="name" (
                    set PROJECT_NAME=!currval!
                ) else if "!currkey!"=="version" (
                    set PROJECT_VER=!currval!
                )
            )
        )
    )
)

echo !PROJECT_NAME!_!PROJECT_VER!

set PROJECT_DIR=!CALL_DIR!
set BUILD_PLATFORM=windows
set BUILD_ARCH=x86_64

set BUILD_DIR=!PROJECT_DIR!\_ige_build\!BUILD_PLATFORM!\!BUILD_ARCH!
set OUTPUT_DIR=!PROJECT_DIR!\app

set CONAN_FILE=!PROJECT_DIR!
if exist "!PROJECT_DIR!\conanfile.txt" (
    set CONAN_FILE=!PROJECT_DIR!\conanfile.txt
)

set CONAN_PROFILE=!IGE_BUILDER!\profiles\!BUILD_PLATFORM!_!BUILD_ARCH!
echo CONAN_PROFILE=!CONAN_PROFILE!

echo Cleaning up...
    if not exist !OUTPUT_DIR! (
        mkdir !OUTPUT_DIR!
    )

    if exist !BUILD_DIR! (
        rmdir /s /q !BUILD_DIR!
    )
    mkdir !BUILD_DIR!

cd !BUILD_DIR!
    conan install --profile=!CONAN_PROFILE! --update !CONAN_FILE! --remote ige-center
    if !ERRORLEVEL! NEQ 0 goto ERROR

    cmake %PROJECT_DIR% -A x64
    if %ERRORLEVEL% NEQ 0 goto ERROR

    cmake --build . --config Release -- -m
    if %ERRORLEVEL% NEQ 0 goto ERROR

    xcopy /q /s /y bin\*.exe %OUTPUT_DIR%
echo Generating x64 DONE

goto ALL_DONE

:ERROR
    echo ERROR OCCURED DURING COMPILING!

:ALL_DONE
    cd %CALL_DIR%
    echo COMPILING DONE!