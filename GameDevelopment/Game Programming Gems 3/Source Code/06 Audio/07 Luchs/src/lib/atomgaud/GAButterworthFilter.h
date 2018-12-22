// GAButterworthFilter - Interface
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

#ifndef _atomosGAButterworthFilter_
#define _atomosGAButterworthFilter_


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// forward declarations:



interface IGAButterworthFilter : public IGAProcessor
{

	virtual void SetCutoff(FLOAT32 fCut)=0;
 	virtual void SetFrequency(FLOAT32 fFreqHz)=0;
 	virtual void SetFMAttenuation(FLOAT32 fAttenuation)=0;
  virtual void	Update()=0;


}; // interface


class ATOMGAUD_DLLCLASS CGAButterworthFilter : public CGAProcessor  
{
public:

	// IObject
	enum { CID = CID_GAButterworthFilter };
	static const CClass classCGAButterworthFilter;
	virtual const IClass*		GetClass();

	
	virtual void Terminate();
//	virtual BOOL	Initialize(IObject* pob);
	virtual UINT32	Process(IObject* pob);

	// IGAObject
	virtual BOOL	SetParameter(CHAR8* pszName, CHAR8* pszValue);

	// IGAButterworthFilter
	virtual void SetCutoff(FLOAT32 fCut);
	virtual void SetFrequency(FLOAT32 fFreqHz);
	virtual void SetFMAttenuation(FLOAT32 fAttenuation);


    virtual void	Update();


	// CTOR / DTOR
	CGAButterworthFilter();
	~CGAButterworthFilter();

protected:

	FLOAT32		m_fFMAttenuation;
	
	FLOAT32   m_fBaseFreq;
	FLOAT32   m_fCurrFreq;
	FLOAT32   m_bw;
	FLOAT32   m_C;
	FLOAT32		m_D;
	FLOAT32   m_a[3];
	FLOAT32	  m_b[2];

	FLOAT32  m_buf_x0;
	FLOAT32  m_buf_x1;
	FLOAT32  m_buf_y0;
	FLOAT32  m_buf_y1;



};

#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS

#endif // _atomosGAButterworthFilter_
// --------------------------------------------------------------------------------------------------------
