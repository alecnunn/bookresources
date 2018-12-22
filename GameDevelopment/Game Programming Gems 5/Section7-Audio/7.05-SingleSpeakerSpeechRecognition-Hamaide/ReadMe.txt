_________________
SpeechRecogn v1.0
_________________

This application tries to recognize a word from a dictionary.


-------------------------
Creating the dictionary
-------------------------

To use it, you need to register each word several times. 
To do so, record a word with an audio tool. Save it as a 44100Hz, 16 bit, mono PCM wav file.

Than for each file, call the program with these arguments:
:
SpeechRecogn wavFile dictionaryFile add word

ex : SpeechRecogn hello1.wav dictionary.dct add hello

When the dictionary is ready, you can start to recognize words.


----------------------------------
Recognize words (from wav files)
----------------------------------

It's easy; just call the program like this

SpeechRecogn wavFile dictionaryFile test

ex : SpeechRecogn hello2.wav dictionary.dct test




-----------------------------------
Problems
-----------------------------------

If the program does not give acceptable results, 
think about adding more references in the dictonary.


Julien Hamaide





