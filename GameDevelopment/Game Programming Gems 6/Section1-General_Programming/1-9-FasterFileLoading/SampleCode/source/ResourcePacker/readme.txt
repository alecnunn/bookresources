file format

HEADER
struct ResourceFileHeader
{
	uint32 nVersion;
	uint32 nNumFiles;
};

------------------------
String table
short
char*
...

------------------------
struct ResourceFileEntry
{
	//char	szFilename[ResourceFileFilenameMaxLen];
	uint16	nStringTableIndex;
	uint32	nFlags;
	uint32	nFileSize;
	uint32	nCompressedFileSize;
	uint32	nFilepointer;
};

------------------------
file data
...