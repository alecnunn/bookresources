// ----------------------------------------------------------------------------
// 
// This file is part of the XDS Toolkit, Copyright (C) 2003 Mark T. Price
// For conditions of distribution and use, see XdsLicense.h
//
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "XDSliteAPI.h"
#include "XDSconfig.h"

const char *szProgName = "xdsConvert";
const char *szVersion  = "1.00";
const char *szDate     = "(2003-09-01)";
bool        bVerbose   = true;


#ifdef WIN32
#define OPEN_BINARY_READ  "rb"
#define OPEN_BINARY_WRITE "wb"
#else
#define OPEN_BINARY_READ  "r"
#define OPEN_BINARY_WRITE "w"
#endif // WIN32

extern "C" int optind;
extern "C" char *optarg;
extern "C" int getopt(int argc,char **argv,char *opt);


#define EXCLUDE_COMMENTS    0x0001
#define EXCLUDE_SIGNATURE   0x0002	// doesn't currently do anything (since DSD is separate from stream)
#define EXCLUDE_NAME        0x0004	// doesn't currently do anything (since DSD is separate from stream)
#define EXCLUDE_DSD         0x0008	// doesn't currently do anything (since DSD is separate from stream)

char *g_Operation;
char *g_File;

struct xdsHandle *g_hXdsOutput;
char *g_szOutputFile;
unsigned int g_iExcludeMap;

void usage(void)
{
	//      01234567890123456789012345678901234567890123456789012345678901234567890123456789
	printf("usage: xdsConvert [-q] [ –d DSD-file ] [ -x DSD | Comments | Signature | Name ]\n"
	       "                  [ -o output-file ] input-files\n"
		   "  -q = quiet mode\n"
	       "  -d = specify Data Stream Definition file\n"
	       "  -x = exclude named items from output file\n"
		   "  -o = specify output file (will written in XDS-format)\n"
	       "  input-files - list of files to convert/combine to output file\n");
	exit(1);
}

unsigned short xdsNodeClass(struct xdsHandle *hXds, unsigned short iType);
unsigned long  xdsNodeSize(struct xdsHandle *hXds, unsigned short iType);

void XDS_PROCESSNODE(unsigned short nodeType, void *nodeData, int nodeSize)
{
	// write node to output
	switch(xdsNodeClass(g_hXdsOutput, nodeType))
	{
	case XDS_RECORD_RECORDDEF:
		xdsStartRecord(g_hXdsOutput, nodeType);
		break;

	case XDS_RECORD_COMMENT:
		if(g_iExcludeMap & EXCLUDE_COMMENTS)
			return;
		xdsWriteComment(g_hXdsOutput, (char *)nodeData);
		break;

	case XDS_RECORD_ELEMDEF:
	case XDS_RECORD_ATTRIBDEF:
	{
		char *szOperation = g_Operation;
		char *szFile      = g_File;
		g_Operation = "Writing";
		g_File      = g_szOutputFile;
		xdsWriteNode(g_hXdsOutput, nodeType, nodeData, nodeSize / xdsNodeSize(g_hXdsOutput, nodeType));
		g_Operation = g_Operation;
		g_File      = g_File;
		break;
	}

	case XDS_RECORD_ENDOFDATA:
		break;
	}
}

void XDS_ERROR(const char *errText)
{
	fprintf(stderr, "%s: error %s '%s' - %s\n", szProgName, g_Operation, g_File, errText);
	exit(1);
}


#define READ_BUFFER_SIZE        1024
#define BASE_BUFFER_ALLOC_SIZE  1024
// loads a binary file into an allocated buffer...
unsigned char *loadFile(char *fname, unsigned long *iSize)
{
	FILE          *fp;
	unsigned char  readBuffer[READ_BUFFER_SIZE];
	int            iReadLength = 0;
	unsigned char *fileBuffer;
	int            iBufLength  = 0;
	int            iBufAlloc   = 0;

	// open file
	if((fp = fopen(fname, OPEN_BINARY_READ)) == NULL)
	{
		fprintf(stderr, "%s: could not open '%s' file for reading - %s\n", szProgName, fname, strerror(errno));
		exit(1);
	}

	do {
		if(iBufLength + iReadLength > iBufAlloc)
		{
			if(!iBufAlloc)
				iBufAlloc = BASE_BUFFER_ALLOC_SIZE;
			while(iBufLength + iReadLength > iBufAlloc)
				iBufAlloc <<= 1;

			// (re)allocate buffer
			fileBuffer = (unsigned char *)malloc(iBufAlloc);
			if(fileBuffer == NULL)
			{
				fprintf(stderr, "%s: could not allocate memory for buffer\n", szProgName);
				fclose(fp);
				exit(1);
			}
		}

		if(iReadLength)
		{
			memcpy(&fileBuffer[iBufLength], &readBuffer[0], iReadLength);
			iBufLength += iReadLength;
		}

		iReadLength = fread(&readBuffer[0], 1, READ_BUFFER_SIZE, fp);
	} while(iReadLength > 0);

	fclose(fp);

	if(iSize)
		*iSize = iBufLength;
	return fileBuffer;
}


int main(int argc, char *argv[])
{
	int   opt;
	char *szDSDFile = NULL;

	g_szOutputFile = NULL;
	g_iExcludeMap = 0;

	printf("%s %s %s\n", szProgName, szVersion, szDate);

	while((opt = getopt(argc, argv, "qd:x:o:")) != EOF)
	{
		switch (opt)
		{
		default:
			usage();

		case 'q':
			bVerbose = false;
			break;

		case 'd':
			szDSDFile = optarg;
			break;

		case 'x':
			if(!stricmp("dsd", optarg))
				g_iExcludeMap |= EXCLUDE_DSD;
			else if(!stricmp("comments", optarg))
				g_iExcludeMap |= EXCLUDE_COMMENTS;
			else if(!stricmp("name", optarg))
				g_iExcludeMap |= EXCLUDE_NAME;
			else if(!stricmp("signature", optarg))
				g_iExcludeMap |= EXCLUDE_SIGNATURE;
			else
			{
				fprintf(stderr, "Unknown exclusion argument '%s'\n", optarg);
				usage();
			}
			break;

		case 'o':
			g_szOutputFile = optarg;
			break;
		}
	}

	if(!szDSDFile)
	{
		fprintf(stderr, "%s: Missing DSD file\n", szProgName);
		usage();
	}
	if(!g_szOutputFile)
	{
		fprintf(stderr, "%s: Missing output file\n", szProgName);
		usage();
	}
	if(optind >= argc)
	{
		fprintf(stderr, "%s: No input files specified!\n", szProgName);
		usage();
	}

	for(int ii = optind; ii < argc; ++ii)
	{
		if(!stricmp(g_szOutputFile, argv[ii]))
		{
			fprintf(stderr, "%s: cannot input and output same file\n", szProgName);
			exit(1);
		}
	}


	// read in the DSD
	unsigned char *pDsd = loadFile(szDSDFile, NULL);

	// open the target file
	FILE *fp = fopen(g_szOutputFile, OPEN_BINARY_WRITE);
	if(!fp)
	{
		fprintf(stderr, "%s: could not open output file '%s' - %s\n", szProgName, g_szOutputFile, strerror(errno));
		exit(1);
	}
	g_Operation = "reading";
	g_File = szDSDFile;
	g_hXdsOutput = xdsInit(fp, NULL, pDsd, XDS_IOMODE_WRITE);
	if(!g_hXdsOutput)
	{
		fprintf(stderr, "%s: Error initializing target file '%s'\n", szProgName, g_szOutputFile);
		exit(1);
	}

	for(; optind < argc; ++optind)
	{
		FILE *fpi = fopen(argv[optind], OPEN_BINARY_READ);
		if(!fp)
		{
			fprintf(stderr, "%s: could not open input file '%s' - %s\n", szProgName, argv[optind], strerror(errno));
			exit(1);
		}

		struct xdsHandle *hXds = xdsInit(fpi, NULL, pDsd, XDS_IOMODE_READ);
		if(!hXds)
		{
			fprintf(stderr, "%s: Error initializing source file '%s'\n", szProgName, argv[optind]);
			exit(1);
		}

		if(bVerbose)
			printf("%s: converting '%s'\n", szProgName, argv[optind]);

		g_File = argv[optind];
		g_Operation = "Reading";

		bool bReadOkay = false;
		do{
			bReadOkay = xdsReadRecord(hXds);
			(void)xdsEndRecord(g_hXdsOutput);
		}
		while(bReadOkay);

		xdsFini(hXds);
		fclose(fpi);
	}

	xdsFini(g_hXdsOutput);
	fclose(fp);

	return 0;
}