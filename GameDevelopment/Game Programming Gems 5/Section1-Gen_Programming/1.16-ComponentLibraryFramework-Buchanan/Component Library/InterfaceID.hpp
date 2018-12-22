#ifndef _COMPONENTLIBRARY_INTERFACEID_HPP
#define _COMPONENTLIBRARY_INTERFACEID_HPP


#include "Version.hpp"


namespace ComponentLibrary
{
	template<class TypeID>
	struct InterfaceID_WithRelaxedVersion
	{
		TypeID typeID;
		int version;


		template<class Type>
		InterfaceID_WithRelaxedVersion(const Type* type)
			:	version(VersionNumber(type)),
				typeID(type)
		{}


		int VersionNumber(...)
		{
			return 0;
		}


		template<int Number>
		int VersionNumber(const Version<Number>*)
		{
			return Number;
		}


		int operator==(const InterfaceID_WithRelaxedVersion& interfaceID)
		{
			return	(version == -interfaceID.version || version == interfaceID.version) &&
					typeID == interfaceID.typeID;
		}


		bool operator<(const InterfaceID_WithRelaxedVersion& interfaceID) const
		{
			if(typeID == interfaceID.typeID)
				return version < interfaceID.version;


			return typeID < interfaceID.typeID;
		}
	};


	template<class TypeID>
	struct InterfaceID_WithStrictVersion
	{
		TypeID typeID;
		int version;


		template<class Type>
		InterfaceID_WithStrictVersion(const Type* type)
			:	version(VersionNumber(type)),
				typeID(type)
		{}


		template<int Number>
		int VersionNumber(const Version<Number>*)
		{
			return Number;
		}


		int operator==(const InterfaceID_WithStrictVersion& interfaceID)
		{
			return	(version == -interfaceID.version || version == interfaceID.version) &&
					typeID == interfaceID.typeID;
		}


		bool operator<(const InterfaceID_WithStrictVersion& interfaceID) const
		{
			if(typeID == interfaceID.typeID)
				return version < interfaceID.version;


			return typeID < interfaceID.typeID;
		}
	};


	template<class TypeID>
	struct InterfaceID_WithNoVersion
	{
		TypeID typeID;


		template<class Type>
		InterfaceID_WithNoVersion(const Type* type)
			:	typeID(type)
		{}


		int operator==(const InterfaceID_WithNoVersion& interfaceID)
		{
			return	typeID == interfaceID.typeID;
		}


		bool operator<(const InterfaceID_WithNoVersion& interfaceID) const
		{
			return typeID < interfaceID.typeID;
		}
	};
}


#endif