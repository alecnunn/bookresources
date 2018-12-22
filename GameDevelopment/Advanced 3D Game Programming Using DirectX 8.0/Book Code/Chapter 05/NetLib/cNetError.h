/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cNetError.h: interface for the cNetError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNETERROR_H__B0419ACA_D198_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CNETERROR_H__B0419ACA_D198_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cNetError  
{
protected:
	char d_text[ 1000 ];

public:
	cNetError( const char *pText );
	cNetError();
	virtual ~cNetError();


  const char *Text()
  {
    return (const char *)d_text;
  }
};

#endif // !defined(AFX_CERROR_H__B0419ACA_D198_11D3_AE4F_00E029031C67__INCLUDED_)
