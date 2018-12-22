// GAButterworthLPF - Interface
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

#ifndef _atomosGAButterworthLPF_
#define _atomosGAButterworthLPF_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAButterworthLPF : public IGAButterworthFilter
{



}; // interface


class ATOMGAUD_DLLCLASS CGAButterworthLPF : public CGAButterworthFilter  
{
public:

	// IObject
	enum { CID = CID_GAButterworthLPF };
	static const CClass classCGAButterworthLPF;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	// IGAButterworthFilter
    virtual void	Update();

	// IGAButterworthLPF


	// CTOR / DTOR
	CGAButterworthLPF();
	~CGAButterworthLPF();

protected:




};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAButterworthLPF_
// --------------------------------------------------------------------------------------------------------
