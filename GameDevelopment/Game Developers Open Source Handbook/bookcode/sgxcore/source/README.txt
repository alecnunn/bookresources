SGX Core - A cross-platform set of library abstractions for game development.
Copyright (C) 1992-2006 Steven Goodwin
Version 9.3


NOTES

Assuming you copy the entire contents of the source directory to c:\, you should
only need to open the project and add the additional include directories of,

  c:\source
  c:\source\libraries\lua\include

to your environment, and you'll be able to compile straight away.

The main\main.cpp file contains some small samples of the incorporated 
libraries in use.

The drivers directory contains the platform-specific drivers for each 
subsystem. Only the core is provided for the consoles, along with a
filesystem layer for WinTel32.


To develop for any of the consoles, first replicate the Null driver code into
a separate directory, and replace each instance with the appropriate platform-
specific versions. These can not be distributed with the SGX core due to NDAs,
but they are easy enough to write for yourself.



LICENSE

All files in this project fall under the terms of the GNU GPL, version 2, as
detailed at the top of main.cpp and in the file COPYING, with the following
exceptions:


Lua (libraries\lua\main, libraries\lua\lib, libraries\lua\include)
Released under the license found in lua.h
Taken from the source archive as supplied in software\chapter9\lua\lua-5.0.2.tar.gz

Quote:
Copyright (C) 1994-2004 Tecgraf, PUC-Rio.  All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.




Expat (libraries\expat\xmltok, libraries\expat\xmlparse)
Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006 Expat maintainers.
Released under the license found in COPYING
Taken from the source archive as supplied in software\chapter10\expat\expat-2.0.0.tar.gz

Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
                               and Clark Cooper
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006 Expat maintainers.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.





PCRE (sgx\core\regex\pcre)
Released under the license found in pcre.h
Taken from the source archive as supplied in bookcode\sgxcore\original\pcre-5.0.tar.gz)

Quote:
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.



Re: A C++ wrapper around pcre-3.2 (sgx\core\regex\pcre)
Available for any purpose (see README)
Taken from the source archive as supplied in bookcode\sgxcore\original\C++Wrapper.tar.gz)
Written by Hugo Etchegoyen

