Game Programming Gems 3
Template-Based Object Serialization
Author: Jason Beardsley <jbeardsley@ncaustin.com>
January 25, 2002


MANIFEST

src/    Source for serialization library and a test driver
stl/    Place to put the appropriate STL (see below)
build/  Build directories for tested platforms


STL

Many compilers do not ship with a fully functional STL, and even so
the non-standard hashed containers are not consistent across different
STL implementations.  I highly recommend that you go out and get a
"good" one, either STLport (www.stlport.org) or the SGI one
(www.sgi.com/tech/stl).  Then, install it in the "stl" directory,
where the make files will search for it.

Due to potential redistribution restrictions that I'm not willing to
even research, I cannot supply you with a good STL.  If it's not
possible for you to download and use an alternate STL, you might have
to remove or rework the support for the hashed containers.


PLATFORMS

- Linux/x86

  * RedHat Linux 7.1 / gcc 2.96 + SGI STL v3.3

    The stock gcc 2.96 STL does not work, and if you use it you should
    really switch to something newer anyway - the default string class
    is not thread-safe and will core dump in threaded code.

  * RedHat Linux 7.2 / gcc 3.0.2

    The provided STL works fine.  Use 'make gcc3-all'.

- NetBSD/sparc

  * NetBSD 1.5 / gcc 2.95.2 + SGI STL v3.3

    You also have to use GNU make (gmake) to build, or redo the
    Makefile.  I tested this on a SparcStation 5, which is not a
    64-bit CPU.  I'm interested in finding out if there are any
    changes needed to make it work on a true 64-bit architecture.

- Win32

  * Windows 2000 / MSVC6 (SP5) + STLport 4.0

    A workspace (serialize.dsw) is provided.  Install STLport before
    even thinking about compiling.

  * Windows 2000 / cygwin gcc 2.95.3 + SGI STL v3.3


TEST RESULTS

I've included the output from the test driver for each platform that I
tested on.  There should be no differences between them, except for
one line in test5(), where a float64 is written into a Buffer without
byteorder conversion; this will differ between x86 and sparc.


BUG REPORTS

Please send any bug reports, patches, comments, or enhancements to me
via email, with "GEM3" in the header.  I'll try to put up some kind of
public site where fixes can be accessed, if there is any interest.
