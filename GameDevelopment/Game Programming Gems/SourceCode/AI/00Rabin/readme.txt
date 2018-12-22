Author: Steve Rabin
E-mail: stevera@noa.nintendo.com
Last revised: 5-20-00

This MS Visual C++ project is the AI engine described in the book Game 
Programming Gems within the article "Designing a General Robust AI Engine". 
Please refer to the book for a full explanation of the implementation. 
Comments within the code also provide additional information.

The state machine language uses a few custom keywords that should be 
highlighted by Visual C++ in order to make the state machine more 
readable. You can get Visual C++ to highlight these words by placing the 
file "usertype.dat" (included in this directory) into the same directory 
as Msdev.exe (C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin). 
The file "usertype.dat" is simply a text file listing the state machine 
keywords.

Here is a brief overview of the state machine structure.

Each game object can point to a state machine. When that game object 
receives a message, it gets routed to it's state machine. The state machine 
can execute any code on behalf of the game object and can send additional 
messages, delayed messages (timers), can change state, or can destroy 
itself. Actually, each state machine can execute any code within your 
game. It is completely general.

The state machine language was designed to provide a simple, consistent, 
robust, and general implementation of a state machine to every game 
object. The language itself consists solely of macros that have been 
carefully crafted to fit together like puzzle pieces. You can see an 
example in the file "fsm_drone.cpp". The macro language can be found in 
the file "fsmmacros.h".

The AI engine provided was ported from C++ to C in order to show that it 
doesn't rely on any object-oriented techniques and that its generally 
applicable (also for legal reasons). I would highly advise porting it back 
to C++ so that it can take advantage of object-oriented techniques (even 
though it doesn't require them).

Good Luck!

