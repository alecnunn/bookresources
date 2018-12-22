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

#ifndef	_AutoVariable_H
#define	_AutoVariable_H

//---------------------------------------------------------------------

#include "AutoArchive.h"
#include "TypeMap.h"

namespace Archive {

//---------------------------------------------------------------------
/**
	@brief A AutoVariableBase is an abstract class which defines a pack
	and unpack interface for the AutoArchive.

	AutoVariables and AutoVariableWatchers derive from this
	class.

	@see AutoVariable<ValueType>
	@see AutoVariableWatcher<ValueType>
	@see AutoArchive
	@see Stream

	@author Justin Randall
*/
class AutoVariableBase
{
public:
	explicit         AutoVariableBase   (const std::string & typeName);
	virtual          ~AutoVariableBase  () = 0;
	virtual void     pack               (Stream & target) const;
	virtual void     unpack             (Stream::ReadIterator & source);
private:
	std::string      typeName;
};

//---------------------------------------------------------------------

inline AutoVariableBase::AutoVariableBase(const std::string & t) :
typeName(t)
{
}

//---------------------------------------------------------------------

inline AutoVariableBase::~AutoVariableBase()
{
}

//---------------------------------------------------------------------

inline void AutoVariableBase::pack(Stream & target) const
{
	Archive::put(target, typeName);
}

//---------------------------------------------------------------------

inline void AutoVariableBase::unpack(Stream::ReadIterator & source)
{
	Archive::get(source, typeName);
}

//---------------------------------------------------------------------

inline void get(Stream::ReadIterator & source, AutoVariableBase & target)
{
	target.unpack(source);
}

//---------------------------------------------------------------------

inline void put(Stream & target, const AutoVariableBase & source)
{
	source.pack(target);
}

//---------------------------------------------------------------------
/** \class AutoVariable AutoVariable.h "Archive/AutoVariable.h"
	A stand-alone, authoritative smart variable template

	The AutoVariable contains an instance of a ValueType. It behaves
	much like the ValueType it encapsulates, but also provides a
	serialization interface for AutoArchives. 

	The use of a AutoVariable is intended to behave similarly to
	the actual ValueType it tracks, adding features for automatic
	archival and retrieval.

	A AutoVariable template should be used in a AutoArchive 
	derived class to automatically pack and unpack data when the
	Stream is packed or unpacked. 

	Refer to AutoArchive for example useage of a AutoVariable.

	@see AutoArchive
	@see AutoVariableBase
	@see AutoVariableWatcher

	@author Justin Randall	
*/
template<class ValueType>
class AutoVariable : public AutoVariableBase
{
public:
	                  AutoVariable  ();
	explicit          AutoVariable  (const ValueType & source);
	virtual           ~AutoVariable ();

	const ValueType & get           () const;
	virtual void      pack          (Stream & target) const;
	virtual void      unpack        (Stream::ReadIterator & source);
	void              set           (const ValueType & source);

private:
	ValueType         value;
};

//---------------------------------------------------------------------

template<class ValueType>
inline AutoVariable<ValueType>::AutoVariable() :
AutoVariableBase(TypeMap::getType((ValueType *)0))
{
}

//---------------------------------------------------------------------
/**
	ValueType copy constructor.

	Motivation: Provide an implicit conversion for the AutoVariable
	that mimicks the ValueType it represents.

	Invoking this copy constructor merely assigns the source value
	to the AutoVariable value member. Unlike the AutoVariableWatcher,
	the reference is not tracked.

	@param source  The source value which will
	                                     employ ValueType's assignment
                                         operator to set
										 AutoVariable::value to the
										 source value.

	@author Justin Randall
*/
template<class ValueType>
inline AutoVariable<ValueType>::AutoVariable(const ValueType & source) :
AutoVariableBase(TypeMap::getType(&source))
{
	value = source;
}

//---------------------------------------------------------------------

template<class ValueType>
inline AutoVariable<ValueType>::~AutoVariable()
{
}

//---------------------------------------------------------------------

template<class ValueType>
inline const ValueType & AutoVariable<ValueType>::get() const
{
	return value;
}

//---------------------------------------------------------------------

template<class ValueType>
inline void AutoVariable<ValueType>::set(const ValueType & source)
{
	value = source;
}

//---------------------------------------------------------------------
/**
	Pack the current value of the AutoVariable

	The current value of the AutoVariable is packed at the write
	position of the target Stream.

	The target archive is typically a AutoArchive, as it has
	protected access to the AutoVariable::pack() method.

	@param target    an Stream to receive the value at its
	                           current write position

	@see AutoVariable
	@see AutoVariableBase
	@see AutoArchive
	@see AutoVariableWatcher

	@author Justin Randall
*/
template<class ValueType>
inline void AutoVariable<ValueType>::pack(Stream & target) const
{
	AutoVariableBase::pack(target);
	Archive::put(target, value);
}

//---------------------------------------------------------------------
/**
	Unpack a value from a source archive and set the current value.

	A new value is extracted from the source archive at its current 
	read position and applied to the AutoVariable value member.

	The source archive is typically a AutoArchive, as it has 
	protected access to the AutoVariable::unpack() method.

	@param source   The archive supplying the new 
	                                value for the 

	@see AutoArchive
	@see AutoVariableBase
	@see AutoVariableWatcher

	@author Justin Randall
*/
template<class ValueType>
inline void AutoVariable<ValueType>::unpack(Stream::ReadIterator & source)
{
	AutoVariableBase::unpack(source);
	Archive::get(source, value);
}

//---------------------------------------------------------------------
}//namespace Archive

#endif	// _AutoVariable_H
