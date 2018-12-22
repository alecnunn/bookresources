// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <string>

#include "xdsMakeSchema.h"
using namespace std;

#define DEFAULT_RECORD_LENGTH_SIZE  4
#define DEFAULT_ARRAY_LENGTH_SIZE   2
#define DEFAULT_STRING_LENGTH_SIZE  1
#define DEFAULT_POINTER_LENGTH_SIZE sizeof(void*)
#define DEFAULT_ENUM_LENGTH_SIZE    sizeof(int)

#define DEFAULT_STREAMNAME "xdsStream"


const char *szProgName = "xdsMakeSchema";
const char *szVersion  = "1.00";
const char *szDate     = "(2003-09-01)";
xdsHandle  *g_hXds = NULL;
bool        g_bVerbose = true;

unsigned short g_iArrayLengthSize   = DEFAULT_ARRAY_LENGTH_SIZE;
unsigned short g_iStringLengthSize  = DEFAULT_STRING_LENGTH_SIZE;
unsigned short g_iPointerLengthSize = DEFAULT_POINTER_LENGTH_SIZE;
unsigned short g_iEnumLengthSize    = DEFAULT_ENUM_LENGTH_SIZE;


extern "C" {
#include "sn.h"
//	void MacroFreeTable();
extern void MacroReadFile(char *Filename);
extern int AddMacro(int typ, char *macrodef);
extern void f_MacroFreeAll(void);
extern void free_lex_buffers();
extern void free_token_buffers();

extern FILE *yyfp;
}

// routines defined in cppDriver.cpp
extern int log_symbol_filename(char *fname);
extern void start_parser( char *filename_a, int is_cpp);

void XDS_ERROR(const char *errText)
{
	//	fprintf(stderr, "%s: error %s '%s' (%s) -- conversion aborted\n", szProgName, g_Operation, g_File, errText);
	fprintf(stderr, "%s: error (%s) -- extraction aborted\n", szProgName, errText);
	exit(1);
}



// getext - returns pointer to extender of passed file name
//
static const char *getext(const char *p)
{
	if(!p)
		return("");

	while(*p != '\0')
	{
		if(*p++ == '.')
			return(p);
	}

	return("");
}

static FILE *openOutputFile(const char *filename, const char *mode)
{
	FILE *fp = fopen(filename, mode);
	if(!fp)
	{
		fprintf(stderr, "%s: could not open output file '%s' - %s\n", szProgName, filename, strerror(errno));
		exit(1);
	}
	return fp;
}

// print usage information
void usage(void)
{
	//      12345678901234567890123456789012345678901234567890123456789012345678901234567890
	printf("usage: xdsMakeSchema [-q] [-a size] [-t size] [-p size] [-e size] [-C]\n"
	       "                     [-m macroFile] [-D name [= definition ]] [-U name]\n"
		   "                     [-s streamName] [-r recordName [: lengthSize ]]\n"
		   "                     [-o outFile] input-files\n"
		   "  -q = quiet mode (don't report skipped classes / variables)\n"
		   "  -a = length size for arrays (default is %d)\n"
		   "  -t = length size for strings (must be 1 or 2, default is %d)\n"
		   "  -p = length size for pointers (default is %d)\n"
		   "  -e = length size for enumerators (default is %d)\n"
		   "  -C = parse as 'C' code instead of 'C++ code\n"
	       "  -m = parse macros in macro file\n"
	       "  -D = define macro\n"
	       "  -U = undefine macro\n"
		   "  -s = define XDS stream name (default stream name is " DEFAULT_STREAMNAME ")\n"
		   "  -r = define XDS record (default length size is %d)\n"
	       "  -o = specify output file (*.xsd, *.dsd or *.h/hpp/hxx)\n",
		   DEFAULT_ARRAY_LENGTH_SIZE, DEFAULT_STRING_LENGTH_SIZE,
		   DEFAULT_POINTER_LENGTH_SIZE, DEFAULT_ENUM_LENGTH_SIZE, DEFAULT_RECORD_LENGTH_SIZE);
	exit(1);
}

extern "C" int optind;
extern "C" char *optarg;

int
main(int argc, char *argv[])
{
	int   opt;
	int parse_cplpl = TRUE;
	char *docName = DEFAULT_STREAMNAME;
	char *dsdFileName = NULL;
	char *xmlFileName = NULL;
	char *hFileName = NULL;
	list<string> lRecords;

	printf("%s %s %s\n", szProgName, szVersion, szDate);

	while((opt = getopt(argc, argv, "qa:t:p:e:Cm:D:U:s:r:o:")) != EOF)
	{
		switch (opt)
		{
		default:
			usage();

		case 'q':
			g_bVerbose = false;
			break;

		case 'a':
		{
			char *ep;
			g_iArrayLengthSize = (unsigned short)strtol(optarg, &ep, 10);
			if(!ep || *ep || (g_iArrayLengthSize != 1 && g_iArrayLengthSize != 2 && g_iArrayLengthSize != 4))
			{
				fprintf(stderr, "%s: invalid array length size '%s' - must be 1, 2, or 4\n", szProgName, optarg);
				usage();
			}
			break;
		}

		case 't':
		{
			char *ep;
			g_iStringLengthSize = (unsigned short)strtol(optarg, &ep, 10);
			if(!ep || *ep || (g_iStringLengthSize != 1 && g_iStringLengthSize != 2))
			{
				fprintf(stderr, "%s: invalid string length size '%s' - must be 1 or 2\n", szProgName, optarg);
				usage();
			}
			break;
		}

		case 'p':
		{
			char *ep;
			g_iPointerLengthSize = (unsigned short)strtol(optarg, &ep, 10);
			if(!ep || *ep || (g_iPointerLengthSize != 1 && g_iPointerLengthSize != 2 && g_iPointerLengthSize != 4))
			{
				fprintf(stderr, "%s: invalid pointer length size '%s' - must be 1, 2, or 4\n", szProgName, optarg);
				usage();
			}
			break;
		}

		case 'e':
		{
			char *ep;
			g_iEnumLengthSize = (unsigned short)strtol(optarg, &ep, 10);
			if(!ep || *ep || (g_iEnumLengthSize != 1 && g_iEnumLengthSize != 2 && g_iEnumLengthSize != 4))
			{
				fprintf(stderr, "%s: invalid enumerator length size '%s' - must be 1, 2, or 4\n", szProgName, optarg);
				usage();
			}
			break;
		}

		case 'C': // Parse files as C and not as C++!
			parse_cplpl = FALSE;
			break;

		case 'm':
			MacroReadFile(optarg);
			break;

		case 'D':
			AddMacro(PAF_WORD_DEFINE, optarg);
			break;

		case 'U':
			AddMacro(PAF_WORD_UNDEF, optarg);
			break;

		case 's':
			docName = optarg;
			break;

		case 'r':
		{
			string sRec = optarg;
			lRecords.push_back(sRec);
			break;
		}

		case 'o':
		{
			const char *ext = getext(optarg);
			if(!stricmp(ext, "xsd"))		// output XML Schema file
				xmlFileName = optarg;
			else if(!stricmp(ext, "dsd"))	// output Data Stream Definition file
				dsdFileName = optarg;
			else if(!stricmp(ext, "h") || !stricmp(ext, "hpp") || !stricmp(ext, "hxx"))
				hFileName = optarg;
			else
			{
				fprintf(stderr, "%s: unknown extension on output file '%s'\n", szProgName, optarg);
				usage();
			}
			break;
		}
		}
	}

	// must specify at least one output file
	if(!xmlFileName && !dsdFileName && !hFileName)
	{
		fprintf(stderr, "%s: missing output file\n", szProgName);
		usage();
	}

	g_hXds = xdsInit(&xdsIoBuffer, docName, NULL, 'w');

	// define records (if any)
	if(!lRecords.empty())
	{
		list<string>::const_iterator ii = lRecords.begin();
		for(; ii != lRecords.end(); ++ii)
		{
			const char *szRecName = (*ii).c_str();
			const char *sp = strchr(szRecName, ':');
			if(sp)
			{
				// extract optional record length size
				char *szRecName2 = xdsStringNDup(szRecName, sp - szRecName);
				++sp;

				char *ep;
				long iSizeLen = strtol(sp, &ep, 10);
				if(!ep || *ep || (iSizeLen != 1 && iSizeLen != 2 && iSizeLen != 4))
				{
					fprintf(stderr, "%s: invalid record length size in %s\n", szProgName, szRecName);
					usage();
				}
				xdsDefRecord(g_hXds, szRecName2, (unsigned char)iSizeLen);
				free((void*)szRecName2);
			}
			else
				xdsDefRecord(g_hXds, szRecName, DEFAULT_RECORD_LENGTH_SIZE);
		}
	}
	else
	{
		char *sz = "xdsDataRecord";
		xdsDefRecord(g_hXds, sz, strlen(sz));
	}

	if(optind < argc)
	{
		for(; optind < argc; ++optind)
		{
			if(!log_symbol_filename(argv[optind]))
			{
				start_parser(argv[optind], parse_cplpl);
			}
		}
	}
	else
	{
		// parse stdin (for piped input)
		if(log_symbol_filename((char *)NULL) == 0)
		{
			start_parser(NULL, parse_cplpl);
		}
	}

	if(yyfp != NULL && yyfp != stdin)
		fclose(yyfp);

	free_lex_buffers();
	free_token_buffers();
	f_MacroFreeAll();

	xdsEndStream(g_hXds);

	if(dsdFileName)
	{
		FILE *fp = openOutputFile(dsdFileName, OPEN_BINARY_WRITE);
		if(g_bVerbose)
			printf("Writing XML Schema to '%s'\n", dsdFileName);
		if(fwrite(xdsIoBuffer.Data(), 1, xdsIoBuffer.Size(), fp) != xdsIoBuffer.Size())
		{
			fprintf(stderr, "%s: error writing to '%s' - %s\n", szProgName, dsdFileName, strerror(errno));
			exit(1);
		}
		fclose(fp);
	}

	if(xmlFileName)
	{
		FILE *fp = openOutputFile(xmlFileName, OPEN_TEXT_WRITE);
		if(g_bVerbose)
			printf("Writing XML Schema to '%s'\n", xmlFileName);
		writeXMLSchema(fp, g_hXds);
		fclose(fp);
	}

	if(hFileName)
	{
		FILE *fp = openOutputFile(hFileName, OPEN_TEXT_WRITE);
		if(g_bVerbose)
			printf("Writing XDS DSD and #defines to '%s'\n", hFileName);
		writeDsdDotH(fp, g_hXds);
		fclose(fp);
	}

	(void)xdsFini(g_hXds);
	return 0;
}
