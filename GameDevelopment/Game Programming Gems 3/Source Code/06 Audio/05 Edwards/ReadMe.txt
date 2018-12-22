Codec.exe
=========

LPC Vocoder Demo
================

Codec.exe is a simple test program which demonstrates the LPC algorithm.  Double-click on the program to start; it requires DirectSound to operate.

Voice Files
-----------

The top part of the window is for voice files.  These are either .WAV files (any uncompressed sample) or .QVX files (a preprocessed voice file).

Browse allows you to load a .WAV or .QVX file - this is processed on loading, which might take some time (the status bar shows progress).

Dump saves a human-readable dump of the sound as a .TXT file.

Save saves a .QVX file - this does not include the "residual" signal, only the LPC coefficients, pitch and volume information (500 bytes/sec).

Clear clears the input and gives you sweet, sweet silence.

Output Selection
----------------

When a .WAV file is processed, it is by default played back using the residual - this reconstructs the original signal (more or less!).  The three buttons then permit selection of the resynthesized speech or the residual alone.

By deselecting "Use residual", the synthesizer is activated instead and the robotic effects occur.  Now, the three buttons allow you to hear the full speech, just the synthesizer output, or just the 2nd FM oscillator.

Rate
----

Changing the rate and clicking Apply will speed up or slow down the playback of speech.

Speaker
-------

The "speaker" is actually a 2-operator FM synth.  Select Modulator or Carrier parameters - these are different "pages".

Each oscillator has a waveform which you choose.  Pitch can be derived from the voice data, it can be a constant, or it can be a combination of the two.  If you select "ratio" the pitch is locked to the pitch of the other oscillator, in the specified ratio.  [Hint: select ratio for the modulator but not for the carrier].

The modulation amount increases the FM multiplier.  At zero, there is no FM and the Modulator parameters are irrelevant.  (On the Modulator page this slider is "feedback amount" and controls feedback).

Amplitude of the oscillator is specified next.  This can be a constant, it can be derived from the voice data, or it can be a combination.  For the Carrier, this is just the loudness.  For the Modulator, the amount determines how much FM occurs dynamically, so the volume of the voice data can affect the FM tone.

Deep/Shallow makes the filter longer/shorter.

Browse allows you to load .SPK files describing speakers

Dump saves a human-readable description of the speaker

Save saves the speaker as a .SPK file

Enjoy!

Eddie Edwards
