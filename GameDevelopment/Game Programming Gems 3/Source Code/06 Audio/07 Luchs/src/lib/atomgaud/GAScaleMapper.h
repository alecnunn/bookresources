// GAScaleMapper - Interface
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

#ifndef _atomosGAScaleMapper_
#define _atomosGAScaleMapper_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAScaleMapper : public IGAProcessor
{

	virtual BOOL	SelectScale(UINT32 nScale, UINT32 nBase)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAScaleMapper : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAScaleMapper };
	static const CClass classCGAScaleMapper;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// IGAScaleMapper

	enum eScale {

		Chromatic,	
		Pentatonic,

	} SCALE;


	virtual BOOL	SelectScale(UINT32 nScale, UINT32 nBase);


	// CTOR / DTOR
	CGAScaleMapper();
	~CGAScaleMapper();

protected:

	void InitScale();
	FLOAT32 FindNote(FLOAT32 fNote);
	FLOAT32 FindChromaticNote(FLOAT32 fNote);
	FLOAT32 FindPentatonicNote(FLOAT32 fNote);

protected:



	FLOAT32	m_fChromaticScale[128];
	FLOAT32	m_fPentatonicScale[128]; // 
};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAScaleMapper_
// --------------------------------------------------------------------------------------------------------
