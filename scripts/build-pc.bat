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
    echo Compiling x86...
    if not exist %BUILD_DIR%\x86 (
        mkdir %BUILD_DIR%\x86
    )
    cd %BUILD_DIR%\x86
    echo Generating x86 CMAKE project ...
    cmake %PROJECT_DIR% -A Win32 -DAPP_STYLE=STATIC
    if %ERRORLEVEL% NEQ 0 goto ERROR

    if [%BUILD_DEBUG%]==[1] (
        echo Compiling x86 - Debug...
        cmake --build . --config Debug -- -m
        if %ERRORLEVEL% NEQ 0 goto ERROR
        xcopy /q /s /y Debug\*.lib %OUTPUT_LIBS_DEBUG%\x86\
        xcopy /q /s /y Debug\*.dll %OUTPUT_LIBS_DEBUG%\x86\
    )

    echo Compiling x86 - Release...
    cmake --build . --config Release -- -m
    if %ERRORLEVEL% NEQ 0 goto ERROR
    xcopy /q /s /y Release\*.lib %OUTPUT_LIBS_RELEASE%\x86\
    xcopy /q /s /y Release\*.dll %OUTPUT_LIBS_RELEASE%\x86\

    echo Compiling x86 DONE
)

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
        xcopy /q /s /y Debug\*.lib %OUTPUT_LIBS_DEBUG%\x64\
        xcopy /q /s /y Debug\*.dll %OUTPUT_LIBS_DEBUG%\x64\
    )

    echo Compiling x64 - Release...
    cmake --build . --config Release -- -m
    if %ERRORLEVEL% NEQ 0 goto ERROR
    xcopy /q /s /y Release\*.lib %OUTPUT_LIBS_RELEASE%\x64\
    xcopy /q /s /y Release\*.dll %OUTPUT_LIBS_RELEASE%\x64\
echo Compiling x64 DONE

if [%BUILD_SHARED%]==[1] (
    cd %PROJECT_DIR%
    if [%BUILD_X86%]==[1] (
        echo Compiling x86_shared...
        if not exist %BUILD_DIR%\x86_shared (
            mkdir %BUILD_DIR%\x86_shared
        )
        cd %BUILD_DIR%\x86_shared
        echo Generating x86_shared CMAKE project ...
        cmake %PROJECT_DIR% -A Win32 -DAPP_STYLE=SHARED -DPYTHON_EXECUTABLE="%PROGRAMFILES(X86)%\Python37-32\python.exe"
        if %ERRORLEVEL% NEQ 0 goto ERROR

        if [%BUILD_DEBUG%]==[1] (
            echo Compiling x86_shared - Debug...
            cmake --build . --config Debug -- -m
            if %ERRORLEVEL% NEQ 0 goto ERROR
        )

        echo Compiling x86_shared - Release...
        cmake --build . --config Release -- -m
        if %ERRORLEVEL% NEQ 0 goto ERROR
        echo Compiling x86_shared DONE
    )

    cd %PROJECT_DIR%
    echo Compiling x64_shared...
        if not exist %BUILD_DIR%\x64_shared (
            mkdir %BUILD_DIR%\x64_shared
        )
        echo Generating x64_shared CMAKE project ...
        cd %BUILD_DIR%\x64_shared
        cmake %PROJECT_DIR% -A x64 -DAPP_STYLE=SHARED -DPYTHON_EXECUTABLE="%PROGRAMFILES%\Python37\python.exe"
        if %ERRORLEVEL% NEQ 0 goto ERROR

        if [%BUILD_DEBUG%]==[1] (
            echo Compiling x64_shared - Debug...
            cmake --build . --config Debug -- -m
            if %ERRORLEVEL% NEQ 0 goto ERROR
        )

        echo Compiling x64_shared - Release...
        cmake --build . --config Release -- -m
        if %ERRORLEVEL% NEQ 0 goto ERROR
    echo Compiling x64_shared DONE
)

goto ALL_DONE

:ERROR
    echo ERROR OCCURED DURING COMPILING!

:ALL_DONE
    cd %CALL_DIR%
    echo COMPILING DONE!