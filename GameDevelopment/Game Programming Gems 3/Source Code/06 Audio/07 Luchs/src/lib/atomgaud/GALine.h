// GALine - Interface
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	The processor GALIne outputs a straight line of length 'duration'.
//				between  a start and an end point.
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





#ifndef _atomosGALine_
#define _atomosGALine_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGALine : public IGAProcessor
{

	virtual void	SetStartValue(FLOAT32 fStartValue)=0;
	virtual void	SetEndValue(FLOAT32 fEndValue)=0;


}; // interface


class ATOMGAUD_DLLCLASS CGALine : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GALine };
	static const CClass classCGALine;
	virtual const IClass*		GetClass();

	
	virtual void	Terminate();
	virtual UINT32	Process(IObject* pob);

	// IGAProcessor
	virtual void Trigger();
	virtual void Update();

	// IGALine


	virtual void	SetStartValue(FLOAT32 fStartValue);
	virtual void	SetEndValue(FLOAT32 fEndValue);



	// CTOR / DTOR
	CGALine();
	~CGALine();

protected:

	UINT32	m_nDuration;
	UINT32	m_nPos;		// current sample pos in duration

	FLOAT*	m_pfOutput; 

	FLOAT32	m_fLevel;	// current level
	FLOAT32	m_fStartValue;	// current level
	FLOAT32	m_fEndValue;	// current level
	FLOAT32 m_fIncr;

};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGALine_
// --------------------------------------------------------------------------------------------------------
