#include "stdafx.h"
#include "ResourceParser.h"
#include "fileinterface.h"
#include <stringtokenizer.h>

typedef std::vector<unsigned char> ByteBuffer;

#define MODE_LEVEL 0
#define MODE_VEHICLES 1
#define MODE_SPRITES 2
#define MODE_FLAGS 3
#define MODE_UNDEFINED 255

void ResourceParser::Parse(const char* szResourceList, FileList& fileList, const char* sLevelName)
{
	if(sLevelName == NULL)
	{
		return;
	}

	FileInterface fileSystem;
	fileSystem.Init();

	BaseFile* pFile = fileSystem.OpenFile(szResourceList, BINARY_FILE, READONLY);
	if(pFile)
	{
		unsigned char cByte;
		ByteBuffer byteBuffer;
		byteBuffer.reserve(32*1024); // Reserve a 64k buffer
		GEN_RESULT res = RESULT_OK;

		// Read in the text file as a stream of bytes
		while(res == RESULT_OK)
		{	
			res = pFile->Read(&cByte, 1);
			if(res == RESULT_OK)
			{
				byteBuffer.push_back(cByte);
			}
		}
		byteBuffer.push_back('\0'); // Make sure we null terminate it
		uint32 nSize = (uint32)byteBuffer.size() -1;

		// startup.txt specific vars
		char l_dir[128];
		l_dir[0] = '\0';
		char l_level[32];
		l_level[0] = '\0';
		bool l_bInDataSection = 0;
		int  l_nMode = MODE_UNDEFINED; // 0 = levels, 1 = vehicles, 2 = sprites, 3 = flags

		// Now do something with the data
		StringTokenizer data((char*)&byteBuffer[0], "\r\n");
		while(data.hasMoreTokens())
		{
			char* pTok = data.nextToken();

			if(stricmp(pTok, "{") == 0)
			{
				l_bInDataSection = true;
			}else
			if(stricmp(pTok, "}") == 0)
			{
				l_bInDataSection = false;
			}

			if(l_bInDataSection == true)
			{
				switch(l_nMode)
				{
				case MODE_LEVEL:
					{
						StringTokenizer lineTokenizer(pTok, ",");
						while(lineTokenizer.hasMoreTokens())
						{
							const char *pSubTok = lineTokenizer.nextToken();
							if(stricmp(pSubTok, "dir") == 0)
							{
								// Get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{						
										if(stricmp(pSubSubToken, sLevelName) != 0)
										{
											// We're not interested in this level so skip it.
											l_bInDataSection = false;
										}
										sprintf(l_dir, "levels\\%s", pSubSubToken);
									}
                                }
							}else
							if(stricmp(pSubTok, "MUSIC") == 0)
							{
								//get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										char szBuf[256];
										sprintf(szBuf, "music\\%s", pSubSubToken);
										fileList.push_back(szBuf);
									}
								}
							}else
								//Simple one arg lines
							if((stricmp(pSubTok, "TERRAINVERTS") == 0)||
								(stricmp(pSubTok, "WORLDOBJVERTS") == 0)||
								(stricmp(pSubTok, "THUMBNAIL") == 0)||
								(stricmp(pSubTok, "LOADSCREEN") == 0)||
								(stricmp(pSubTok, "SKY") == 0)
								)
							{
								//get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										char szBuf[256];
										sprintf(szBuf, "%s\\%s", l_dir, pSubSubToken);
										fileList.push_back(szBuf);
									}
								}
							}else
							if((stricmp(pSubTok, "RADAR") == 0) ||
								(stricmp(pSubTok, "MODEL") == 0))
							{
								//get next token
								while(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										if( (stricmp(pSubSubToken, "null") != 0) && (isalpha(pSubSubToken[0])) )
										{	
											char szBuf[256];
											sprintf(szBuf, "%s\\%s", l_dir, pSubSubToken);
											fileList.push_back(szBuf);
										}
									}
								}
							}


						}//line tokenizer has tokens
					}
					break;
				case MODE_VEHICLES:
					{
						StringTokenizer lineTokenizer(pTok, ",");
						while(lineTokenizer.hasMoreTokens())
						{
							const char *pSubTok = lineTokenizer.nextToken();
							if(stricmp(pSubTok, "dir") == 0)
							{
								//get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										strncpy(l_dir, pSubSubToken, 31);
									}
								}
							}else		
							if((stricmp(pSubTok, "MODEL") == 0)||
								(stricmp(pSubTok, "LEFTTREAD") == 0)||
								(stricmp(pSubTok, "RIGHTTREAD") == 0)||
								(stricmp(pSubTok, "TURRET") == 0)||
								(stricmp(pSubTok, "BARREL") == 0))
							{
								// Get next token
								while(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										if( (stricmp(pSubSubToken, "null") != 0) && (isalpha(pSubSubToken[0])) )
										{	
											char szBuf[256];
											sprintf(szBuf, "%s\\%s", l_dir, pSubSubToken);
											fileList.push_back(szBuf);
										}
									}
								}
							}							

						}// End line tokenizer has tokens
					}
					break;
				case MODE_SPRITES:
					{
						StringTokenizer lineTokenizer(pTok, ",");
						while(lineTokenizer.hasMoreTokens())
						{
							const char *pSubTok = lineTokenizer.nextToken();
							if(stricmp(pSubTok, "dir") == 0)
							{
								// Get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										strncpy(l_dir, pSubSubToken, 31);
									}
								}
							}else		
								if((stricmp(pSubTok, "MODEL") == 0)||
									(stricmp(pSubTok, "IMG") == 0))
								{
									// Get next token
									while(lineTokenizer.hasMoreTokens())
									{
										const char* pSubSubToken = lineTokenizer.nextToken();
										if(pSubSubToken)
										{
											if( (stricmp(pSubSubToken, "null") != 0) && (isalpha(pSubSubToken[0])) )
											{	
												char szBuf[256];
												sprintf(szBuf, "sprites\\%s\\%s", l_dir, pSubSubToken);
												fileList.push_back(szBuf);
											}
										}
									}
								}							

						}// End line tokenizer has tokens
					}
					break;
				case MODE_FLAGS:
					{
						StringTokenizer lineTokenizer(pTok, ",");
						while(lineTokenizer.hasMoreTokens())
						{
							const char *pSubTok = lineTokenizer.nextToken();
							if(stricmp(pSubTok, "dir") == 0)
							{
								// Get next token
								if(lineTokenizer.hasMoreTokens())
								{
									const char* pSubSubToken = lineTokenizer.nextToken();
									if(pSubSubToken)
									{
										strncpy(l_dir, pSubSubToken, 31);
									}
								}
							}else		
								if((stricmp(pSubTok, "MODEL") == 0)||
									(stricmp(pSubTok, "IMG") == 0))
								{
									// Get next token
									while(lineTokenizer.hasMoreTokens())
									{
										const char* pSubSubToken = lineTokenizer.nextToken();
										if(pSubSubToken)
										{
											if(stricmp(pSubSubToken, "null") != 0)
											{	
												char szBuf[256];
												sprintf(szBuf, "%s\\%s", l_dir, pSubSubToken);
												fileList.push_back(szBuf);
											}
										}
									}
								}							

						}// End line tokenizer has tokens
					}
					break;
				default:
					break;
				}
			}
			else
			{
				if(stricmp(pTok, "[levels]") == 0)
				{
					l_nMode = MODE_LEVEL;
				}else
				if(stricmp(pTok, "[vehicles]") == 0)
				{
					l_nMode = MODE_VEHICLES;
				}else
				if(stricmp(pTok, "[sprites]") == 0)
				{
					l_nMode = MODE_SPRITES;
				}else
				if(stricmp(pTok, "[flags]") == 0)
				{
					l_nMode = MODE_FLAGS;
				}
			}

		}// End while data has tokens

		fileSystem.CloseFile(pFile);
	}

	printf("done parsing...\n");
}
