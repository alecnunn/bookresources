-----------------------------------------------------------------------------
Faking DSP Effects: Sample Program
All files Copyright (C) Robert Sparks, 2005
-----------------------------------------------------------------------------

Application
-----------

The included sample program demonstrates how DSP effects can be faked by
playing specially prepared multi-channel files.  Please double-click 
bin/fakingdsp.exe.

Run the program and move the slider around to hear what happens as the 
listener moves across the screen.  The red "L" represents the listener,
the blue square represents a piano/radio, and the black square represents the 
walls of a room.

When the listener enters the room, the sound of the piano is clear and loud.
When the listener leaves the room, the sound of the piano becomes distant and
reverberates.  This reverb effect is not being done with hardware.  It's 
being faked.

Please refer to the code for implementation details. 


Source WAV File
---------------

The included WAV file (Brahms Intermezzo in B-Flat Minor, Op. 117, No. 2.wav)
has been specially prepared for this sample.  The left channel contains a 
song being played on the piano.  The right channel contains the 
same song, only it has been processed by a low-pass filter and a strong 
reverb effect.  In the sample program, it is the left channel that we hear 
when the listener is inside the room with the piano and it is the right 
channel that we hear when the listener is outside the room.  Piano played
by Robert Sparks.








