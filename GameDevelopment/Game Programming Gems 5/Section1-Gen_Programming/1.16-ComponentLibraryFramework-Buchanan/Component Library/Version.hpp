#ifndef _COMPONENTLIBRARY_VERSION_HPP
#define _COMPONENTLIBRARY_VERSION_HPP


namespace ComponentLibrary
{
	template<unsigned Number>
	struct Version
	/*
		Derive your interface classes from this class to add version number support:

		struct MyInterface : Version<0>
		{
			...interface methods go here...
		};
	*/
	{
		#ifdef _DEBUG
				enum { VersionNumber = -(int)Number };	//	Negative version numbers in debug builds
		#else
				enum { VersionNumber = (int)Number };	//	Positive version numbers in release builds
		#endif
	};
}


#endif