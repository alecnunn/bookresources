/*
Copyright (c) 2001 by Justin Randall.  All rights reserved.
This copyright applies only to this particular software distribution 
package, and does not imply a copyright on any particular module in 
the library.

Permission to use, copy, modify, and distribute this compilation for
non-commercial use is hereby granted without fee, subject to 
the following license:

1. Any copy or modification of this compilation must include the above
copyright notice and this license.

2. Users of the software included in this compilation agree to use
their best efforts to provide Justin Randall with any modifications containing
improvements or extensions and hereby grant Justin Randall a perpetual,
royalty-free license to use and distribute such modifications under
the terms of this license.

3. Justin Randall makes no warranty or representation that the operation of the
software in this compilation will be error-free, and Justin Randall is under no
obligation to provide any services, by way of maintenance, update, or
otherwise.  THE SOFTWARE AND ANY DOCUMENTATION ARE PROVIDED 'AS IS'
WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT WILL JUSTIN RANDALL OR ANY OTHER CONTRIBUTOR BE LIABLE FOR
DIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

4. Users will not use Justin Randall or any other contributor's name in any 
publicity or advertising, without prior written consent in each case.

5. Export of this software from the United States may require a
specific license from the United States Government.  It is the
responsibility of any person or organization contemplating export
to obtain such a license before exporting.

6. Certain parts of this software may be protected by patents.  It
is the users' responsibility to obtain the appropriate
licenses before using those parts.
*/

#ifndef	_Archive_H
#define	_Archive_H

//---------------------------------------------------------------------

#include "Stream.h"

//---------------------------------------------------------------------

namespace Archive {

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, double & target)
{
	source.get(&target, 8);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, float & target)
{
	source.get(&target, 4);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, unsigned long int & target)
{
	source.get(&target, 4);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, signed long int & target)
{
	source.get(&target, 4);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, unsigned int & target)
{
	source.get(&target, 4);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, signed int & target)
{
	source.get(&target, 4);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, unsigned short int & target)
{
	source.get(&target, 2);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, signed short int & target)
{
	source.get(&target, 2);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, unsigned char & target)
{
	source.get(&target, 1);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, signed char & target)
{
	source.get(&target, 1);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, char & target)
{
	source.get(&target, 1);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, std::string & target)
{
	target = reinterpret_cast<const char *>(source.getBuffer());
	if(target.length() > source.getSize())
	{
		// input did NOT include a NULL!
		target[source.getSize()] = 0;
	}
	source.advance(target.length() + 1);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, bool & target)
{
	source.get(&target, 1);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, Stream & target)
{
	target.put(source.getBuffer(), source.getSize());
	source.advance(source.getSize());
}

//---------------------------------------------------------------------

inline void put(Stream & target, const double & source)
{
	target.put(&source, 8);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const float & source)
{
	target.put(&source, 4);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const unsigned long int & source)
{
	target.put(&source, 4);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const signed long int & source)
{
	target.put(&source, 4);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const unsigned int & source)
{
	target.put(&source, 4);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const signed int & source)
{
	target.put(&source, 4);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const unsigned short int & source)
{
	target.put(&source, 2);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const signed short int & source)
{
	target.put(&source, 2);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const unsigned char & source)
{
	target.put(&source, 1);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const signed char & source)
{
	target.put(&source, 1);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const char & source)
{
	target.put(&source, 1);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const std::string & source)
{
	const char c = 0;
	target.put(source.c_str(), source.size());
	target.put(&c, 1);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const bool & source)
{
	target.put(&source, 1);
}

//---------------------------------------------------------------------

inline void put(Stream & target, Archive::Stream::ReadIterator & source)
{
	target.put(source.getBuffer(), source.getSize());
	source.advance(source.getSize());
}
//---------------------------------------------------------------------

inline void put(Stream & target, const Stream & source)
{
	target.put(source.getBuffer(), source.getSize());
}

//---------------------------------------------------------------------

}//namespace Archive

//---------------------------------------------------------------------

#endif	// _Archive_H
