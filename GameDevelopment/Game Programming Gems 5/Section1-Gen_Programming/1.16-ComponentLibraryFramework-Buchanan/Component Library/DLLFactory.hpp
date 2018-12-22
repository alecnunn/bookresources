#ifndef _COMPONENTLIBRARY_DLLFACTORY_HPP
#define _COMPONENTLIBRARY_DLLFACTORY_HPP


#ifndef _WIN32
#error "This file only works on the Microsoft Windows Platforms as it requires DLL functionality"
#endif


#include <windows.h>


#include <string>


#include "Factory.hpp"


namespace ComponentLibrary
{
	template<class TypeID, class Base>
	class DLLFactory : public Factory<TypeID, Base>
	{
		public:

			DLLFactory(const DLLFactory& factory);
			DLLFactory(const char* const name);
			DLLFactory();
			virtual ~DLLFactory();


		public:

			void Load(const char* const name);
			void Unload();


		public:

			bool IsLoaded();


		public:

			DLLFactory& operator=(const DLLFactory& factory);


		private:

			typedef Factory<TypeID, Base> Factory;
			typedef void (*DestroyFactoryFunction)(Factory* factory);
			typedef Factory* (*CreateFactoryFunction)();


		private:

			DestroyFactoryFunction destroy;
			CreateFactoryFunction create;

			std::string name;
			Factory* factory;
			HMODULE handle;
	};


	template<class TypeID, class Base>
	inline DLLFactory<TypeID, Base>::DLLFactory(const DLLFactory& factory)
		:	destroy(0),
			create(0),
			handle(0)
	{
		if(factory.name.size() && factory.handle)
			Load(factory.name.c_str());
	}


	template<class TypeID, class Base>
	inline DLLFactory<TypeID, Base>::DLLFactory(const char* const name)
		:	destroy(0),
			create(0),
			handle(0)
	{
		Load(name);
	}


	template<class TypeID, class Base>
	inline DLLFactory<TypeID, Base>::DLLFactory()
		:	destroy(0),
			create(0),
			handle(0)
	{
	}


	template<class TypeID, class Base>
	inline DLLFactory<TypeID, Base>::~DLLFactory()
	{
		Unload();
	}


	template<class TypeID, class Base>
	inline void DLLFactory<TypeID, Base>::Load(const char* const name)
	{
		handle = LoadLibrary(name);


		if(handle)
		{
			create = (CreateFactoryFunction)GetProcAddress(handle, "CreateFactory");
			destroy = (DestroyFactoryFunction)GetProcAddress(handle, "DestroyFactory");

			this->name = std::string(name);

			if(create)
			{
				factory = (*create)();

				if(factory)
					AddChild(factory);
				else
					Unload();
			}
			else
				Unload();
		}
	}


	template<class TypeID, class Base>
	inline void DLLFactory<TypeID, Base>::Unload()
	{
		if(handle)
		{
			if(destroy)
				(*destroy)(factory);

			if(factory)
				RemoveChild(factory);

			FreeLibrary(handle);

			name.clear();
			factory = 0;
			destroy = 0;
			create = 0;
			handle = 0;
		}
	}


	template<class TypeID, class Base>
	inline bool DLLFactory<TypeID, Base>::IsLoaded()
	{
		return	factory &&
				handle &&
				create;
	}


	template<class TypeID, class Base>
	inline DLLFactory<TypeID, Base>& DLLFactory<TypeID, Base>::operator=(const DLLFactory& factory)
	{
		Unload();

		if(factory.name.size() && factory.handle)
			Load(factory.name.c_str());


		return *this;
	}
}


#endif