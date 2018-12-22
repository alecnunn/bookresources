INTRODUCTION
The software in this package constitutes a sample program
that is intended to demonstrate the principles discussed 
in the articles:
  
  Precise Game Event Broadcasting with Python
  Creating a "Safe Sandbox" for Game Scripting

In the accompanying book, "Massively Multiplayer Game Development".

The two articles were originally written as independent
representations, but in constructing the sample program, it
became clear that both concepts would benefit from a consistent
treatment within the same code base. 


FEATURES
The program is a simple text-based multiplayer game server,
similar to a MUD. The key features of interest include:

  * An asynchronous event broadcasting framework that allows
    interested code to register for events using very 
    precise criteria, minimizing the overhead of polling 
    for state changes and testing for the relevance of an
    event that has been received.

  * A framework for creating a "safe sandbox" in which game
    designers and other non-technical scripters may feel
    free to push the envelope of creativity without fear of
    undermining the stability of the server with coding 
    errors or a less than perfectly engineered implementation.

To showcase the above features, an interactive, game-like 
environment was developed to illustrate both how these 
features work, and how to write code that makes use of them.
The secondary features that achieve this include:

  - Multi-user text-based (i.e. telnet) client support.
  - A "world" with 3 distinct "rooms" that players can enter.
  - A small set of trivial game behaviors, including a 
    player inventory, the ability to take objects from the
    world, and the ability to engage in combat with other
    players.
  - A limited text-based command set that supports the 
    game play, and navigation within the "world".


FILES
The files included in this package are listed and described
below:

/ <top-level> 
README                -- This file
gameserverdemo.py     -- The "main" program that drives the server.

/gameserver
__init__.py           -- Special empty file used by Python
deferred.py           -- Deferred call implementation [1]
eventbroadcaster.py   -- Event broadcaster implemenation [1]
gameeventkeys.py      -- Event keys for precise registration [1]
gameevents.py         -- Module of event id constants [2]
gameobs.py            -- Module containing a variety of game objects
gameserver.py         -- Core server framework, based on asyncore and asynchat [3]
objmgr.py             -- Object manager that allows addressing objects by an id [2]
request.py            -- Request handler and processor of input commands
safesandbox.py        -- Safe Sandbox implementation [1]
sandboxrexec.py       -- Restricted execution implementation [1]

/gameserver/sandboxmodules
__init__.py           -- Special empty file used by Python
battle.py             -- A sandbox that provides an arena for combat [2]
thetrap.py            -- A sandbox that demonstrates prevention of prohibited operations [2]
town.py               -- A sandbox that implements a "social space" [2]

Notes:
[1] The code in these files implement the core concepts described
    in the articles. Refer to them for implementation details referred
    to in the text, and for comments that provide further explanation.
    This code is almost identical to the listings in the articles, 
    except where variable or module names had to be changed in order
    to enable the code to work together in the same process.

[2] These files represent concepts that either use the core concepts,
    or facilitate their implementation. They are briefly alluded to 
    in the article text in an abstract fashion. Their implementation 
    details will differ from the examples in the articles, but they
    should illustrate the concepts mentioned in them.

[3] The asyncore.py and asynchat.py Python modules provide a useful 
    framework for building asynchronous server applications. They are
    excellent for quickly prototyping concepts such as those 
    presented by this demo.


RUNNING THE DEMO
To run the demo, you must have Python installed on your system.
This code has been tested with Python 2.2, but it should work
fine with anything from 2.0 and later. The code should work
on both Windows and Linux, as no platform-specific features
were used.

To start the server, change directory to the directory 
containing this file. Then, run it with the following 
command:

$ python gameserverdemo.py <port>

where <port> is the port number on which the server will listen
for client connections.

To connect to the server as a client, use any telnet client
to connect to the server's listening port. For example, if
you started the server on port 2112, connect with:

$ telnet <hostname> 2112

where <hostname> is the name of the computer on which the 
server is running.

Once connected, type 'help' for a list of commands, and 
play around with it.


CAVEAT EMPTOR
The code presented in this project is for demonstration purposes
only. Care was taken to ensure that the core concepts described
in the articles were accurately implemented. However, it should 
be recognized that in the interest of simplicity, only limited 
attention was given to error handling. In addition, the code in
this demo represents only one of many possible implementations of
the concepts described in the article. You should consider your 
project's needs and architectural constraints when applying 
these principles in your own implementation.


CONTACT INFORMATION
I welcome questions and comments from readers. Please direct 
them to: 

Matt Walker 
mwalker@softhome.net






