@echo off


echo:
echo Watcom C++ 10.x build script
echo:


if "%1" == "" goto error


set PTC_INCLUDE=.;..\..\source


:select_build
if "%2" == "" goto release
if "%2" == "debug" goto debug
if "%2" == "release" goto release
goto error

:debug
echo debug build
echo:
set PTC_BUILD_OPTIONS=-bt=nt -hc
set PTC_LIBRARY=..\..\library\watcom\ptcdebug.lib
goto select_output

:release
echo release build
echo:
set PTC_BUILD_OPTIONS=-bt=nt -oneatxl+ -5r -fp5 -fpi87
set PTC_LIBRARY=..\..\library\watcom\ptc.lib
goto select_output


:select_output
if "%3" == "" goto windows
if "%3" == "console" goto console
if "%3" == "windows" goto windows
goto error

:console
echo console application
echo:
set PTC_LINKER_OPTIONS=system nt option stack=256k
goto select_compiler

:windows
echo windows application
echo:
set PTC_LINKER_OPTIONS=system nt_win option stack=256k
goto select_compiler


:select_compiler
if exist %1.cpp goto cpp_compiler
if exist %1.c goto c_compiler
goto error


:cpp_compiler
echo c++ compiler
echo:
wpp386 %1.cpp -i=%PTC_INCLUDE% %PTC_BUILD_OPTIONS% -xs
goto link


:c_compiler
echo c compiler
echo:
wcc386 %1.c -i=%PTC_INCLUDE% %PTC_BUILD_OPTIONS% 
goto link


:link
echo:
wlink name %1.exe file %1.obj library %PTC_LIBRARY% %PTC_LINKER_OPTIONS%
goto done


:error
echo error: read "scripts.txt"
goto done


:done
