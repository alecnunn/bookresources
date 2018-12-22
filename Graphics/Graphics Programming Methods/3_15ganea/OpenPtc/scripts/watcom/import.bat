@echo off
echo watcom c/c++ import library
if not exist ..\..\library\watcom mkdir ..\..\library\watcom
wlib -n -b ../../library/watcom/ptc.lib ../../library/ptc.dll > NUL
wlib -n -b ../../library/watcom/ptcdebug.lib ../../library/ptcdebug.dll > NUL
