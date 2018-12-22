#ifndef _COMPONENTLIBRARY_RTTITYPEID_HPP
#define _COMPONENTLIBRARY_RTTITYPEID_HPP


#include <typeinfo.h>
#include <string>


namespace ComponentLibrary
{
	class RTTITypeID
	{
		public:

			template<typename Type>
			RTTITypeID(Type const* type)
				:	name(typeid(Type).name())
			{}


			RTTITypeID()
			{}


		public:

			static RTTITypeID FromName(const char* const name)
			{
				RTTITypeID typeID;


				typeID.name = name;


				return typeID;
			}


		public:

			const char* const Name()
			{
				return name.c_str();
			}


		public:

			int operator==(RTTITypeID const& typeID) const
			{
				return name == typeID.name;
			}


			bool operator<(RTTITypeID const& typeID) const
			{
				return name < typeID.name;
			}


		private:

			std::string name;
	};
}


#endif