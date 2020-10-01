@echo off

SET LIB_NAME=igeGame

SET BUILD_DEBUG=0

echo COMPILING ...
SET PROJECT_DIR=%~dp0..\libs\igeGame
SET BUILD_DIR=%~dp0..\build\game-pc
SET OUTPUT_DIR=%~dp0..\app

SET CALL_DIR=%CD%

echo Compiling %LIB_NAME% ...

if not exist %OUTPUT_DIR% (
    mkdir %OUTPUT_DIR%
)

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %PROJECT_DIR%

cd %PROJECT_DIR%
echo Compiling x64...
    if not exist %BUILD_DIR%\x64 (
        mkdir %BUILD_DIR%\x64
    )
    echo Generating x64 CMAKE project ...
    cd %BUILD_DIR%\x64
    cmake %PROJECT_DIR% -A x64 -DAPP_STYLE=STATIC
    if %ERRORLEVEL% NEQ 0 goto ERROR

    if [%BUILD_DEBUG%]==[1] (
        echo Compiling x64 - Debug...
        cmake --build . --config Debug -- -m
        if %ERRORLEVEL% NEQ 0 goto ERROR
        xcopy /q /s /y Debug\*.exe %OUTPUT_DIR%
    ) else (
        echo Compiling x64 - Release...
        cmake --build . --config Release -- -m
        if %ERRORLEVEL% NEQ 0 goto ERROR
        xcopy /q /s /y Release\*.exe %OUTPUT_DIR%
    )
echo Compiling x64 DONE

goto ALL_DONE

:ERROR
    echo ERROR OCCURED DURING COMPILING!

:ALL_DONE
    cd %CALL_DIR%
    echo COMPILING DONE!