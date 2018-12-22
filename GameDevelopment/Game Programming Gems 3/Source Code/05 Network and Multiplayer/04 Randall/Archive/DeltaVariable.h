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

#ifndef	_DeltaVariable_H
#define	_DeltaVariable_H

//---------------------------------------------------------------------

#include "AutoVariable.h"
#include "DeltaArchive.h"
#include <map>

namespace Archive {

//---------------------------------------------------------------------

class DeltaVariableBase : public AutoVariableBase
{
public:
	explicit            DeltaVariableBase   (const std::string & typeName);
	virtual             ~DeltaVariableBase  () = 0;

	void                dirty               ();
	virtual void        packDelta           (Stream & target) = 0;
	virtual void        unpackDelta         (Stream::ReadIterator & source) = 0;

protected:
	friend class DeltaArchive;
	const unsigned int  getIndex            (DeltaArchive * owner) const;
	void                setOwner            (DeltaArchive * target, const unsigned int index);
	void                removeOwner         (DeltaArchive * target);

private:
	std::map<DeltaArchive *, unsigned int> owners;
};

//---------------------------------------------------------------------

inline DeltaVariableBase::DeltaVariableBase(const std::string & t) :
AutoVariableBase(t),
owners()
{
}

//---------------------------------------------------------------------

inline DeltaVariableBase::~DeltaVariableBase()
{
}

//---------------------------------------------------------------------

inline void DeltaVariableBase::dirty()
{
	std::map<DeltaArchive *, unsigned int>::iterator i;
	for(i = owners.begin(); i != owners.end(); ++i)
	{
		(*i).first->addToDirtyList(this);
	}
}

//---------------------------------------------------------------------

inline const unsigned int DeltaVariableBase::getIndex(DeltaArchive * target) const
{
	unsigned int result = 0xFFFFFFFF;
	std::map<DeltaArchive *, unsigned int>::const_iterator f = owners.find(target);
	if(f != owners.end())
		result = (*f).second;
	return result;
}

//---------------------------------------------------------------------

inline void DeltaVariableBase::removeOwner(DeltaArchive * target)
{
	owners.erase(owners.find(target));	
}

//---------------------------------------------------------------------

inline void DeltaVariableBase::setOwner(DeltaArchive * target, const unsigned int index)
{
	owners[target] = index;
}

//---------------------------------------------------------------------

template<class ValueType>
class DeltaVariable : public DeltaVariableBase
{
public:
	DeltaVariable();
	explicit DeltaVariable(const ValueType & source);
	~DeltaVariable();
	
	                  operator const ValueType & () const; //lint !e1930 this is the intended behavior of a DeltaVariable
	const ValueType & operator = (const ValueType & rhs);

	const ValueType & get() const;
	void pack(Stream & target) const;
	void packDelta(Stream & target);
	void set(const ValueType & source);
	void unpack(Stream::ReadIterator & source);
	void unpackDelta(Stream::ReadIterator & source);
private:
	ValueType currentValue;
};

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVariable<ValueType>::DeltaVariable() :
DeltaVariableBase(TypeMap::getType(ValueType)),
currentValue(),
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVariable<ValueType>::DeltaVariable(const ValueType & source) :
DeltaVariableBase(TypeMap::getType(&source)),
currentValue(source)
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVariable<ValueType>::~DeltaVariable()
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline DeltaVariable<ValueType>::operator const ValueType & () const
{
	return get();
}

//---------------------------------------------------------------------

template<class ValueType>
inline const ValueType & DeltaVariable<ValueType>::operator = (const ValueType & rhs)
{
	if(rhs != currentValue)
		set(rhs);
	return currentValue;
}

//---------------------------------------------------------------------

template<class ValueType>
inline const ValueType & DeltaVariable<ValueType>::get() const
{
	return currentValue;
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVariable<ValueType>::pack(Stream & target) const
{
	AutoVariableBase::pack(target);
	Archive::put(target, currentValue);
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVariable<ValueType>::packDelta(Stream & target) 
{
	AutoVariableBase::pack(target);
	Archive::put(target, currentValue);
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVariable<ValueType>::set(const ValueType & source)
{
	if(currentValue != source)
	{
		currentValue = source;
		dirty();
	}
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVariable<ValueType>::unpack(Stream::ReadIterator & source)
{
	AutoVariableBase::unpack(source);
	Archive::get(source, currentValue);
}

//---------------------------------------------------------------------

template<class ValueType>
inline void DeltaVariable<ValueType>::unpackDelta(Stream::ReadIterator & source)
{
	AutoVariableBase::unpack(source);
	Archive::get(source, currentValue);
}

//---------------------------------------------------------------------

}//namespace Archive

#endif	// _DeltaVariable_H
