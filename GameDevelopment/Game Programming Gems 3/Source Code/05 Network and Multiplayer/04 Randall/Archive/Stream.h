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

#ifndef	_Stream_H
#define	_Stream_H

//---------------------------------------------------------------------
#pragma warning ( disable : 4786 ) // dentifier was truncated to '255' characters in the debug information
#include <cassert>
#include <string>

//---------------------------------------------------------------------

namespace Archive
{

//---------------------------------------------------------------------
/** \class Stream Stream.h "Archive/Stream.h"
	@brief A byte stream
*/
class Stream
{
public: // ctor/dtors
	Stream();
	Stream(const unsigned char * const buffer, const unsigned int bufferSize);
	Stream(const Stream & source);
	virtual ~Stream();

public: // inner classes
	/** \class ReadIterator Stream.h "Archive/Stream.h"
		@brief an iterator object used to retrieve data from a Stream

		The ReadIterator is employed to coordinate multiple consumers of 
		a Stream object without altering the Stream itself. It is a const-like
		iterator that will not change the contents of a Stream. Multiple Stream
		consumers may access data in the Stream using ReadIterators.

		@author Justin Randall
	*/
	class ReadIterator
	{
	public:
		                            ReadIterator    ();
		                            ReadIterator    (const ReadIterator & source);
		explicit                    ReadIterator    (const Stream & source);
		                            ~ReadIterator   ();

		ReadIterator &              operator =      (const ReadIterator & source);
		void                        advance         (const unsigned int distance);
		void                        get             (void * target, const unsigned long int readSize);
		const unsigned int          getSize         () const;
		const unsigned char * const getBuffer       () const;
		const unsigned int          getReadPosition () const;
	private:
		
		unsigned int readPtr;
		const Stream *     stream;
	};

public:
	Stream &                    operator = (const Stream & source);
	ReadIterator                begin() const;
	void                        clear();
	const unsigned char * const getBuffer() const;
	const unsigned int          getSize() const;
	void                        put(const void * const source, const unsigned int sourceSize);

private:
	void                        get(void * target, ReadIterator & readIterator, const unsigned long int readSize) const;
	void                        growToAtLeast(const unsigned int targetSize);
	void                        reAllocate(const unsigned int newSize);

private: // inner classes
	class Data
	{
	public:
		Data();
		explicit Data(unsigned char * buffer);
		~Data();
		const int getRef () const;
		void      deref  ();
		void      ref    ();
	protected:
		friend class Stream;
		friend class ReadIterator;
		unsigned char * buffer;
	private:
		int             refCount;
	};

private:
	friend class ReadIterator;
	unsigned int                allocatedSize;
	Data *                      data;
	unsigned int                size;
}; //lint !e1934

//---------------------------------------------------------------------

inline Stream::Data::Data() :
buffer(0),
refCount(1)
{
}

//---------------------------------------------------------------------

inline Stream::Data::Data(unsigned char * newBuffer) :
buffer(newBuffer),
refCount(1)
{
}

//---------------------------------------------------------------------

inline Stream::Data::~Data()
{
	refCount = 0;
	delete[] buffer;
}

//---------------------------------------------------------------------

inline void Stream::Data::deref()
{
	refCount--;
	if(refCount < 1)
		delete this;
}

//---------------------------------------------------------------------

inline const int Stream::Data::getRef() const
{
	return refCount;
}

//---------------------------------------------------------------------

inline void Stream::Data::ref() 
{
	refCount++;
}

//---------------------------------------------------------------------

inline Stream::ReadIterator & Stream::ReadIterator::operator = (const Stream::ReadIterator & rhs)
{
	if(&rhs != this)
	{
		readPtr = rhs.readPtr;
		stream = rhs.stream;
	}
	return *this;
}

//---------------------------------------------------------------------

inline void Stream::ReadIterator::get(void * target, const unsigned long int readSize)
{
	assert(stream);
	stream->get(target, *this, readSize);
	readPtr += readSize;
}

//---------------------------------------------------------------------

inline const unsigned int Stream::ReadIterator::getSize() const
{
	assert(stream);
	return stream->getSize() - readPtr;
}

//---------------------------------------------------------------------

inline Stream::ReadIterator Stream::begin() const
{
	return ReadIterator(*this);
}

//---------------------------------------------------------------------

inline void Stream::ReadIterator::advance(const unsigned int distance)
{
	readPtr += distance;
}

//---------------------------------------------------------------------

inline const unsigned int Stream::ReadIterator::getReadPosition() const
{
	return readPtr;
}

//---------------------------------------------------------------------

inline const unsigned char * const Stream::ReadIterator::getBuffer() const
{
	if(stream)
		return &stream->data->buffer[readPtr];

	return 0;
}

//---------------------------------------------------------------------
/**
	@brief clear read and write settings for the Stream

	This does not free allocated memory. Instead, it keeps the Stream
	buffer and sets the read and write pointers back to the beginning
	of the Stream

	@author Justin Randall
*/
inline void Stream::clear() 
{
	size = 0;
}

//---------------------------------------------------------------------
/**
	@brief get the whole stream buffer (very const)
*/
inline const unsigned char * const Stream::getBuffer() const
{
	return data->buffer;
}

//---------------------------------------------------------------------
/**
	@brief Get the size, in bytes, of the Stream buffer

	@return the size, in bytes, of the Stream buffer

	@author Justin Randall
*/
inline const unsigned int Stream::getSize() const
{
	return size;
}

//---------------------------------------------------------------------
/**
	This private method ensures that the Stream buffer is large enough
	to receive data at least as large as the requested targetSize.

	@param unsigned int targetSize   The desired size to increase the
	                                 Stream buffer.

	@author Justin Randall
*/

inline void Stream::growToAtLeast(const unsigned int targetSize)
{
	if(allocatedSize < targetSize)
	{
		reAllocate(allocatedSize + allocatedSize + targetSize);
	}
}

//---------------------------------------------------------------------

} // namespace Archive
//---------------------------------------------------------------------

#endif	// _Stream_H

