#ifndef _COMPONENTLIBRARY_EXAMPLECONFIGURATION_HPP
#define _COMPONENTLIBRARY_EXAMPLECONFIGURATION_HPP


#include "ReferenceCount.hpp"
#include "InterfaceID.hpp"
#include "RTTITypeID.hpp"
#include "DLLFactory.hpp"
#include "Component.hpp"
#include "Factory.hpp"


typedef ComponentLibrary::RTTITypeID TypeID;									//	Use the C++ TypeID system
typedef ComponentLibrary::InterfaceID_WithRelaxedVersion<TypeID> InterfaceID;	//	Relaxed interface versioning


class Component :	public ComponentLibrary::Component<TypeID, InterfaceID>,
					public ComponentLibrary::ReferenceCount
	//	Our custom Component class
{
	protected:

		Component()
		{}


	public:

		virtual Component* Clone()
		//	Optional clone method
		{
			return 0;
		}
};


typedef ComponentLibrary::Factory<TypeID, Component> Factory;
typedef ComponentLibrary::DLLFactory<TypeID, Component> DLLFactory;


#endif