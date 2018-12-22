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

#ifndef	_AutoVariableWatcher_H
#define	_AutoVariableWatcher_H

#pragma warning ( disable : 4701 ) // unitialized value (archive inits value by ref)
//---------------------------------------------------------------------

#include "AutoVariable.h"

namespace Archive {
//-----------------------------------------------------------------------
/** \class AccessorTemplate AutoArchive.h "Archive/AutoArchive.h"
	@brief A templated struct that provides set and get adaptors

	AccessorTemplate describes a struct containing two methods,
	set and get, which invoke SourceObjectType::get(ValueType & target)
	and SourceObjectType::set(ValueType & value) in their respect
	get and set routines.

	If the SourceObjectType provides set(const ValueType &) and
	a get(ValueType &) methods, then this template may be passed as
	a parameter to a AutoVariableWatcher. E.g. 
	AutoVariableWatcher<int, MySourceObjectType, AccessorTemplate<int, MySourceObjectType> >

	While the declaration of this AutoVariableWatcher is long, it is
	less complex than writing accessor struct types for each hidden 
	member for a base class.

	If the SourceObjectType class does not provide set() and get() as
	defined above, then the implementor will have to resort to
	writing an accessor to use with the AutoVariableWatcher.

	The use of AutoVariableWatchers is discouraged, but are provided
	as a means to access otherwise hidden data.

	@note It is important to ensure that get() is const. Though it accepts a 
	non-const reference as a target parameter, the get should ensure that 
	the source variable is not used non-constly (e.g. explicitly make a copy
	and assign via the copy!)

	Example Usage:
	\verbatim
	class MyAutoArchive : public AutoArchive
	{
	public:
		MyAutoArchive();
		~MyAutoArchive() {};

		void get(double & target) const;
		void set(const double & source);

	private:
		SomeOtherObjectClass c;

		// AutoVariableWatchers tracking other members
		AutoVariableWatcher<double, MyAutoArchive, AccessorTemplate<double, MyAutoArchive> > smartD;
	};

	MyAutoArchive::MyAutoArchive()
	{
		addVariable(smartD);
		smartD.setSourceObject(*this);
	}

	void MyAutoarchive::get(double & target) const
	{
		target = c.getSomeDoubleValue();
	}

	void MyAutoArchive::set(const double & source)
	{
		c.setSomeDoubleValue(source);
	}

	bar()
	{
		MyAutoArchive a;
		
		network_send(a.pack());
	}

	network_receive(const Stream & source)
	{
		MyAutoArchive b;
		b << source;
	}

	\endverbatim

	@see AutoVariableBase
	@see AutoVariableWatcher
	@see AutoArchive
	@see Stream

	@author Justin Randall
*/
template<class ValueType, class SourceObjectType>
struct AccessorTemplate
{
	const ValueType get(const SourceObjectType & source) const;
	void set(SourceObjectType & target, const ValueType & value) const;
};

//-----------------------------------------------------------------------
/**
	@brief Implementation of Accessor get method

	Invokes get() on a SourceObjectType instance (defined when the client
	invokes AutoVariableWatcher<>::setSourceObject() ) and returns a 
	copy of the result.

	@param source   A const reference to an instance of a SourceObjectType.
	                It is important to remember that SourceObjectType MUST
					define get(ValueType &)!!

	@return a copy of the result

	@see AutoArchive
	@see AutoVariableBase
	@see AutoVariableWatcher
	@see Stream

	@author Justin Randall
*/
template<class ValueType, class SourceObjectType>
inline const ValueType AccessorTemplate<ValueType, SourceObjectType>::get(const SourceObjectType & source) const
{
	ValueType result;
	source.get(result);
	return result;
}

//-----------------------------------------------------------------------
/**
	@brief Implementation of Accessor set method

	Invokes set() on a SourceObjectType instance (defined when the client
	invokes AutoVariableBase<ValueType>::setSourceObject() ).

	@param target   An instance of the source object,
	                                   which is provided by 
									   AutoVariableWatcher when it invokes
									   this method on the Accessor
	@param value     The source value passed to the 
	                                   target

	@see AutoArchive
	@see AutoVariableBase
	@see AutoVariableWatcher
	@see Stream

	@author Justin Randall
*/
template<class ValueType, class SourceObjectType>
inline void AccessorTemplate<ValueType, SourceObjectType>::set(SourceObjectType & target, const ValueType & value) const
{
	target.set(value);
}

//---------------------------------------------------------------------
/** \class AutoVariableWatcher AutoArchive.h "Archive/AutoArchive.h"
	@brief An adaptor for accessing data hidden in some class

	The AutoVariableWatcher is intended to provide some
	AutoArchive access to data that is hidden in some other
	class. This template uses the Accessor parameter to set
	the adaptor which will be used to access hidden data.

	Consider the following:
	\verbatim
	class BaseObject
	{
	public:
		// ... ctor / dtor and other code ...
	
		const double getHeading() const { return heading; };
		void         setHeading(const double newHeading) { heading = newHeading; };
	private:
		double heading;
	};
	\endverbatim

	Here, a BaseObject does not directly expose the heading member as a double,
	but provides setters and getters for the heading. Clients of BaseObject 
	do not need to know anything about the internal representation of 
	the heading, which is the intent of the author for BaseObject.

	Now, someone extends a system that uses a base object and wants to
	archive it's heading (perhaps for sending it over the network).

	How would an archive access the heading for serialization and
	de-serialization? One way is to use a basic Stream class and explicitly
	get/set the heading in pack() and unpack() routines. What if the
	client wants to use a smart archive to gaurantee packing and unpacking
	order?

	The AutoVariableWatcher encapsulates that functionality (explicit
	pack and unpack, but with gauranteed ordering provided by a AutoArchive).

	\verbatim
	class SomeAutoArchive : public AutoArchive
	{
	public:
		// ... ctor / dtor and other code ...
		const double getHeading() const { return someObject.getHeading();};
		void setHeading(const double heading) { someObject.setHeading(heading); };
	private:
		BaseObject    someObject;
		AutoWatcherVariable<double, SomeAutoArchive, HeadingAccessor> smartHeading;
	};

	struct HeadingAccessor
	{
		const double get(const SomeAutoArchive & source) const;
		void set(SomeAutoArchive & target, const double & value) const;		
	};

	const double HeadingAccessor::get(const SomeAutoArchive & source) const
	{
		double result = source.getHeading();
	}

	void HeadingAccessor::set(SomeAutoArchive & target, const double & value) const
	{
		target.setHeading(value);
	}
	\endverbatim

	The AutoVariableWatcher will invoke set and get using the Heading
	accessor when the heading value needs to be retrieved or persisted,
	without ever having direct access to the heading value in
	BaseObject!

	@see AutoVariableBase
	@see AutoArchive
	@see Stream
	@see AccessorTemplate

	@author Justin Randall

*/
template<class ValueType, class SourceObjectType, class Accessor>
class AutoVariableWatcher : public AutoVariableBase
{
public:
	AutoVariableWatcher();
	~AutoVariableWatcher();

	virtual void    pack(Stream & target) const;
	void            setSourceObject(SourceObjectType & sourceObject);
	virtual void    unpack(Stream::ReadIterator & source);
private:
	Accessor                 accessor;
	SourceObjectType *       sourceObject;
};

//---------------------------------------------------------------------

template<class ValueType, class SourceObjectType, class Accessor>
inline AutoVariableWatcher<ValueType, SourceObjectType, Accessor>::AutoVariableWatcher() :
AutoVariableBase(TypeMap::getType((ValueType *)0)),
sourceObject(0)
{
}

//---------------------------------------------------------------------

template<class ValueType, class SourceObjectType, class Accessor>
inline AutoVariableWatcher<ValueType, SourceObjectType, Accessor>::~AutoVariableWatcher()
{
}

//---------------------------------------------------------------------
/**
	@brief Retrieve the watched value for a AutoArchive.

	A AutoArchive will invoke this method when it is packing the
	archive. The AutoVariableWatcher will employ the Accessor (supplied
	as template arguments to the AutoVariableWatcher) to retrieve
	the source object's watched value. The source object is set
	by invoking setSourceObject(SourceObjectType & newSource) on the
	AutoVariableWatcher before it is used (usually during construction
	of whatever AutoArchive derivative contains the AutoVariableWatcher.

	The pack method is not designed for use by a AutoArchive client,
	but by the AutoArchive itself.

	@param target  The archive (usually a AutoArchive) that
	                         will write the watched value to the archive's
							 buffer at the current write position.

	@see AutoVariableWatcher
	@see AutoVariableWatcher::setSourceObject
	@see AutoVariableBase
	@see AutoArchive
	@see Stream

	@author Justin Randall	
*/
template<class ValueType, class SourceObjectType, class Accessor>
inline void AutoVariableWatcher<ValueType, SourceObjectType, Accessor>::pack(Stream & target) const
{
	assert(sourceObject != 0);
	AutoVariableBase::pack(target);
	Archive::put(target, accessor.get(*sourceObject));
}

//---------------------------------------------------------------------
/**
	@brief Tell the AutoVariableWatcher which object owns the variable
	to be watched.

	If a watched variable is defined in some other class an is not
	directly accessible (hence, the use of the AutoVariableWatcher),
	the Accessor can get/set the watched member only if it can identify
	an instance of the target object owning the watched member.

	Before a AutoVariableWatcher can pack or unpack the watched
	variable, this method *MUST* be invoked. It us typically invoked
	during the construction of a AutoArchive derived class.

	Because the source object is settable at run time, the instance
	of a watched variable can change. 

	It is an error to pack or unpack a AutoArchive unless the source
	object is set.

	@param newSourceObject  A reference to an 
	                                           instance of the object
											   containing the member
											   to be watched.

	@see AutoVariableBase
	@see AutoArchive
	@see AutoVariableWatcher
	@see Stream

	@author Justin Randall
*/
template<class ValueType, class SourceObjectType, class Accessor>
inline void AutoVariableWatcher<ValueType, SourceObjectType, Accessor>::setSourceObject(SourceObjectType & newSourceObject)
{
	sourceObject = &newSourceObject;
}

//---------------------------------------------------------------------
/**
	@brief retrieve a value from an archive and apply it to the watched
		variable

	The unpack method is invoked by the AutoArchive during it's
	unpack method. The AutoVariableWatched retrieves the value from
	the source archive buffer, then invokes the Accessor set() method
	to apply the value to the watched variable

	@param source   The source archive containing the value at the 
	                archive's current read position.

	@see AutoVariableWatcher
	@see AutoVariableBase
	@see AutoArchive
	@see Stream

	@author Justin Randall
*/
template<class ValueType, class SourceObjectType, class Accessor>
inline void AutoVariableWatcher<ValueType, SourceObjectType, Accessor>::unpack(Stream::ReadIterator & source)
{
	assert(sourceObject != 0);

	AutoVariableBase::unpack(source);
	ValueType value;
	Archive::get(source, value);
	accessor.set(*sourceObject, value);
}

//---------------------------------------------------------------------
}//namespace
#endif	// _AutoVariableWatcher_H
