/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MiscNM.cpp
 *    Desc: Miscellaneous Net Messages
 *          Grok NetMessages.h before you explore here.
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MISCNM_H
#define _MISCNM_H

#include "NetMessages.h"


//=================================================================

/**
 * Message: Score Change
 * ID: 13
 * Desc: When someone dies, someone's score changes.  Just takes
 * a player ID and a new score.  Generally broadcast to all clients
 * whenever the score changes.
 */
class cNM_ScoreChange
: public cNetMessage
{
	int m_entID; // ID of the object that has been added.
	int m_newScore; // New score for that object.

public:

	cNM_ScoreChange( 
		int entID, 
		int newScore );
	cNM_ScoreChange(){}

	virtual int SerializeTo( uchar* pOutput );
	virtual void SerializeFrom( uchar *pFromData, int datasize );
	virtual void Exec();

protected:
};


#endif//_MISCNM_H