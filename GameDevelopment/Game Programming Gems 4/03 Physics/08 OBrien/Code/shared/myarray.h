/*********************************************************************
 * myarray.h
 * Authored by Kris Hauser 2002-2003
 *
 * A templated array class with variable growth size.
 *
 * Copyright (c) 2002,2003 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/



#ifndef MY_ARRAY_H
#define MY_ARRAY_H

#include "stdincludes.h"

template <class type>
class myarray
{
public:
	myarray();
	myarray(int size);
	~myarray() { clear(); }

	inline int size() const { return cursize; }		//this is the "official" size
	inline int capacity() const { return maxsize; }	//this is the amount of memory allocated
	void resize(int size);				//this will resize only if size > maxsize, and copies items
	void resize_strict(int size);		//this forces a resize, no matter what size is given, and doesnt copies items
	void resize_strict_copy(int size);	//this forces a resize, and does copy items
	void clear();

	void set_grow_size(int gs) { growsize = gs; }

	void operator = (const myarray<type>&);
	inline operator type* () { return list; }
	inline operator const type* () const { return list; }
	inline type& operator [] (int i) { return list[i]; }
	inline const type& operator [] (int i) const { return list[i]; }

private:
	type* list;
	int cursize;
	int maxsize;
	int growsize;
};

template <class type>
myarray<type>::myarray()
:list(NULL), cursize(0), maxsize(0), growsize(1)
{}

template <class type>
myarray<type>::myarray(int size)
:list(NULL), cursize(0), maxsize(0), growsize(1)
{
	resize(size);
}

template <class type>
void myarray<type>::resize(int size)
{
	if(size > maxsize)
	{
		int newsize = max(size, maxsize+growsize);
		resize_strict_copy(newsize);
	}
	cursize = size;
}

template <class type>
void myarray<type>::resize_strict(int size)
{
	if(size != maxsize)
	{
		type* newlist = new type[size];
		SafeArrayDelete(list);
		list = newlist;
	}
	maxsize = size;
	cursize = size;
}

template <class type>
void myarray<type>::resize_strict_copy(int size)
{
	if(size != maxsize)
	{
		type* newlist = new type[size];
		int ncopy = min(size, cursize);
		for(int i=0; i<ncopy; i++)
			newlist[i] = list[i];
		SafeArrayDelete(list);
		list = newlist;
	}
	maxsize = size;
	cursize = size;
}

template <class type>
void myarray<type>::clear()
{
	SafeArrayDelete(list);
	maxsize = 0;
	cursize = 0;
}

template <class type>
void myarray<type>::operator = (const myarray<type>& l)
{
	if(maxsize < l.cursize)
		resize_strict(l.cursize);
	cursize = l.cursize;
	for(int i=0; i<cursize; i++)
		list[i] = l.list[i];
}

#endif

