@echo off


echo:
echo LCC-Win32 build script
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
set PTC_BUILD_OPTIONS=-g2
set PTC_LIBRARY=..\..\library\lcc-win32\ptcdebug.lib
set PTC_LINK_STRIP=
goto select_output

:release
echo release build
echo:
set PTC_BUILD_OPTIONS=-O
set PTC_LIBRARY=..\..\library\lcc-win32\ptc.lib
set PTC_LINK_STRIP=-s
goto select_output


:select_output
if "%3" == "" goto windows
if "%3" == "console" goto console
if "%3" == "windows" goto windows
goto error

:console
echo console application
echo:
set PTC_LINKER_OPTIONS=-subsystem console
goto select_compiler

:windows
echo windows application
echo:
set PTC_LINKER_OPTIONS=-subsystem windows
goto select_compiler


:select_compiler
if exist %1.c goto c_compiler
if exist %1.cpp goto cpp_compiler
goto error


:cpp_compiler
echo c++ compiler
echo:
echo LCC-Win32 is a C only compiler!
goto done


:c_compiler
echo c compiler
echo:
lcc %1.c %PTC_BUILD_OPTIONS% -I"%PTC_INCLUDE%"
goto link


:link
echo linking %1.exe
lcclnk %1.obj %PTC_LIBRARY% %PTC_LINKER_OPTIONS% %PTC_LINK_STRIP%
goto done


:error
echo error: read "scripts.txt"
goto done


:done
