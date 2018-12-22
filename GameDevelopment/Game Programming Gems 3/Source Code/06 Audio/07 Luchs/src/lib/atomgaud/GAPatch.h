// GAPatch - Interface
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

#ifndef _atomosGAPatch_
#define _atomosGAPatch_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:


interface IGAPatch : public IGAProcessor
{

	virtual void	Trigger()=0;
	virtual void	Start()=0;
	virtual void	Stop()=0;
	virtual IGAProcessors* GetProcessors()=0;
	virtual BOOL	Connect(UINT32 nFrom, UINT32 nTo, UINT32 nPinFrom = SampleOut, UINT32 nPinTo = SampleIn)=0;

}; // interface


class ATOMGAUD_DLLCLASS CGAPatch : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAPatch };
	static const CClass classCGAPatch;
	virtual const IClass*		GetClass();

	virtual void Terminate();
	virtual UINT32	Process(IObject* pob=NULL);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAPatch
	virtual void Trigger();
	virtual void Start();
	virtual void Stop();
	virtual IGAProcessors* GetProcessors();
	virtual BOOL	Connect(UINT32 nFrom, UINT32 nTo, UINT32 nPinFrom = SampleOut, UINT32 nPinTo = SampleIn);


	// CTOR / DTOR
	CGAPatch();
	~CGAPatch();

protected:

	CGAProcessors	m_Processors; // the processor chain


};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAPatch_
// --------------------------------------------------------------------------------------------------------
