This directory contains source code related to the article on
	Game Input Recording and Playback

For updates, visit:

	http://www.cygnus-software.com/

or send me e-mail at:

	bruced@humongous.com

To use this source code, load
	MicroThreadsAndInputRecording.dsw
into VisualC++, version 5.0 or higher and build the SimpleGame
project. This will automatically build and link all of the
necessary libraries.

SimpleGame is not really a game, it is more of a technology
demonstration and experimentation zone. It is written using
MFC for rapid prototyping. The libraries do not use MFC and
can be used directly in a game.

Relevant highlights of the code are:

	GameLibrary\microthreads.* - an implementation of micro threads
for VisualC++ on 80x86 processors. Most of the code is portable to
any processor.

	GameLibrary\microthreadmanager.* - an optional system for
managing micro threads.

	GameLibrary\gameobjects.* - a simple game object system that
uses micro threads or call backs.

	SimpleGame\janitorobjects.cpp - the Janitor and MicroThreadJanitor
classes referenced in the micro threads paper. Other objects are in
simplegameobjects.cpp. Some use micro threads, some use callbacks.

	Core\randomnumber.* - a random number class as required for
game input recording and playback.

	GameLibrary\gameinput.* - a partial implementation of an input
class. The details - such as saving and loading of input - are
left as an exercise for the reader :-)


To use SimpleGame just select objects to create from the menu.

Yes, there are some memory leaks. Sorry. I didn't get a chance to
fix them.
