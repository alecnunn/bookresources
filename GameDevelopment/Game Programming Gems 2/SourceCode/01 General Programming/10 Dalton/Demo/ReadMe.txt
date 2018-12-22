                       Game Programming Gems II
              -------------------------------------------
Gem    : A Drop-In Debug Memory Manager

Author : Peter Dalton
Email  : pdalton@xmission.com
URL    : http://www.xmission.com/~pdalton


Here is the code for the Memory Manager referred to Game Programming Gems II.  
The main files of interest are:

  MemoryManager.h  ............. Memory Manager header file.
  MemoryManager.cpp  ........... Memory Manager Implementation.
  new_on.h  .................... Header file for turning the Memory Manager on.
  new_off.h  ................... Header file for turning the Memory Manager off. 

All options and details of the memory manager are described extensively within 
the embedded comments of the files.

Included is also a small console checkers game that acts as a driver for the 
memory manager and allows for many of its benefits to be seen.  Take for example 
the Path.cpp file.  At the top of this file, you will find several flags:

 // For optimal performance turn all flags on except for USE_DYNAMIC_MEMORY
 #define USE_MEMORY_CACHE   1     // 1 - Enables  0 - Disables 
 #define CONSERVE_MEMORY    1     // 1 - Enables  0 - Disables
 #define USE_DYNAMIC_MEMORY 0     // 1 - Enables  0 - Disables

By toggling these flags on and off, running the program and examining the memory
manager log file you should be able to see the effects of each flag.  Each flag 
changes the implementation of the PathList class (a common data structure used 
extensively throughout the application).  For example, one implementation uses a 
memory cache to eliminate the need to always allocate new memory.  Turning this 
flag off, you should see that approximately twice as much memory is being used. 

This application along with the Memory Manager code has been tested on the Windows 
and Unix platforms.  A VC++6.0 workspace has also been included.

The checkers application uses a minimax search algorithm and incorporates 
alpha-beta pruning, plausible node ordering, and an iterative search.  A few of 
these techniques are discussed in the "Game Trees" article in Game Programming 
Gems I, written by Jan Svarovsky. 

Any questions or comments, please let me know.
