
April 21, 2001

First of all I have to thank Phil and Dan for helping to make this a really cool demo!  Phil is a software synthesis expert that I met about 7 years ago when he was doing the excellent audio engine for the 3DO.  Dan is a music composer and sound designer that I met about 5 years ago at Midway and he created all of the cool audio content in the Mortal Kombat video games.

I also have to thank Mark Deloura for being up for the idea of an audio section for GPGems2.  When many people think of game programming, audio code is not commonly the first thing that comes to mind.  But there are a lot of great ways to integrate audio response with game response and I hope my gems in this book shed some light on the subject and spark some new ideas and interest.

Basic Sequencer

Just run the BasicSeq.exe and it will load the AIFF files and MIDI file needed as it starts.  As long as the demo runs close to 30 FPS on your machine, everything should sound good.  Press the Spacebar to start and stop the tune.  Press the 'M' key to cycle through various output display modes.  Press the 'S' key to toggle the output display enable.  Press the Escape key to exit the demo.

In the code, the MusicSequencerSystem_t object is your man.  The public interface provided in this class is all that the code in BasicSeq.cpp needs to run the audio.  Start from there and then delve deeper to get to the nitty gritty.

Note on code other file contents:
Code from the book is abbreviated from MusicSys.h, Sequence.h, Track.h, and Voice.h.
Code for reading MIDI files is in SequenceData.h, TrackData.h, and EventData.h.
This demo is programmed to run under Win32 operating systems and a project file for Microsoft Visual C++ 6.0 is included.  The music sequencer code and CSyn are both very portable.

Disclaimer:

It's Demo Code!

Note that this is a demo of music sequencer programming and is not recommended to be put
in to a production environment without additional sequencer features, error reporting, data validation, and optimizations.

Although it works with the .AIFF and .MID files provided with this demo, keep in mind that
these files have been prepared for this demo with certain restrictions in mind.

Enjoy!

Scott Patterson

website:    www.tonebyte.com
email:   scottp@tonebyte.com


