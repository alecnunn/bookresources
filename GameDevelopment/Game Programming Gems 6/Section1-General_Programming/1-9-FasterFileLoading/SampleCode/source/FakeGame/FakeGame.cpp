// FakeGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "performance.h"
#include "fileinterface.h"
#include "CommandLineParser.h"
#include "ResourceParser.h"
#include "ResourceCache.h"
#include "ResourceFile.h"
#include "defs.h"

namespace
{
	enum EFileAccessMode
	{
		FILE_SYSTEM_OS = 0,
		FILE_SYSTEM_RESOURCEFILE
	};

	EFileAccessMode g_FileAccessMode = FILE_SYSTEM_OS;
}



int main(int argc, char** argv)
{
	char szStartupFilename[512];
	memset(szStartupFilename, 0, 512);

	char szResourceName[512];
	memset(szResourceName, 0, 512);

	bool bLog = false;

	//Parse the command line
	CommandLineParser cmdParser;
	cmdParser.Parse(argc, argv);

	ECommandLineResult cmdResult;
	cmdParser.Find("-log", cmdResult);
	if(cmdResult == CMDLINE_RESULT_EXISTS)
	{
		bLog = true;
	}


	const char* pArg = cmdParser.Find("-resource_dir", cmdResult);
	if((cmdResult == CMDLINE_RESULT_EXISTS)&&(pArg != NULL))
	{
		g_FileAccessMode = FILE_SYSTEM_OS;
		strncpy(szResourceName, pArg, 511);
	}
	else
	{
		pArg = cmdParser.Find("-resource_file", cmdResult);
		if((cmdResult == CMDLINE_RESULT_EXISTS)&&(pArg != NULL))
		{
			g_FileAccessMode = FILE_SYSTEM_RESOURCEFILE;
			strncpy(szResourceName, pArg, 511);
		}
		else
		{
			printf("Error: No Valid Resources...\n");
			return 1;
		}

	}

	pArg = cmdParser.Find("-Startup", cmdResult);
	
	if(pArg)
	{
		strncpy(szStartupFilename, pArg, 511);
	}
	else
	{
		printf("Error: No Startup file...\n");
		return 1;
	}

	if((cmdResult == CMDLINE_RESULT_EXISTS) && (szStartupFilename != NULL))
	{
		char szBuf[512];
		FileList files;
		printf("Parsing files!\n");
		ResourceParser parser;
		parser.Parse(szStartupFilename, files, "level1"); //only worry about level 1.
		printf("Done!\n");

		// How many files do we need to load from disk or cache?
		printf("%d files to process.\n", files.size());

		// Setup our file list iterator.
		FileList::iterator iter = files.begin();

		// Create our file cache.
		ResourceCache fileCache;

		// Start profiler.
		START_TIME();

		switch(g_FileAccessMode)
		{
		case FILE_SYSTEM_OS:
			{
				// Create a file interface for loading files from disk
				FileInterface fileInterface;
				while(iter != files.end())
				{
					// Do something with the file	
					sprintf(szBuf, "%s\\%s", szResourceName, (*iter).c_str());

					uint8* pData;
					bool bFileInCache = fileCache.Find(szBuf, pData);

					// Is it in the cache? or do we need to load it from disk?
					if(!bFileInCache)
					{
						
						if(bLog)
						{
							FILE* fp = fopen("filelog.txt", "a");
							if(fp)
							{
								fprintf(fp, "%s\n", szBuf);
								fclose(fp);
							}
						}						

						FileStats stats;
						if(fileInterface.StatFile(szBuf, &stats))
						{
							BaseFile* pFile = fileInterface.OpenFile(szBuf, BINARY_FILE, READONLY);
							if(pFile)
							{
								// Read in the file in one big chunk.
								pData = new uint8[stats.nSize];
								pFile->Read(pData, stats.nSize);

								fileCache.Add(szBuf, pData);
								fileInterface.CloseFile(pFile);
							}
						}
					}
					++iter;
				}
			}
			break;
		case FILE_SYSTEM_RESOURCEFILE:
			{

				ResourceFile resFile;
				GEN_RESULT gen_result = resFile.Open(szResourceName);
				if (gen_result != RESULT_OK)
				{
					//ERROR!
					printf("Error: failed to open resource file (%s)\n", szResourceName);
					break;
				}
				while(iter != files.end())
				{
					// Do something with the file	
					sprintf(szBuf, "%s", (*iter).c_str());

					uint8* pData;
					bool bFileInCache = fileCache.Find(szBuf, pData);

					// Is it in the cache? or do we need to load it from disk?
					if(!bFileInCache)
					{
						ResourceFileStats stats;
						BaseFile* pFile = resFile.Find(szBuf, &stats);
						if(pFile)
						{
							// Read in the file in one big chunk
							pData = new uint8[stats.nFileSize];
							pFile->Read(pData, stats.nFileSize);

							fileCache.Add(szBuf, pData);
							resFile.ClosePackedFile(pFile);
						}
					}
					++iter;
				}

				resFile.Close();
			}
			break;
		default:
			break;
		}

		// Stop profiler.
		STOP_TIME();
		printf("Profile time: %f milliseconds\n", TIME_IN_MILLI);
		printf("%d files in cache.\n", fileCache.Size());
	}
	else
	{
		printf("Command argument \"-Startup\" not found!\n");
	}

	return 0;
}
