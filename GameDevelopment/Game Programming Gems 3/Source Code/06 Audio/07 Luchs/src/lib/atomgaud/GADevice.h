// GADevice - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGADevice_
#define _atomosGADevice_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGADevice : public IGAObject
{



}; // interface


class ATOMGAUD_DLLCLASS CGADevice : public CGAObject  
{
public:

	// IObject
	enum { CID = CID_GADevice };
	static const CClass classCGADevice;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();

	// IGADevice


	// CTOR / DTOR
	CGADevice();
	~CGADevice();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGADevice_
// --------------------------------------------------------------------------------------------------------
