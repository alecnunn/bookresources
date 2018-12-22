@echo off
echo cygnus c/c++ import library
echo this might take a while...
dlltool --dllname ptc.dll --def import.def --output-lib ../../library/cygnus/ptc.lib -v
dlltool --dllname ptcdebug.dll --def import.def --output-lib ../../library/cygnus/ptcdebug.lib -v
