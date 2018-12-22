/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameErrors.h
 *    Desc: errors that game functions can have
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMEERRORS_H
#define _GAMEERRORS_H

#include <string>
#include "GameGlobals.h"

//==========--------------------------  

/**
 * This code is slow, using dynamic memory, but since we only use them
 * when something Really Bad happens, the user won't notice that their
 * application exits abmornally a few nanoseconds slower than usual
 */
class cGameError
{
	std::string m_errorText;
public:
	cGameError( const char* errorText )
	{
		DP1("***\n*** [ERROR] cGameError thrown! text: [%s]\n***\n", errorText );
		m_errorText = std::string( errorText );
	}

	const char* GetText()
	{
		return m_errorText.c_str();
	}
};

//==========--------------------------  

enum eResult
{
	resAllGood		= 0, // function passed with flying colors
	resFalse		= 1, // function worked and returns value 'false'
	resFailed		= -1, // function failed miserably
	resNotImpl		= -2, // function has not been implemented
	resForceDWord = 0x7FFFFFFF
};

inline bool Succeeded( eResult in )
{
	if( in >= 0 )
		return true;
	return false;
}

inline bool Failed( eResult in )
{
	if( in < 0 )
		return true;
	return false;
}

#endif //_GAMEERRORS_H