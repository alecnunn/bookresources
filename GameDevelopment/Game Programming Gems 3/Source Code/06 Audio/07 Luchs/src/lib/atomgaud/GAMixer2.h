// GAMixer2 - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	GAMixer2 mixes 2 input signals controlled by the input at 'Mixer2'. The default  
//				Mixer2 is 0.5.
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

#ifndef _atomosGAMixer2_
#define _atomosGAMixer2_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAMixer2 : public IGAMixer
{

}; // interface


class ATOMGAUD_DLLCLASS CGAMixer2 : public CGAMixer  
{
public:

	// IObject
	enum { CID = CID_GAMixer2 };
	static const CClass classCGAMixer2;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32			Process(IObject* pContext=NULL);

	// CTOR / DTOR
	CGAMixer2();
	~CGAMixer2();

protected:


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAMixer2_
// --------------------------------------------------------------------------------------------------------
