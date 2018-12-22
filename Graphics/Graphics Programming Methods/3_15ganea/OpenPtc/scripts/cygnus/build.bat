@echo off


echo:
echo Cygnus C++ (Cygwin) build script
echo:


if "%1" == "" goto error


set PTC_INCLUDE=-I. -I../../source -I-


:select_build
if "%2" == "" goto release
if "%2" == "debug" goto debug
if "%2" == "release" goto release
goto error

:debug
echo debug build
echo:
set PTC_BUILD_OPTIONS=
set PTC_LIBRARY=..\..\library\cygnus\ptcdebug.lib
set PTC_LINK_STRIP=
goto select_output

:release
echo release build
echo:
set PTC_BUILD_OPTIONS=-O2
set PTC_LIBRARY=..\..\library\cygnus\ptc.lib
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
set PTC_LINKER_OPTIONS=-Wl,--subsystem,console
goto select_compiler

:windows
echo windows application
echo:
set PTC_LINKER_OPTIONS=-Wl,--subsystem,windows,-e,_mainCRTStartup
goto select_compiler


:select_compiler
if exist %1.c goto c_compiler
if exist %1.cpp goto cpp_compiler
goto error


:cpp_compiler
echo c++ compiler
echo:
g++ %1.cpp -o %1.exe %PTC_INCLUDE% %PTC_BUILD_OPTIONS% %PTC_LINK_OPTIONS% %PTC_LIBRARY% %PTC_LINK_STRIP% -lgdi32 -luser32
goto done


:c_compiler
echo c compiler
echo:
g++ %1.c -o %1.exe %PTC_INCLUDE% %PTC_BUILD_OPTIONS% %PTC_LINK_OPTIONS% %PTC_LIBRARY% %PTC_LINK_STRIP% -lgdi32 -luser32
goto done


:error
echo error: read "scripts.txt"
goto done


:done
