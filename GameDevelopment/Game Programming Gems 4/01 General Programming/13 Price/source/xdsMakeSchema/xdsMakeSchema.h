#ifndef XDSMAKESCHEMA_INCL
#define XDSMAKESCHEMA_INCL

#include "stdlib.h"
#include "stdio.h"

#include "XDSinternal.h"

#ifdef WIN32
//#define  OPEN_MODE   O_RDONLY|O_BINARY
#define OPEN_MODE "rb"
#define MAXPATHLEN FILENAME_MAX
#define OPEN_BINARY_WRITE "wb"
#else
//#define  OPEN_MODE   O_RDONLY
#define OPEN_MODE "r"
#define OPEN_BINARY_WRITE "w"
#include <sys/param.h>
#endif // WIN32
#define OPEN_TEXT_WRITE "w"



// xdsMakeSchema.cpp
extern xdsHandle  *g_hXds;
extern const char *szProgName;
extern bool        g_bVerbose;
extern unsigned short g_iArrayLengthSize;
extern unsigned short g_iStringLengthSize;
extern unsigned short g_iPointerLengthSize;
extern unsigned short g_iEnumLengthSize;

// genDotH.cpp
extern void writeDsdDotH(FILE *fp, struct xdsHandle *hXds);
extern CxdsBuffer xdsIoBuffer;

// genSchema.cpp
extern void writeXMLSchema(FILE *fp, struct xdsHandle *hXds);

// Helpers
inline const char *makeStreamName(struct xdsHandle *hXds)
{
	const char *sz = xdsGetStreamName(hXds);
	if(sz && *sz)
		return sz;
	else
		return "xdsStream";
}

#endif // XDSMAKESCHEMA_INCL