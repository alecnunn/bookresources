====================================================================
Fast and Efficient Implementation of Remote Procedure Call System
====================================================================

This sample code consists of these project files:

 antlrlib
 IDLComp
 MyRPC
 SimpleTest
 RPCDuel

All of these files are tested and verified with Visual Studio or C++.net 2003.
However, contents of codes are written considering platform independence, except for RPCDuel.


------------------
antlrlib
------------------

antlrlib is a refined version of the original ANTLR C++ library 2.7.4, for convenient build
for Visual C++.net 2003. antlr.jar is ANTLR 2.7.4 runtime which takes grammar file and 
generates parser files in C++ or java.

ANTLR is copyrighted by Terence Parr. http://www.antlr.org/


------------------
IDLComp
------------------
IDLComp is a compiler which takes an IDL file and generates proxy and files
for our own RPC implementations.

IDLComp consists of two parts: front end and back end. The front end exists
in IDL.g,IncludeGeneratedFiles.cpp,IncludeGeneratedFiles.h,Specification.cpp and
Specification.h. The back end exists Backend.cpp and Backend.h.

Java Runtime Environment 1.4(http://java.sun.com) and antlrlib are needed to compile 
this project, however, Java Runtime Environment is not needed to run IDLComp.exe.

------------------
MyRPC
------------------

MyRPC is a library consists of helper functions, typedefs and so on.
It is needed to add this library to your project when you adopt 'our own
RPC implementation.'

The core source files are MyRPC.cpp and MyRPC.h.

------------------
SimpleTest
------------------

SimpleTest is the simplest 'Hello World' example of MyRPC.
Check out SimpleTest.cpp and Test.IDL for examining how our own RPC can be
used.

MyRPC and IDLComp.exe are needed to build this project. 
IDLComp.exe already exists in this project, however, you may want to build 
it for yourself.

------------------
RPCDuel 
------------------

This program is a modified version of Microsoft DirectX Sample 'Duel.'
It functions as the original version of Duel, except for its internal networking
messages are simplified by MyRPC.

Search for the comment 'before RPC' for the change before and after usage of RPC.
An outstand file is RPCDuel.idl, which declares RPC functions replacing legacy Duel
game messages.

MyRPC and IDLComp.exe are needed to build this project. 
IDLComp.exe already exists in this project, however, you may want to build 
it for yourself.

RPCDuel seems to use older version of DirectX, however, its base source Duel was 
copied from DirectX9 SDK.

This program requires less than 1 Meg of video ram.

The commands which this game recognizes are listed on the opening screen
and in the help dialog (F1).

    ENTER	    - Starts game and multiplayer session setup
    LEFT ARROW	    - Turn left
    RIGHT ARROW	    - Turn right
    UP ARROW	    - Accelerate forward
    DOWN ARROW	    - Accelerate backward
    NUMPAD 5	    - Stop moving
    SPACEBAR	    - Fire
    ESC, F12        - Quit
    F1              - Help - list of commands
    F5              - toggle frame rate display


To play multiplayer one machine hosts a game, the others join.

To play using TCP/IP over the Internet, the people who are joining the
game must enter the IP address of the machine that hosted the game.
You can find out the IP address of your machine by running "winipcfg".
If you have a net card and a modem installed, you will need to make
sure you read the IP address of the modem connection to the Internet.
The modem will show up as a "PPP Adapter".  DirectPlay will not work
through proxies or firewalls.

This game allows the selection of which DirectPlay messaging protocol
is used.  The host machine can select the optimized DirectPlay
guaranteed protocol for the game session in the setup wizard.  Reliable
and asynchronous messaging, when supported by the service provider or
by using the DirectPlay Protocol, can be toggled on and off with the R
and A keys.  The protocol status will be displayed in the caption bar.

Also see the DPLAUNCH sample as an example of lobby launching this game.

Note: MSVC may include older versions of the DirectX header files and
libraries.  In order to avoid compile errors, make sure the path to the
latest DirectX header files and libraries are listed BEFORE the MSVC header
files and libraries through the Tools -> Options -> Directories menu.

In order to support multi-byte character sets like Kanji, the dialogs in the
rc file need to be changed to use the "SYSTEM" font and the rc file needs to
be recompiled.







