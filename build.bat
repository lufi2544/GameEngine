
@echo off

:: Set variables
set THIS_DIR=%~dp0
set SRC_DIR=%THIS_DIR%/source
set TESTS_DIR=%~dp0/tests
set BIN_DIR=bin
set CFLAGS=/std:c17 /Zi
set MAIN_FILE=%SRC_DIR%/win32_main.cpp
set MAYORANA_DIR=%SRC_DIR%/MayoranaFramework/source
set MAYORANA_REF=%SRC_DIR%/mayorana-reflection/source
set FLAGS=MAYORANA
set BINARY_NAME=engine
set BUILD_DIR=%THIS_DIR%/build

if not exist %BIN_DIR% mkdir %BIN_DIR%

cd %BIN_DIR%

:: Create the bin directory if it doesn't exist

:: Set up MSVC environment (ensure it's the correct script)
call %BUILD_DIR%/setup_cl_x64.bat

:: Compile with MSVC
:: Using Fe: in stead of OUT, since we are using unity build.
cl /Fe:%BINARY_NAME% %CFLAGS% -I%SRC_DIR% -I%TESTS_DIR% -I%MAYORANA_DIR% -I%MAYORANA_REF%/reflection_includes.h %MAIN_FILE% -D%FLAGS% /link user32.lib D3D11.lib /SUBSYSTEM:CONSOLE /nologo

:: print the generated code to a file( we can do this more elegantly with the game files, but is fine ).
::%BINARY_NAME% > game_generated.h

