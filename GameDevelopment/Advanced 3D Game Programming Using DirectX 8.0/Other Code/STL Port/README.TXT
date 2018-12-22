STL port is a free, fast, multiplatform implementation of the STL based off of SGI's implementation.  It is considerably more efficient than the Visual C++ STL implementation (at memory management, in particular).  

Using the VC STL with the "Mobots Attack!" source code is prone to error, I found using STLPort made everything both faster and more robust.

First read the license agreement.  Unzip the provided file, and put the 'stlport' directory in some logical place.  Go to the tools->options dialog in Visual C and add the directory to the 'headers' area in the directories tab.  Make sure that the stlport directory is above the visual C entries on the list (use the arrows to move it around).  It's pretty plug and play; most code should compile without fuss.

The STLport web page is:  www.stlport.org

Peter Walsh (July, 2001)
Adrian Perez (May, 2000)