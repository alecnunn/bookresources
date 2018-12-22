// PlugInDialog.cpp: implementation of the CPlugInDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSPdemo.h"
#include "PlugIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlugIn::CPlugIn( unsigned nIDTemplate, CWnd *Parent ) :
	CDialog( nIDTemplate, Parent ),
	m_nIDTemplate( nIDTemplate ),
	m_NotifyPostRequested( false )
{}

bool CPlugIn::NotifyParameterChange() const
{
	m_NotifyPostRequested = true;
	return true;
}

bool CPlugIn::OkToPostNotify() const
{
	if( m_NotifyPostRequested && GetParent() ) {
		m_NotifyPostRequested = false;
		NMHDR nmhdr;
		memset( &nmhdr, 0, sizeof( nmhdr ) );
		nmhdr.hwndFrom = m_hWnd;
		nmhdr.idFrom = m_nIDTemplate;
		nmhdr.code = PLUGIN_PARAMETERCHANGE;
		return 0 != GetParent()->PostMessage( WM_USER, IDC_EFFECTAREA, 0/* (long) &nmhdr */);
	} else
		return false;
}
