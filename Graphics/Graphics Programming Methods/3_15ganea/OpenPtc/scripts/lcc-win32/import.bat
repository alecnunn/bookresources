@echo off
echo lcc-win32 import library
if not exist ..\..\library\lcc-win32 mkdir ..\..\library\lcc-win32
buildlib ptc.exp ../../library/lcc-win32/ptc.lib ptc.dll
buildlib ptcdebug.exp ../../library/lcc-win32/ptcdebug.lib ptcdebug.dll
