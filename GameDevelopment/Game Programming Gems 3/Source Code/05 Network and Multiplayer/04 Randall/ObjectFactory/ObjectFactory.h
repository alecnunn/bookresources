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

#ifndef	_ObjectFactory_H
#define	_ObjectFactory_H

//---------------------------------------------------------------------

#pragma warning( disable : 4514)	// unused STL function removed
#pragma warning( disable : 4786)	// long function names truncated in STL

#include <map>
#include <string>

//---------------------------------------------------------------------
/** \class ObjectFactory ObjectFactory.h "ObjectFactory/ObjectFactory.h"
	ObjectFactory, Object Creational, (aka Abstract Factory, Kit)

	@brief Intent
		Provide an interface for creating families of related or 
		dependant objects without specifying their concrete classes.

	-- Design Patterns, pg 87

	Inherits: Singleton

	This is a templated object factory. The client supplies the base
	(abstract) type, and registers create functions for various
	concrete types.

	Example:
	\verbatim

	class Car
	{
	public:
		Car() {};
		virtual ~Car() {};
		std::string getName() { return name; };
	private:
		std::string name;
	};

	class Pinto : public Car
	{
	public:
		Pinto() : name("Pinto") {};
		~Pinto();
	};

	class CelicaGT : public Car
	{
	public:
		CelicaGT() : name("CelicaGT") {};
		~CelicaGT() {};
	};

	typedef ObjectFactory<Car> CarFactory;

	Car * createPinto () { return new Pinto; };
	Car * createCelicaGT () { return new CelicaGT; };

	void foo()
	{
		CarFactory::registerCreateFunction("Pinto", createPinto);
		CarFactory::registerCreateFunction("CelicaGT", createCelicaGT);

		Car * pinto = CarFactory::create("Pinto");
		Car * celica = CarFactory::create("CelicaGT");

		cout << pinto->getName();
		cout << celica->getName();
	}

	\endverbatim

	@see Singleton

	@author Justin Randall
*/
template<typename BaseValueType>
class ObjectFactory 
{
public:
	~ObjectFactory();
	static BaseValueType *  create(const std::string & concreteTypeName);
	static void             enumerateTypes(std::vector<std::string> & targetVectorOfStrings);
	static void             registerCreateFunction(const std::string & derivedTypeName, BaseValueType * (*createFunction)());

private:
	ObjectFactory();
	static ObjectFactory & instance();

private:
	std::map<std::string, BaseValueType *(*)()>	createFunctions;
};

//---------------------------------------------------------------------
/**
	@brief Construct an object factory.

	It should **ONLY** be instantiated
	via the install() method defined in the Singleton<> template base
	class.

	E.g.
	class MyFactory<MyBaseClass>::install()

	@see Singleton<ValueType>::install()
	
	@author Justin Randall
*/
template<typename BaseValueType>
inline ObjectFactory<BaseValueType>::ObjectFactory()
{
}

//---------------------------------------------------------------------
/**
	@brief Destroy the ObjectFactory. 
	
	This should **ONLY** be invoked
	via the remove() method defined in the Singeton<ValueType> template
	base class.

	E,g,
	class MyFactory<MyBaseClass>::remove()

	@see Singleton<ValueType>::remove()

	@author Justin Randall
*/
template<typename BaseValueType>
inline ObjectFactory<BaseValueType>::~ObjectFactory()
{
}

//---------------------------------------------------------------------
/**
	@brief Create an object given a string describing it's type. 

	The create function must have first been registered with the
	ObjectFactory<BaseType>::registerCreateFunction() method.

	In debug mode, this method will assert if the create function
	has not been registered with the ObjectFactory.

	@param std::string & concreteTypeName     The type name describing the
	                                          object to create.
	@return A pointer to the object factory's base type. The concrete type
	will have actually been instantiated.

	@author Justin Randall
*/
template<typename BaseValueType>
inline BaseValueType * ObjectFactory<BaseValueType>::create(const std::string & concreteTypeName)
{
	std::map<std::string, BaseValueType *(*)()> & c = instance().createFunctions;
	std::map<std::string, BaseValueType *(*)()>::iterator f = c.find(concreteTypeName);
	BaseValueType * result = 0;
	if(f != c.end())
	{
		result = ((*f).second)();
	}
	return result;
}

//---------------------------------------------------------------------
/**
	@brief Enumerate all of the registered createFunction names. 

	The supplied vector of STL strings will be populated with all of the
	registered create types. 

	Motivation: At run time, it may be useful to query the available
		create functions which have been registered with the factory.
		Since create functions may be registered at run time, any number
		of create functions may be available via the factory. New client
		code may extend it's functionality, and create routines may want
		to query the availability of new object types.

		For example: an object editor tool may load all DLL's in a 
		directory at runtime (plug-ins), and present the user with
		a selection of objects to create and edit. The original
		client code does not need compile-time knowledge of the
		objects available for creation.

	@param std::vector<std::string> & targetVectorOFStrings  Target list 
	                                                         of strings to
															 store the enum

	@author Justin Randall
*/
template<typename BaseValueType>
inline void ObjectFactory<BaseValueType>::enumerateTypes(std::vector<std::string> & targetVectorOfStrings)
{
	std::map<std::string, BaseValueType *(*)()> & c = instance.createFunctions;	
	std::map<std::string, BaseValueType *(*)()>::iterator i;
	for(i = c.begin(); i != c.end(); ++i)
	{
		targetVectorOfStrings.push_back((*i).first);
	}
}

//---------------------------------------------------------------------

template<typename BaseValueType>
inline ObjectFactory<BaseValueType> & ObjectFactory<BaseValueType>::instance()
{
	static ObjectFactory<BaseValueType> objectFactory;
	return objectFactory;
}

//---------------------------------------------------------------------
/**
	@brief Create a concrete instance which is a derivation of the 
	ObjectFactory base type.

	The create functions are the core functionality of the object
	factory. 

	@param const std::string & derivedTypeName  A string describing the
	                                            type of object to be
												created
	@param BaseValueType *(*createFunction)()   The client-supplied 
	                                            create function should
												instantiate a new
												concrete type, and 
												return a pointer to the
												object factory base type.
*/
template<typename BaseValueType>
inline void ObjectFactory<BaseValueType>::registerCreateFunction(const std::string & derivedTypeName, BaseValueType *(*createFunction)())
{
	assert(createFunction);
	assert(derivedTypeName.size() > 0);
	instance().createFunctions[derivedTypeName] = createFunction;
}

//---------------------------------------------------------------------

#endif	// _ObjectFactory_H
