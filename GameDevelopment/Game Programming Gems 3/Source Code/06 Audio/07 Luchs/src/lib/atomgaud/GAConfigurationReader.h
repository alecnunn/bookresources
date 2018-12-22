// GAConfigurationReader - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGAConfigurationReader_
#define _atomosGAConfigurationReader_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAConfigurationReader : public IGAObjectReader
{



}; // interface


class ATOMGAUD_DLLCLASS CGAConfigurationReader : public CGAObjectReader  
{
public:

	// IObject
	enum { CID = CID_GAConfigurationReader };
	static const CClass classCGAConfigurationReader;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	// IGAObjectReader
	virtual UINT32 Process();

	// IGAConfigurationReader


	// CTOR / DTOR
	CGAConfigurationReader();
	~CGAConfigurationReader();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAConfigurationReader_
// --------------------------------------------------------------------------------------------------------
