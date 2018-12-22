/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
//-----------------------------------------------------------------------------
//
//  DeprecationMgr.h
//
/**      \class  DeprecationMgr
//      Tally usage of deprecated functions and report at end of execution.
//
//      Noel Llopis  --  11/20/2000
*/
//-----------------------------------------------------------------------------


#ifndef _DEPRECATIONMGR_H_
#define _DEPRECATIONMGR_H_

// Identifier was truncated to '255' characters in the debug information
#pragma warning( disable:4786 )  


#include <string>
#include <map>
#include <set>


#ifdef _DEBUG
#define DEPRECATE(a,b) { \
		void * fptr;	\
		_asm { mov fptr, ebp }	\
		DeprecationMgr::GetInstance()->AddDeprecatedFunction(a, b, fptr); \
	}
#else
#define DEPRECATE(a,b)
#endif


class DeprecationMgr
{
  public:
	static DeprecationMgr * GetInstance ( void );

	~DeprecationMgr ( void );

	bool AddDeprecatedFunction ( const char * OldFunctionName, 
								 const char * NewFunctionName,
								 const void * FramePtr );

  private:
	struct DeprecatedFunction
	{
		const char *  OldFunctionName;
		const char *  NewFunctionName;
		std::set<int> CalledFrom;
	};

	DeprecationMgr ( void );

	std::map<const char *, DeprecatedFunction> m_Functions;
};


#endif //_DEPRECATIONMGR_H_
