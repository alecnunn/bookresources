// PlugInDialog.h: interface for the CPlugInDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINDIALOG_H__56208FA9_18DF_4383_915E_BC2B9F14A7CE__INCLUDED_)
#define AFX_PLUGINDIALOG_H__56208FA9_18DF_4383_915E_BC2B9F14A7CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template< typename >
class cSoundBuffer;

#include <complex>

#define PLUGIN_PARAMETERCHANGE (WM_USER + 1);

class CPlugIn : public CDialog  
{
	unsigned m_nIDTemplate;
	mutable bool m_NotifyPostRequested;
public:
	// which transfer function ?
	enum { left = 0, right = 1 };

	CPlugIn( unsigned nIDTemplate, CWnd *Parent = 0 );
	virtual void CreateFromTemplate( CWnd *parent ) = 0;
	virtual void Process( cSoundBuffer<float> &buffer ) {};
	virtual void ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR ) {};
	virtual void Reset() {};
	virtual std::complex<float> TransferFunction( const std::complex<float> & ) const
		{ return std::complex<float>( 1, 0 ); }
	virtual bool NotifyParameterChange() const;
	virtual bool OkToPostNotify() const;
	virtual const char *Name() const { return "Void PlugIn"; }
};

#endif // !defined(AFX_PLUGINDIALOG_H__56208FA9_18DF_4383_915E_BC2B9F14A7CE__INCLUDED_)
