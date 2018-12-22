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

//---------------------------------------------------------------------
#include "ArchivePCH.h"
#include <cassert>
#include "Stream.h"

namespace Archive {

//---------------------------------------------------------------------
/**
	@brief ReadIterator ctor

	Initializes the read position to zero, and the stream member value
	is NULL
*/
Stream::ReadIterator::ReadIterator() :
readPtr(0),
stream(0)
{
}

//---------------------------------------------------------------------
/**
	@brief ReadIterator copy constructor

	Sets the current position to the source read position and the
	stream pointer to the source stream.
*/
Stream::ReadIterator::ReadIterator(const ReadIterator & source) :
readPtr(source.readPtr),
stream(source.stream)
{
}


//---------------------------------------------------------------------
/**
	@brief construct a read iterator with the position at zero and
	the stream set to the source.
*/
Stream::ReadIterator::ReadIterator(const Stream & source) :
readPtr(0),
stream(&source)
{
}

//---------------------------------------------------------------------
/**
	@brief destroy a read iterator object, set the stream member to 
	zero.
*/
Stream::ReadIterator::~ReadIterator()
{
	stream = 0;
}

//---------------------------------------------------------------------
/**
	Construct an Stream object.

	This constructor merely initializes members, No buffer allocation
	occurs.

	@author Justin Randall
*/
Stream::Stream() :
allocatedSize(0),
data(new Data(0)),
size(0)
{
}

//---------------------------------------------------------------------
/**
	@brief Construct an Stream from a byte buffer

	@author Justin Randall
*/
Stream::Stream(const unsigned char * const newBuffer, const unsigned int bufferSize) :
allocatedSize(bufferSize),
data(0),
size(bufferSize)
{
	unsigned char * buffer = new unsigned char[size];
	data = new Data(buffer);
	memcpy(buffer, newBuffer, size);
}

//---------------------------------------------------------------------
/**
	Stream Copy Constructor

	Deep copy an Stream.
*/
Stream::Stream(const Stream & source):
allocatedSize(source.getSize()),	// only allocate what is really there, be opportinistic when grow()'ing
data(source.data),
size(source.getSize())
{
	source.data->ref();
}

//---------------------------------------------------------------------
/**
	Stream destructor

	Deletes the assocated buffer and resets allocateSize, buffer, and 
	size to zero.

	@author Justin Randall
*/
Stream::~Stream()
{

	data->deref();
	allocatedSize = 0;
	data = 0; //lint !e672 (data deref insures the data is deleted if no one references it)
	size = 0;
}

//---------------------------------------------------------------------
/**
	Assignment operator

	If the Stream right hand side is not the same Stream, performs
	a deep copy of the Stream.

	@author Justin Randall
*/
Stream & Stream::operator=(const Stream & rhs)
{
	if(this != &rhs)
	{
		data->deref();
		rhs.data->ref();
		allocatedSize = rhs.allocatedSize;
		size = rhs.size;
		data = rhs.data; //lint !e672 (data is ref counted)
	}
	return *this;
}

//---------------------------------------------------------------------
/**
	@brief Accesses stream data

	@param target       a user supplied buffer that must be at least as
	                    large as targetSize
	@param readIterator An accessor that indicates where the read operation
	                    will begin
	@param targetSize   specifies the number of bytes to read from the 
	                    Stream

	@author Justin Randall
*/
void Stream::get(void * target, ReadIterator & readIterator, const unsigned long int targetSize) const
{
	assert(readIterator.getReadPosition() + targetSize <= allocatedSize);
	memcpy(target, &data->buffer[readIterator.getReadPosition()], targetSize);
}

//---------------------------------------------------------------------
/**
	@brief deep copy new data to the stream

	If the data member is shared with another stream, then it is 
	copied before this operation begins. The Stream buffer is increased
	to hold the new data (determined by the requested sourceSize). 

	@param source      a user supplied buffer that contains data to be
	                   copied to the Stream
	@param sourceSize  The number of bytes in the source buffer which 
	                   will be copied to the stream.

	@author Justin Randall
*/
void Stream::put(const void * const source, const unsigned int sourceSize)
{
	if(data->getRef() > 1)
	{
		// about to write new data. If data is currently shared
		// with other objects, then decrement the ref count and
		// create a new local data to write to.
		data->deref();
		const unsigned char * const tmp = data->buffer;
		data = new Data(new unsigned char[size + sourceSize]); //lint !e672 (data is ref counted)
		memcpy(data->buffer, tmp, size);
		allocatedSize = size + sourceSize;
	}
	growToAtLeast(size + sourceSize);
	memcpy(&data->buffer[size], source, sourceSize);
	size += sourceSize;
}

//---------------------------------------------------------------------

void Stream::reAllocate(const unsigned int newSize)
{
	allocatedSize = newSize;
	unsigned char * tmp = new unsigned char[newSize];
	if(data->buffer != NULL)
		memcpy(tmp, data->buffer, size);
	delete[] data->buffer;
	data->buffer = tmp;
}

//---------------------------------------------------------------------

}//namespace Archive

//---------------------------------------------------------------------
