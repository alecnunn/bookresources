#ifndef _COMPONENTLIBRARY_COMPONENT_HPP
#define _COMPONENTLIBRARY_COMPONENT_HPP


#include <map>


namespace ComponentLibrary
{
	template<class TypeID, class InterfaceID>
	class Component
	{
		protected:

			Component()
			{}


		public:

			template<class Type> void ExposeInterface(Type* type);
			template<class Type> Type* QueryForInterface();
			template<class Type> bool ExposesInterface();

			InterfaceID GetSupportedInterfaceID(unsigned index);
			unsigned NumberOfSupportedInterfaces();


		private:

			typedef std::map<InterfaceID, void*> InterfaceMap;


		private:

			InterfaceMap interfaceMap;
	

		private:

			void* QueryForInterface(const InterfaceID& interfaceID);
	};


	template<class TypeID, class InterfaceID>
	template<class Type>
	void Component<TypeID, InterfaceID>::ExposeInterface(Type* type)
	{
		interfaceMap[InterfaceID(type)] = type;
	}


	template<class TypeID, class InterfaceID>
	template<class Type>
	Type* Component<TypeID, InterfaceID>::QueryForInterface()
	{
		Type* type = 0;


		return reinterpret_cast<Type*>(QueryForInterface(InterfaceID(type)));
	}


	template<class TypeID, class InterfaceID>
	template<class Type>
	bool Component<TypeID, InterfaceID>::ExposesInterface()
	{
		Type* type = 0;


		return QueryForInterface(InterfaceID(type)) != 0;
	}


	template<class TypeID, class InterfaceID>
	void* Component<TypeID, InterfaceID>::QueryForInterface(const InterfaceID& interfaceID)
	{
		InterfaceMap::iterator entry = interfaceMap.find(interfaceID);


		if(entry != interfaceMap.end())
			return entry->second;


		return 0;
	}
}


#endif