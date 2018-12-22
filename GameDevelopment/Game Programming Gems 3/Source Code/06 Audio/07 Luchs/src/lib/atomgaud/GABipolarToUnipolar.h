// GABipolarToUnipolar - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	Most oscillator signals are bipolar with a range of(-1.0 to +1.0).
//				Other functions, like line and envelope are unipolar (0.0 to +1.0).
//				This processor converts from bipolar to unipolar
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#ifndef _atomosGABipolarToUnipolar_
#define _atomosGABipolarToUnipolar_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGABipolarToUnipolar : public IGAProcessor
{



}; // interface


class ATOMGAUD_DLLCLASS CGABipolarToUnipolar : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GABipolarToUnipolar };
	static const CClass classCGABipolarToUnipolar;
	virtual const IClass*		GetClass();

	virtual void	Terminate();	
	virtual UINT32	Process(IObject* pob=NULL);
	

	// IGABipolarToUnipolar



	// CTOR / DTOR
	CGABipolarToUnipolar();
	~CGABipolarToUnipolar();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGABipolarToUnipolar_
// --------------------------------------------------------------------------------------------------------
