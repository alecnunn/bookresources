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

#ifndef	_DeltaArchive_H
#define	_DeltaArchive_H

//---------------------------------------------------------------------

#include "AutoArchive.h"
#include <set>

//---------------------------------------------------------------------

namespace Archive {

class DeltaVariableBase;

//---------------------------------------------------------------------
/**	
	\class DeltaArchive DeltaArchive.h "Archive/DeltaArchive.h"
	@brief An archive that tracks changes to watched variables

	The DeltaArchive extends the AutoArchive by tracking and
	packing only variables which have been updated. Adding variables
	to the DeltaArchive follows the same semantics as adding
	variables to an AutoArchive. The DeltaArchive additionally
	checks the variable for it's type. If the variable is a
	DeltaVariableBase, it will assign it's owner member to the
	DeltaArchive.

	Any changes to watched variables since the last call to
	packDeltas() will be included in the next call to
	packDeltas(). The resulting stream can be fed to another
	DeltaArchive object through it's unpackDeltas() member to
	synchronize it with the original instance.

	The AutoArchive and DeltaArchive objects place a magic byte in the
	stream when the pack or packDeltas. The unpack() method is 
	overridden in DeltaArchive, and will check to see if the 
	supplied Stream contains an AutoArchive (all of the watched
	variables) or a DeltaArchive (only changed variables) and dispatch
	the stream appropriately.

	\verbatim
	class MyClass : public Archive::DeltaArchive
	{
	public:
		MyClass();
		~MyClass(){};
		int getValue() const;
		void setValue(int newValue);
	private:
		Archive::DeltaVariable<int> value;
		Archive::DeltaArchive       package;
	};

	MyClass::MyClass() :
	Archive::DeltaArchive(),
	value(0)
	{
	}

	int MyClass::getValue() const
	{
		return value;
	}

	void MyClass::setValue(int newValue)
	{
		value = newValue;
	}

	void foo()
	{
		MyClass a;
		MyClass b;
		
		a.setValue(10);
		Archive::Stream s;
		a.pack(s);
		Archive::Stream::ReadIterator r = s.begin();
		b.unpack(r);

		// b is now equal to a
		assert(a == b);

		b.setValue(11);
		s.clear();
		b.packDeltas(s);
		r = s.begin();
		a.unpack(r);

		// a has now received the changed (delta) for the value. 
		// any other values in the object are not included in the
		// stream because they have not changed.
		assert(a == b);
	}
	\endverbatim
	@see AutoArchive
	@see Stream
	@see Archive
	@see AutoVariableBase
	@see DeltaVariableBase

	@author Justin Randall
*/
class DeltaArchive : public AutoArchive
{
public:
	DeltaArchive();
	virtual ~DeltaArchive();
	virtual void addVariable(AutoVariableBase & newVariable);
	const bool   isDirty() const;
	virtual void packDeltas(Stream & target);
	virtual void unpack(Stream::ReadIterator & source);
	virtual void unpackDeltas(Stream::ReadIterator & source);
protected:
	friend class DeltaVariableBase;
	void addToDirtyList(DeltaVariableBase * source);

private:
	DeltaArchive(const DeltaArchive & source);
	DeltaArchive & operator = (const DeltaArchive &  rhs);

private:
	std::set<DeltaVariableBase *> dirtyVariables;
	/** \example TestDeltaArchive.cpp 
		\include TestDeltaArchive.h
	*/
};

//---------------------------------------------------------------------
/**
	@brief an internal method to track changes to watched variables

	When a DeltaVariableBase (or derivative) is altered, it will add
	itself to it's owner's dirty list via it's DeltaVariableBase::dirty()
	method. Objects on this list will be included during a DeltaArchive
	object's call to packDeltas().

	@author Justin Randall
*/
inline void DeltaArchive::addToDirtyList(DeltaVariableBase * source)
{
	dirtyVariables.insert(source); //lint !e534 who cares about the return iterator?
}

//---------------------------------------------------------------------
/**
	@brief checks to see if the DeltaArchive has data to be sent

	If there are any variables in the dirtyVariables list, then this
	method will return true.

	@author Justin Randall
*/
inline const bool DeltaArchive::isDirty() const
{
	return (dirtyVariables.size() > 0);
}

//---------------------------------------------------------------------

}//namespace Archive

//---------------------------------------------------------------------

#endif	// _DeltaArchive_H
