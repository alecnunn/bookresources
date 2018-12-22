FSM_Projects ReadMe

Description:
The FSM projects contains the source code for the Function Pointer-Based Embedded Finite State
Machines Gem in the Game Programming Gems III book.  

Files:
FSM_Static				The original CFSM classes (static library)
FSM_DLL				The original CFSM classes (DLL)
FSM_Static_Cast			An alternative version of the CFSM classes (static library)
FSM_DLL_Cast			An alternative version of the CFSM classes (DLL)
Demo					CLightBulb and CFlashingLightBulb examples

Release Notes:
The CFSM classes come in two different versions.  The original version is the source code
described in the book.  The second version has been optimized for lower memory usage (by 
removing pointers and allows CStateTemplate instances to be made static).  While the second 
version is much more memory efficient, it is slightly slower than the original CFSM 
implementation do the the necessity of passing the class instance and casting it to the 
appropriate type at run-time.

Charles Farris
charlesf@vr1.com