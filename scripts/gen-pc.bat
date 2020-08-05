@echo off

SET LIB_NAME=igeCreator

SET BUILD_DEBUG=0
SET BUILD_X86=0
SET BUILD_SHARED=0

echo COMPILING ...
SET PROJECT_DIR=%~dp0..

SET BUILD_DIR=%PROJECT_DIR%\build\pc
SET OUTPUT_DIR=%IGE_LIBS%\%LIB_NAME%
SET OUTPUT_LIBS_DEBUG=%OUTPUT_DIR%\libs\pc\Debug
SET OUTPUT_LIBS_RELEASE=%OUTPUT_DIR%\libs\pc

SET CALL_DIR=%CD%

echo Compiling %LIB_NAME% ...

if not exist %OUTPUT_DIR% (
    mkdir %OUTPUT_DIR%
)

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

echo Cleaning up...
    if [%BUILD_DEBUG%]==[1] (
        if exist %OUTPUT_LIBS_DEBUG% (
            rmdir /s /q %OUTPUT_LIBS_DEBUG%
        )
        mkdir %OUTPUT_LIBS_DEBUG%
    )

    if exist %OUTPUT_LIBS_RELEASE% (
        rmdir /s /q %OUTPUT_LIBS_RELEASE%
    )
    mkdir %OUTPUT_LIBS_RELEASE%

cd %PROJECT_DIR%
if [%BUILD_X86%]==[1] (
    echo Generating x86...
    if not exist %BUILD_DIR%\x86 (
        mkdir %BUILD_DIR%\x86
    )
    cd %BUILD_DIR%\x86
    echo Generating x86 CMAKE project ...
    cmake %PROJECT_DIR% -A Win32 -DAPP_STYLE=STATIC
    if %ERRORLEVEL% NEQ 0 goto ERROR

    echo Generating x86 DONE
)

cd %PROJECT_DIR%
echo Generating x64...
    if not exist %BUILD_DIR%\x64 (
        mkdir %BUILD_DIR%\x64
    )
    echo Generating x64 CMAKE project ...
    cd %BUILD_DIR%\x64
    cmake %PROJECT_DIR% -A x64 -DAPP_STYLE=STATIC
    if %ERRORLEVEL% NEQ 0 goto ERROR    
echo Generating x64 DONE

goto ALL_DONE

:ERROR
    echo ERROR OCCURED DURING COMPILING!

:ALL_DONE
    cd %CALL_DIR%
    echo COMPILING DONE!