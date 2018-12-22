//==============================================================
//= Timer.h
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= A simple and effective timing class
//==============================================================
#ifndef __DemoFramework_Timer_H__
#define __DemoFramework_Timer_H__

#include <SDL.h>
#include <cmath>

namespace DemoFramework {
	//a multiplatform timer class
	class Timer {
		//FPS variables
		float m_startTime;
		float m_elapsedTime;
		float m_resolution;
		int m_frequency;

		float m_diff, m_t1, m_t2;
		float m_fps;
		int m_elapsedFrames;


	public:

		//update the timer system
		void Update( void ) {
			//increase the number of frames that have passed
			m_elapsedFrames++;

			if( ( m_elapsedFrames % 5 ) == 1 )
			  m_t1= GetTime( )/1000;
			else if( ( m_elapsedFrames % 5 ) == 0 )  {
				m_t2= GetTime( )/1000;
				m_diff= ( float )fabs( m_t2 - m_t1 );      
			}  

			m_fps= 5/( m_diff );
		}

		//get the elapsed program time
		float GetTime( void ) {
			//return the time since the program started
			return ( ( float )( SDL_GetTicks( ) - m_startTime )*m_resolution )*1000.0f;
		}

		//get the current number of frames per second
		inline float GetFPS( void ) {	 return m_fps;	}

		Timer( void ) : m_elapsedFrames( 0 ), m_fps( 0 ), m_t1( 0 ), m_t2( 0 ) {
			m_startTime= ( float )SDL_GetTicks( );		//record the time the program started
			m_elapsedTime= m_startTime;					//initialize the elapsed time variable
			m_resolution= 1.0f/1000.0f;
			m_frequency= 1000;
		}
		~Timer( void ) { }
	};
}


#endif	//__DemoFramework_Timer_H__