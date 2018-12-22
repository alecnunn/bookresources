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

#ifndef	_AutoArchive_H
#define	_AutoArchive_H

//---------------------------------------------------------------------
#pragma warning ( disable : 4786)
#pragma warning ( disable : 4514) // unreferenced inline function has been removed
#include "Archive.h"
#include <assert.h>
#include <vector>

//---------------------------------------------------------------------

namespace Archive {

//---------------------------------------------------------------------

class Stream;
class AutoVariableBase;

//---------------------------------------------------------------------
/**	\class AutoArchive AutoArchive.h "Archive/AutoArchive.h"
	@brief AutoAchive is a specialization of Stream that automates packing
	and unpacking.

	To use a AutoArchive, the client declares AutoVariables for
	members it owns and wants to persist. If the client class derives
	from some parent which does not provide smart variables, there
	are two approaches:

	1) If the parent class provides a reference to some value that
	   should be persisted, the new subclass may declare a
	   AutoVariableWatcher to track the value in the parent class.
	2) Declare a AutoVariable that shadows the parent class' normal
	   integral type, override pack and unpack. During the pack 
	   operation, the subclass sets the AutoVariable shadowing the
	   parent value, then invokes AutoArchive::pack(). Durin the
	   unpack operation, invoke AutoArchive::unpack(), then invoke
	   the parent's setter to assign the parent's value to the 
	   AutoVariable's value.

	Example Usage:
	\verbatim
	class MyAutoArchive : public AutoArchive
	{
	public:
		MyAutoArchive();
		~MyAutoArchive() {};

        const float getFloatValue() const;
		const int   getIntValue() const;
		void        setValue(const float value);
		void        setValue(const int value);

	private:
		AutoVariable<int>	        i;
		AutoVariableWatcher<float> f;
	};

	MyAutoArchive::MyAutoArchive() :
	i(0)
	{
		addVariable(i);
		addVariable(f);
	}

	const int MyAutoArchive::getIntValue() const
	{
		return i;	// conversion take care of in the AutoVariable template
	}

	void MyAutoArchive::setValue(const int newValue)
	{
		i = newValue;
	}

	void foo()
	{
		MyAutoArchvie a;
		
		a.setValue(42);
		a.setValue(3.1415f);
		receiveNetworkMessage(a.pack());
	}

	void receiveNetworkMessage(const Stream & source)
	{
		MyAutoArchive b

		b << source;

		assert(b.getIntValue() == 42);
		assert(b.getFloatValue() == 3.1415f);
	}
	\endverbatim

	@see AutoVariable<ValueType>
	@see AutoVariableWatcher<ValueType>

	@author Justin Randall
*/
class AutoArchive 
{
public:
	AutoArchive();
	virtual ~AutoArchive();
	virtual void       addVariable(AutoVariableBase & newVariable);
	virtual void       pack(Stream & target) const;
	virtual void       unpack(Stream::ReadIterator & source);
protected:
	std::vector<AutoVariableBase *>	members;
};

//---------------------------------------------------------------------

inline void put(Stream & target, const AutoArchive & source)
{
	source.pack(target);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, AutoArchive & target)
{
	target.unpack(source);
}

//---------------------------------------------------------------------

}//namespace Archive

//---------------------------------------------------------------------

#endif	// _AutoArchive_H
