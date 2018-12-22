12345678911234567892123456789312345678941234567895

      PRandGen : PseudoRandomNumberGenerator
          (C)2000 Guy W. Lecky-Thompson

             Developed for the book:
              Game Programming Gems
             Edited by Mark A. DeLoura

1 Purpose
PRandGen is a class developed to produce a seeded
series of pseudorandom numbers.  Two test harness
applications are provided by way of technique
demonstration.

2 Algorithm
In order to generate the numbers, the algorithm
used follows the principles described in the 
accompanying book, and those proposed in the book
"The Armchair Universe", by A.K. Dewdney, p.225.

3 Source Code
Two subdirectories hold the source code to both
the pseudorandom class and the test harness
applications, named "class" and "test".

4 Executables
The "bin" directory holds the executable files,
built under Microsoft Windows 95, "Prand.exe" and
"Serial.exe".

5 Prand
This test application has a command line interface
that requires as arguments :
   
    PrandGen <seed> <n_max> <n_runs>

Where <seed> is a seed value between 1 and the
maximum long integer value allowed by ANSI.

The <n_max> argument indicates the highest number
that may be generated.

The <n_runs> argument determines the number of
pseudorandom numbers to generate.

6 Serial
This application uses a technique for generating
a sequence of random numbers from a seed which
is arrived at using as input the serialisation
of an object in space.

Simply put, the seed is derived from the x and y
co-ordinates of an object in the given object 
space.  The aim is to demonstrate how an entire
game universe can be generated predictably.

In addition, it seeks to demonstrate how a given
object may be initialised without needing to 
regenerate the object space in its entirety, so
long as the position of the object in the object 
space is known.

The command line is:

    Serial <object_probability> <seed>

Where <object_probability> is the chance of an
object existing at a given point, and <seed> is
the identification of the object space.

