// GAController - Interface
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

#ifndef _atomosGAController_
#define _atomosGAController_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAController : public IGAProcessor
{

	virtual BOOL		SetSignal(UINT32 nIndex, FLOAT32** ppSignal)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAController : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAController };
	static const CClass classCGAController;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);
	
	// IGAController

	virtual BOOL		SetSignal(UINT32 nIndex, FLOAT32** ppSignal);


	// CTOR / DTOR
	CGAController();
	~CGAController();

protected:

	FLOAT32*	m_pfOutput;
	FLOAT**		m_ppSignal; // address of pointer to value


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAController_
// --------------------------------------------------------------------------------------------------------
