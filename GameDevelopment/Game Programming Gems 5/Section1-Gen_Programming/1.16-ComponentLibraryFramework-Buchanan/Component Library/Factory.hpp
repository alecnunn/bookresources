#ifndef _COMPONENTLIBRARY_FACTORY_HPP
#define _COMPONENTLIBRARY_FACTORY_HPP


#include <assert.h>


#include <list>
#include <map>


namespace ComponentLibrary
{
	template<class TypeID, class Base>
	class Factory
	{
		public:

			virtual ~Factory();


		public:	//	Singleton access

			static Factory& Singleton();


		public:	//	Child factory management

			void RemoveChild(Factory* factory);
			void AddChild(Factory* factory);


		public:	//	Creation facilities

			Base* Create(const char* const name);
			template<class Type> Base* Create();
			Base* Create(const TypeID& typeID);


		public:	//	Creation support management

			void RemoveSupport(const TypeID& typeID);
			template<class Type> void RemoveSupport();
			template<class Type> void Support();

			TypeID GetSupportedTypeID(unsigned index);
			unsigned NumberOfSupportedTypes();

			template<class Type> bool Supports();
			bool Supports(const TypeID& typeID);


		private:

			struct BaseConstructor
			{
				virtual Base* Construct() = 0;
			};


			template<class Type>
			struct Constructor : BaseConstructor
			{
				Base* Construct()
				{
					return new Type();
				}
			};


			typedef std::map<TypeID, BaseConstructor*> ConstructorMap;


		private:

			ConstructorMap constructorMap;
			std::list<Factory*> children;


		private:

			BaseConstructor* FindConstructor(const TypeID& typeID);
	};


	template<class TypeID, class Base>
	inline Factory<TypeID, Base>::~Factory()
	{
		for(ConstructorMap::iterator constructor = constructorMap.begin();
			constructor != constructorMap.begin();
			constructor++)
		{
			delete constructor->second;
		}
	}


	template<class TypeID, class Base>
	inline Factory<TypeID, Base>& Factory<TypeID, Base>::Singleton()
	{
		static Factory factory;


		return factory;
	}


	template<class TypeID, class Base>
	inline void Factory<TypeID, Base>::RemoveChild(Factory* factory)
	{
		children.remove(factory);
	}


	template<class TypeID, class Base>
	inline void Factory<TypeID, Base>::AddChild(Factory* factory)
	{
		children.push_back(factory);
	}


	template<class TypeID, class Base>
	inline Base* Factory<TypeID, Base>::Create(const char* const name)
	{
		return Create(TypeID::FromName(name));
	}


	template<class TypeID, class Base>
	inline Base* Factory<TypeID, Base>::Create(const TypeID& typeID)
	{
		BaseConstructor* constructor = FindConstructor(typeID);


		if(constructor)
			return constructor->Construct();


		return 0;
	}


	template<class TypeID, class Base>
	template<class Type>
	inline Base* Factory<TypeID, Base>::Create()
	{
		Type* type = 0;


		return Create(TypeID(type));
	}


	template<class TypeID, class Base>
	inline void Factory<TypeID, Base>::RemoveSupport(const TypeID& typeID)
	{
		ConstructorMap::iterator constructor = constructorMap.find(typeID);


		if(constructor != constructorMap.end())
		{
			delete (*constructor).second;
			constructorMap.erase(constructor);
		}
	}


	template<class TypeID, class Base>
	template<class Type>
	inline void Factory<TypeID, Base>::RemoveSupport()
	{
		Type* type = 0;


		RemoveSupport(TypeID(type));
	}


	template<class TypeID, class Base>
	template<class Type>
	inline void Factory<TypeID, Base>::Support()
	{
		Type* type = 0;


		//	Create a new constructor object of the required type and add it to our map
		constructorMap[TypeID(type)] = new Constructor<Type>();
	}


	template<class TypeID, class Base>
	inline TypeID Factory<TypeID, Base>::GetSupportedTypeID(unsigned index)
	{
		ConstructorMap::iterator constructor = constructorMap.begin();


		assert(index < constructorMap.size());


		//	NOTE:	This is not exactly efficient but it is assumed that it does not need to be
		while(index--)
			constructor++;


		return constructor->first;
	}


	template<class TypeID, class Base>
	inline unsigned Factory<TypeID, Base>::NumberOfSupportedTypes()
	{
		return constructorMap.size();
	}


	template<class TypeID, class Base>
	template<class Type>
	inline bool Factory<TypeID, Base>::Supports()
	{
		Type* type = 0;


		return Supports(TypeID(type));
	}


	template<class TypeID, class Base>
	inline bool Factory<TypeID, Base>::Supports(const TypeID& typeID)
	{
		return FindConstructor(typeID) != 0;
	}


	template<class TypeID, class Base>
	inline typename Factory<TypeID, Base>::BaseConstructor* Factory<TypeID, Base>::FindConstructor(const TypeID& typeID)
	{
		ConstructorMap::iterator constructor = constructorMap.find(typeID);


		//	If we support the creation of this object return the constructor object
		if(constructor != constructorMap.end())
			return constructor->second;

		//	Otherwise check to see if any of our children support its creation
		for(std::list<Factory*>::iterator child = children.begin();
			child != children.end();
			child++)
		{
			BaseConstructor* constructor = (*child)->FindConstructor(typeID);


			if(constructor)
				return constructor;
		}


		return 0;
	}
}


#endif