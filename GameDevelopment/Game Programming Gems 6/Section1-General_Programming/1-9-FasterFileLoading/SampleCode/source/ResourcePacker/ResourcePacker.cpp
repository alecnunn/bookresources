// ResourcePacker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "resourcefile.h"
#include "fileinterface.h"
#include <vector>
#include <algorithm>
typedef std::vector<ResourceFileEntry> FileEntryTable;
ResourceFileHeader fileHeader;
ResourceFileStringTable fileStringTable;

int g_nFilecount = 0;
int g_nDircount = 0;
int g_nFilesize = 0;
FileEntryTable fileTable;

#ifdef _MSC_VER
void SearchDir(const char* szDir);
void CheckFile(const char* szDir, _finddata_t &file);
#endif

#ifdef __GNUC__
#include <dirent.h>
#include<sys/stat.h>
void SearchDir(const char* szDir);
void CheckFile(const char* szDir, const char* szFile);
#endif

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("\nusage: ResourcePacker <directory> <filename> [packing order file]\n");
		return 1;
	}

	fileHeader.nVersion = 1;
	g_nFilecount = 0;
	g_nDircount = 0;
	g_nFilesize = 0;

	printf("Resource Packer:\n\n");
	
	SearchDir(argv[1]);

	// This is not the most optimal function, but it does the trick.
	if(argc == 4) //Do we have a packing order file?
	{
		//open the file
		FILE* fp = fopen(argv[3], "r");
		if(fp)
		{
			ResourceFileStringTable local_fileStringTable;
			FileEntryTable local_fileTable;

			char line[128];
			FileInterface fileInterface;
			for(int i = 0; fgets(line, 127, fp) != NULL; ++i)
			{
				int nLen = (int)strlen(line);
				if(line[nLen-1] == '\n')
				{
					line[nLen-1] = '\0';
				}

				FileStats stats;
				if (fileInterface.StatFile(line, &stats))
				{				
					local_fileStringTable.push_back(line);
					ResourceFileEntry file_entry;
					file_entry.nStringTableIndex = (uint16)(local_fileStringTable.size() - 1);
					file_entry.nFileSize = stats.nSize;
					file_entry.nFlags = 0;
					file_entry.nCompressedFileSize = stats.nSize;
					local_fileTable.push_back(file_entry);
				}
				else
				{
					printf("Error: Can not stat file (%s)!\n", line);
				}

			}
			fclose(fp);

			//Now that we've created an ordered list, push the rest of the file entries on.
			int nStringTableSize = (int)fileStringTable.size();
			for(int j = 0; j < nStringTableSize; ++j)
			{
				ResourceFileStringTable::iterator iter = local_fileStringTable.begin();

				// Is this entry already in our new list?
				iter = find(local_fileStringTable.begin(), local_fileStringTable.end(), fileStringTable[j]);
				
				if(iter == local_fileStringTable.end())
				{
					// It wasn't found, so let's push it on
					local_fileStringTable.push_back(fileStringTable[j]);
					ResourceFileEntry file_entry;
					file_entry.nStringTableIndex = (uint16)(local_fileStringTable.size() - 1);
					file_entry.nFileSize = fileTable[j].nFileSize;
					file_entry.nFlags = 0;
					file_entry.nCompressedFileSize = fileTable[j].nFileSize;
					local_fileTable.push_back(file_entry);
				}

			}

			// Clear the old tables
			fileStringTable.clear();
			fileTable.clear();

			// Move our values over
			ResourceFileStringTable::iterator iter1 = local_fileStringTable.begin();
			while(iter1 != local_fileStringTable.end())
			{
				fileStringTable.push_back(*iter1);
				++iter1;
			}
 
			FileEntryTable::iterator iter2 = local_fileTable.begin();
			while(iter2 != local_fileTable.end())
			{
				fileTable.push_back(*iter2);
				++iter2;
			}
		}		
	}

	fileHeader.nNumFiles = (int)fileTable.size();

	int nFilePointerStart = sizeof(ResourceFileHeader) + (sizeof(ResourceFileEntry) *fileHeader.nNumFiles);
	int nFilePointerCur = nFilePointerStart;

	FileInterface fileInterface;
	BaseFile* pFile = fileInterface.OpenFile(argv[2], BINARY_FILE, WRITEONLY);

	// Write the file header
	pFile->Write(&fileHeader, sizeof(ResourceFileHeader));

	//Grab the resource directory name length plus the '\' or '/'
	int nResDirNameLen = (int)strlen(argv[1]) + 1;

	// Write out the string table
	FileEntryTable::iterator iter = fileTable.begin();
	while(iter != fileTable.end())
	{
		const char* pszString = fileStringTable[(*iter).nStringTableIndex].c_str();

		// Chop off the resource directory name len.
		int nStringLen = (int)strlen(pszString) - nResDirNameLen;
		pFile->Write(&nStringLen, sizeof(uint16));

		// Chop off the resource directory name.
		pFile->Write((void*)&pszString[nResDirNameLen], nStringLen);
		nFilePointerCur += nStringLen + sizeof(uint16);
		++iter;
	}	

	// Setup the file pointers
	iter = fileTable.begin();
	while(iter != fileTable.end())
	{
		(*iter).nFilepointer = nFilePointerCur;
		nFilePointerCur += (*iter).nFileSize;
		++iter;
	}

	// Write the file entries.
	pFile->Write(&fileTable[0], (sizeof(ResourceFileEntry) *fileHeader.nNumFiles));

	// Now add the file entry data to the resource file.
	iter = fileTable.begin();
	while(iter != fileTable.end())
	{
		BaseFile* pFileEntry = fileInterface.OpenFile(fileStringTable[(*iter).nStringTableIndex].c_str(), BINARY_FILE, READONLY);
		if(NULL != pFileEntry)
		{

			printf("Adding File: %s, Size: %d\n", fileStringTable[(*iter).nStringTableIndex].c_str(), (*iter).nFileSize);

			// Create the data buffer.
			uint8* pBytes = new uint8[(*iter).nFileSize];
			
			// Read in the file contents into our buffer.
			pFileEntry->Read(pBytes, (*iter).nFileSize);

			// Write it out to the resource file.
			pFile->Write(pBytes, (*iter).nFileSize);

			//Delete the data buffer.
			delete[] pBytes;

			//Close the file.
			fileInterface.CloseFile(pFileEntry);
		}
		++iter;
	}

	fileInterface.CloseFile(pFile);

	printf("  Number of directories found: %d\n", g_nDircount);
	printf("  Number of files found: %d\n", g_nFilecount);
	printf("  Total file size: %d (%dK)\n", g_nFilesize, g_nFilesize/1024);

	return 0;
}

#ifdef _MSC_VER
void SearchDir(const char* szDir)
{
	struct _finddata_t file;
	intptr_t hFile;

	char fulldirpath[4096];

	sprintf(fulldirpath, "%s\\*.*", szDir);


	if( (hFile = _findfirst( fulldirpath, &file )) == -1L )
	{
		printf( "ERROR: No files in current directory! (%s)\n" , fulldirpath);
	}
	else
	{

		CheckFile(szDir, file);
		/* Find the rest of the files */

		while( _findnext( hFile, &file ) == 0 )
		{
			CheckFile(szDir, file);
		}
		_findclose( hFile );
	}		
}

void CheckFile(const char* szDir, _finddata_t &file)
{

	if(file.attrib & _A_SUBDIR)
	{
		if(stricmp(file.name, ".") == 0)
		{
			return;
		}
		if(stricmp(file.name, "..") == 0)
		{
			return;
		}
		++g_nDircount;

		char szDir2[512];
		sprintf(szDir2,"%s\\%s",szDir,file.name);

		// if the file name is appended with ".\" then cut that off.
		// otherwise the hashed value will differ at load time.
		SearchDir((szDir2[0] == '.') ? (char*)&szDir2[2] : szDir2);
	}
	else
	{
		char szFullPath[512];
		sprintf(szFullPath,"%s\\%s",szDir,file.name);

		ResourceFileEntry file_entry;
		fileStringTable.push_back(szFullPath);
		file_entry.nStringTableIndex = (uint16)(fileStringTable.size() - 1);
		file_entry.nFileSize = file.size;
		file_entry.nFlags = 0;
		file_entry.nCompressedFileSize = file.size;
		fileTable.push_back(file_entry);

		g_nFilesize += file.size;
		++g_nFilecount;		
	}

}
#endif

#ifdef __GNUC__
void SearchDir(const char* szDir)
{
  DIR *dirp;
  struct dirent *dp;

  if ((dirp = opendir(szDir)) == NULL)
  {
	printf("Error! Can not open (%s)\n", szDir);
	return;
  }

  dp = readdir(dirp);
  while( dp != NULL )
  {  
	  CheckFile(szDir, dp->d_name);
	  dp = readdir(dirp);
  }

  closedir(dirp);
}

void CheckFile(const char* szDir, const char* szFile)
{
	char szFullPath[1024];
	sprintf(szFullPath, "%s/%s", szDir, szFile);
	struct stat s;
	if (stat(szFullPath, &s) == -1)
	{
		return;
	}
	
	if(strcmp(szFile, ".") == 0)
	{
		return;
	}
	if(strcmp(szFile, "..") == 0)
	{
		return;
	}
	
	if ((s.st_mode & S_IFMT) == S_IFDIR)
	{
		++g_nDircount;
		SearchDir(szFullPath);
	}
	else
	{
		ResourceFileEntry file_entry;
		fileStringTable.push_back(szFullPath);
		file_entry.nStringTableIndex = (uint16)(fileStringTable.size() - 1);
		file_entry.nFileSize = s.st_size;
		file_entry.nFlags = 0;
		file_entry.nCompressedFileSize = s.st_size;
		fileTable.push_back(file_entry);
		g_nFilesize += s.st_size;
		++g_nFilecount;			
	}
}
#endif

