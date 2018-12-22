//
// BaseObject.h
//

#ifndef _BASEOBJECT_H_
#define _BASEOBJECT_H_

#include "RTTIClass.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CNullClass
//
// CNullClass is an empty class with no attached RTTI information. We use this class as an ancestor
// to our CBaseObject which is the true ultimate ancestor for all our classes. Without the null class
// we would have no templater parameter for CBaseObject's ancestor. CNullClass itself has no RTTI support.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CNullClass
{

protected :

	// It is necessary to implement this to avoid compilation errors in the templatized RTTI code
	// In any case no RTTI support is provided from this class
	static inline CRTTI* GetClassRTTI()			{ return NULL;									};	

};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CBaseObject
//
// Ultimate ancestor for all classes. CBaseObject implements no specific functionality, apart from
// the fact it provides the ultimate ancestor RTTI for all other classes. The number in the
// template parameters is a random user-generated unique identifier.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CBaseObject : public CRTTIClass<CBaseObject, CNullClass, 0x1622afe>
{

};

#endif	// _BASEOBJECT_H_