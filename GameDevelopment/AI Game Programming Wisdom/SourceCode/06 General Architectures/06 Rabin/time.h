/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#ifndef __TIME_H__
#define __TIME_H__

#include "singleton.h"


class Time : public Singleton <Time>
{
public:

	Time( void );
	~Time( void ) {}

	void MarkTimeThisTick( void );
	float GetElapsedTime( void )				{ return( m_TimeLastTick ); }
	float GetCurTime( void )					{ return( m_CurrentTime ); }

	void SetGameSpeed( float value );
	float GetGameSpeed( void )					{ return( m_GameSpeed ); }


private:

	float m_StartTime;
	float m_CurrentTime;
	float m_TimeLastTick;
	float m_GameSpeed;

	unsigned int GetExactTimeInMilliseconds( void );

};







#endif	// __TIME_H__