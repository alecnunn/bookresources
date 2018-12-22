//==============================================================
//= log.cpp
//= Original coders: Evan Pipho (evan@codershq.com) and
//=					 Trent Polack (trent@codershq.com)
//==============================================================
//= The routines for the run-time log (HTML output) component
//==============================================================
#include <cstdarg>
#include <cstdio>
#include <ctime>

#include "Log.h"
using namespace DemoFramework;

Log g_log( "ProgramLog.htm" );

const char *Log::m_colors[4]= {
	"#00FF00",
	"#FF0000",
	"#FFFFFF",
	"#4995B1"
};

//initialize the log system
bool Log::Init( const char *filename ) {
	FILE *pFile;
	time_t ttime;
	tm *pCurTime;
	char LogStart[128];

	m_filename= filename;

	//get the current time
	time( &ttime );
	pCurTime= localtime( &ttime );
	strftime( LogStart, 128, "Log Started on %m/%d/%Y at %H:%M:%S", pCurTime );

	//open the log file
	pFile= fopen( filename, "wt" );

	if( !pFile )	return false;

	//setup the HTML file	
	fprintf( pFile, "<HTML>\n<TITLE>%s</TITLE>\n"
					"<BODY BGCOLOR= 000000>\n"
					"<FONT COLOR= FFFFFF>%s</FONT><BR><BR>\n"
					"</BODY>\n</HTML>", LogStart, LogStart );

	//enable the log
	m_isEnabled= true;

	fclose( pFile );
	return true;
}

//write a line of text to the log
void Log::Write( LogColors color, const char *string, ... ) {
	FILE* pFile;
	va_list va;
	time_t ttime;
	tm* pCurTime;

	char parsedString[1024];
	char logTime[32];

	if( !m_isEnabled )	return;

	//get the time
	time( &ttime );
	pCurTime= localtime( &ttime );
	strftime( logTime, 32, "<PRE>%H:%M:%S    ", pCurTime );

	//parse the given string
	va_start( va, string );
		vsprintf( parsedString, string, va );
	va_end( va );

	//open the log file
	pFile= fopen( m_filename, "r+" );
	if( !pFile )	return;

	//write text to the log file
	fseek( pFile, -16, SEEK_END );
	fprintf( pFile, "<FONT COLOR= \"%s\">%s %s</PRE></FONT>\n"
					"</BODY>\n</HTML>", m_colors[color], logTime, parsedString );
	fclose( pFile );
}
