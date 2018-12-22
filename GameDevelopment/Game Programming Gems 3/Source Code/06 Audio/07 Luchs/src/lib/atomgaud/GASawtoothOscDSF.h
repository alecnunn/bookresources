// GASawtoothOscDSF - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The CGASawtoothOscDSF is a bandlimited pulse generator implemented by summation of
//				partials. The number of harmonics can be specified for filtering effects.
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1998-08-02	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------



#ifndef _atomosGASawtoothOscDSF_
#define _atomosGASawtoothOscDSF_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGASawtoothOscDSF : public IGAOscillator
{



}; // interface


class ATOMGAUD_DLLCLASS CGASawtoothOscDSF : public CGAOscillator
{
public:

	// IObject
	enum { CID = CID_GASawtoothOscDSF };
	static const CClass classCGASawtoothOscDSF;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGASawtoothOscDSF


	// CTOR / DTOR
	CGASawtoothOscDSF();
	~CGASawtoothOscDSF();

protected:

	UINT32				m_nHarmonics;
	FLOAT32				m_fCutoff;	 // 0 to 1 , with 1 brighter



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGASawtoothOscDSF_
// --------------------------------------------------------------------------------------------------------
