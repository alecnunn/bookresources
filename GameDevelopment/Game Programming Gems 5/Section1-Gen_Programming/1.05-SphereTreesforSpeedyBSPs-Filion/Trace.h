

#ifndef _TRACE_H_
#define _TRACE_H_

inline void Trace( const char* szString, ... )
{
	va_list ArgList;

	va_start( ArgList, szString );
	char szTemp[255];
	_vsnprintf( szTemp, 255, szString, ArgList );
	OutputDebugString( szTemp );
	va_end( ArgList );
};

#endif	// _TRACE_H_