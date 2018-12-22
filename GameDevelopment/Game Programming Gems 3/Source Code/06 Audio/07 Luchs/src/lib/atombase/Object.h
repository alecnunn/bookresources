// Object.h: Schnittstelle für die Klasse CObject.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CObject_
#define _CObject_

// baseclass for all objects without any member data

// In order to have the same memory layout for interfaces and objects,
// these objects have no virtual constructor.
// see Don Box, Interface Pointers Considered Harmful
//
#include <sstream>
#include <string>
#include "atombase_interfaces.h"


namespace atomos
{
// forward
interface ISignal;
interface IClass;
class CClass;
/*
interface IObject
{
	virtual const IClass*	GetClass()=0;
	virtual BOOL			Initialize(IObject* pob=NULL)=0;
	virtual void			Terminate()=0;
	virtual void			Reset()=0;

	virtual UINT32			GetID()=0;
	virtual void			SetID(UINT32 nID)=0;

	virtual IObject*		GetOwner()=0;
	virtual void			SetOwner(IObject* pOwner)=0;

	virtual UINT32			Sync(IObject* pTimer)=0;
	virtual void			Update(UINT32 nContext=0)=0;
	virtual UINT32			Process(IObject* pContext)=0;
	virtual UINT32			Write(ISignal* pStream)=0;

	virtual BOOL			SetParameter(CHAR8* pszName, CHAR8* pszValue)=0;
};
*/
class ATOMBASE_DLLCLASS CObject  
{

public:

	// IObject
	enum { CID = CID_Object };
	static const CClass			classCObject;
	virtual const IClass*		GetClass();

	virtual BOOL			Initialize(IObject* pob=NULL);
	virtual void			Terminate();
	virtual void			Reset();

	virtual UINT32			GetID();
	virtual void			SetID(UINT32 nID);

	virtual IObject*		GetOwner();
	virtual void			SetOwner(IObject* pOwner);

	virtual UINT32			Sync(IObject* pTimer);
	virtual void			Update(UINT32 nContext=0);
	virtual UINT32			Process(IObject* pContext);
	virtual UINT32			Write(ISignal* pStream);

	virtual BOOL			SetParameter(CHAR8* pszName, CHAR8* pszValue);


	
	CObject();
	~CObject();

protected:

	// no data in this baseclass

};


} // namespace


#endif // _CObject_
