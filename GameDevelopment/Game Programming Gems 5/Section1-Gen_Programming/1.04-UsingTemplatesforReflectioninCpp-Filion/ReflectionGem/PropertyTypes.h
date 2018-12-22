//
// PropertyTypes.h
//

#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

enum ePropertyType
{
	eptBOOL,
	eptDWORD,
	eptINT,
	eptFLOAT,
	eptSTRING,
	eptPTR,
	eptMAX_PROPERTY_TYPES
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPropertyType
//
// This templatized class will associate compile-time types with unique enum members.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class CPropertyType
{

public :

	// Returns type ID associated with the templatized type.
	static	ePropertyType	GetTypeID();

private:

    static			ePropertyType    ms_TypeID;

};

template<class T>
ePropertyType CPropertyType<T>::GetTypeID()
{
	return ms_TypeID;
}

template<class T> ePropertyType CPropertyType<T>::ms_TypeID = eptPTR;
template<> ePropertyType CPropertyType<bool>::ms_TypeID		= eptBOOL;
template<> ePropertyType CPropertyType<DWORD>::ms_TypeID	= eptDWORD; 
template<> ePropertyType CPropertyType<int>::ms_TypeID		= eptINT; 
template<> ePropertyType CPropertyType<float>::ms_TypeID	= eptFLOAT; 
template<> ePropertyType CPropertyType<char*>::ms_TypeID	= eptSTRING;

#endif	// _PROPERTY_TYPES_H