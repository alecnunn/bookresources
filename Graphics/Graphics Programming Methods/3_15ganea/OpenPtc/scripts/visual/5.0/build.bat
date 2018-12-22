@echo off


echo:
echo Visual C++ 5.0 build script
echo:


if "%1" == "" goto error


set PTC_INCLUDE="..\..\source"


:select_build
if "%2" == "" goto release
if "%2" == "debug" goto debug
if "%2" == "release" goto release
goto error

:debug
echo debug build
echo:
set PTC_BUILD_OPTIONS=/nologo /MD /W3 /GX
set PTC_LIBRARY=..\..\library\visual\ptcdebug.lib
set PTC_DEFINES=/D "WIN32" /D "_WINDOWS" /D "_MBCS"
goto select_output

:release
echo release build
echo:
set PTC_BUILD_OPTIONS=/nologo /MD /W3 /GX /O2 /Ox
set PTC_LIBRARY=..\..\library\visual\ptc.lib
set PTC_DEFINES=/D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"
goto select_output


:select_output
if "%3" == "" goto windows
if "%3" == "console" goto console
if "%3" == "windows" goto windows
goto error

:console
echo console application
echo:
set PTC_LINKER_OPTIONS=/subsystem:console
goto select_compiler

:windows
echo windows application
echo:
set PTC_LINKER_OPTIONS=/subsystem:windows
goto select_compiler


:select_compiler
if exist %1.cpp goto cpp_compiler
if exist %1.c goto c_compiler
goto error


:cpp_compiler
echo c++ compiler
echo:
cl %PTC_BUILD_OPTIONS% /I %PTC_INCLUDE% %PTC_DEFINES% /c %1.cpp
goto link


:c_compiler
echo c compiler
echo:
cl %PTC_BUILD_OPTIONS% /I %PTC_INCLUDE% %PTC_DEFINES% /c %1.c
goto link


:link
echo:
link %1.obj %PTC_LIBRARY% kernel32.lib user32.lib gdi32.lib /nologo %PTC_LINKER_OPTIONS% /incremental:no /pdb:none /machine:I386 /out:"%1.exe"
goto done


:error
echo error: read "scripts.txt"
goto done


:done
