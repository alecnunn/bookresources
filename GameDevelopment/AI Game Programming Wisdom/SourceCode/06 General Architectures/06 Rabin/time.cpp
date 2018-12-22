/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */

#include "time.h"
#include <windows.h>
#include <assert.h>

#define INITIAL_GAME_SPEED 1.0f
#define MIN_GAME_SPEED 0.001f
#define MAX_GAME_SPEED 100.0f


Time::Time( void )
{
	m_CurrentTime = 0.0f;
	m_TimeLastTick = 0.001f;
	m_GameSpeed = INITIAL_GAME_SPEED;

	m_StartTime = ( GetExactTimeInMilliseconds() * INITIAL_GAME_SPEED ) / 1000.0f;

}


void Time::MarkTimeThisTick( void )
{
	float newTime = ( ( GetExactTimeInMilliseconds() * m_GameSpeed ) / 1000.0f ) - m_StartTime;

	m_TimeLastTick = newTime - m_CurrentTime;
	m_CurrentTime = newTime;

	if( m_TimeLastTick <= 0.0f ) {
		m_TimeLastTick = 0.001f;
	}

}


void Time::SetGameSpeed( float value )
{
	assert( value >= MIN_GAME_SPEED && value <= MAX_GAME_SPEED && "Time::SetGameSpeed - out of range" );

	m_GameSpeed = value;

	if( m_GameSpeed < MIN_GAME_SPEED ) {
		m_GameSpeed = MIN_GAME_SPEED;
	}
	else if ( m_GameSpeed > MAX_GAME_SPEED ) {
		m_GameSpeed = MIN_GAME_SPEED;
	}

}


unsigned int Time::GetExactTimeInMilliseconds( void )
{
	return( timeGetTime() );
}


