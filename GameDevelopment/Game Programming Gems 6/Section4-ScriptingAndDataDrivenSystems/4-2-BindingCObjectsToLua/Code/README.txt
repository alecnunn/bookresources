Binding C/C++ Objects to Lua
----------------------------

Waldemar Celes, Luiz Henrique de Figueiredo, Roberto Ierusalimschy

E-mail: team@lua.org




The distributed code illustrates the implementation of different strategies to 
bind C/C++ objects to Lua.

Each directory represents a different strategy to bind objects to Lua:

    + light: use light userdata without type checking
    + typedlight: use light userdata with type checking
    + full: use full userdata without extensibility
    + extfull: use full userdata with extensibility
    + table: use tables

The content of each directory is:

    + binder.h: implements the strategy to bind objects to Lua
    + hero.h: example of C++ class, Hero, mapped to Lua
    + actor.h: base class for Hero (only for full and table)
    + testcase.cpp: uses binder.h to map the class Hero to Lua
    + main.cpp: initializes the Lua library and the binding code
    + testcase.lua: Lua code for running the test case
    + testcase.exe: Windows executable

The code is in accordance with Lua 5.1. 
The executables were generated using Lua 5.1a. 
For rebuilding the programs, one should download the latest version of Lua (www.lua.org).

