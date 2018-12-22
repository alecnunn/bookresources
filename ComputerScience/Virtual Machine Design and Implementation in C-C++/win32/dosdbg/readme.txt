Greetings Reader,

The Dosdbg code requires Visual C++ 1.52 to build. You could probably also build it with Borland C++ 4.5 or Turbo C.

Because of this, I did not include dosdbg in the normal build because the normal build uses VC 6.0+.

If you feel like experimenting, you can build this program as follows:

step 1) go to the bin directory and run the MSVCVARS.BAT batch file 
        ( this will set up your environment )
step 2) run CL  dosdbg.c


