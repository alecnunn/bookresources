//==============================================================
//= log.h
//= Original coders: Evan Pipho (evan@codershq.com) and
//=					 Trent Polack (trent@codershq.com)
//==============================================================
//= The routines for the run-time log (HTML output) component
//==============================================================
#ifndef __DemoFramework_Log_H__
#define __DemoFramework_Log_H__

namespace DemoFramework {
	enum LogColors {
		LOG_SUCCESS= 0,
		LOG_FAILURE,
		LOG_PLAINTEXT,
		LOG_RENDERER
	};

	//a real-time logging class
	class Log {
		static const char *m_colors[4];

		const char *m_filename;		//the log filename (set at initiation)
		bool m_isEnabled;			//is the log currently enabled?

		public:
		

		bool Init( const char *filename );
		void Write( LogColors color, const char *text, ... );

		inline void Enable( void ) { m_isEnabled= true; }
		inline void Disable( void ) { m_isEnabled= false; }
		inline bool IsEnabled( void ) { return m_isEnabled; }

	Log( void ) : m_isEnabled( false ) { }
	Log( const char *filename ) : m_isEnabled( false ) {
		Init( filename );
	}

	~Log( void ) { }
	};
}

extern DemoFramework::Log g_log;

#endif //__DemoFramework_Log_H__