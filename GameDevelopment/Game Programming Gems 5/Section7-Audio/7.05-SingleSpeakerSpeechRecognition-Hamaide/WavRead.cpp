#include "wavread.h"

#include <fstream>
using namespace std;


//WAV header
#pragma pack(push,2)
struct _header
{
	char _chunkID[4];
	int _chunkSize;
	char _format[4];

	//fmt chunk
	char _fmt[4];
	int _fmtSize;
	short _fmtFormat;
	short _numChannel;
	unsigned int _sampleRate;
	unsigned int _byteRate;
	unsigned short _blockAlign;
	unsigned short _bitsPerSample;
	char _data[4];
	unsigned int _dataSize;
};
#pragma pack(pop)

///  We are not reading other file than 16 bit, 44100Hz, 1 channel PCM file.
///  @param IN : filename name of the wav file to open
///  @param OUT : output pointer of pointer to int, allocate inside with new[], so delete it with delete[]
///  @param OUT : size of the buffer (in samples)
///  @return true if successed, false if failed
bool wavOpen(const char *filename, short **output, int &size)
{
	fstream wavFile;
	_header header;

	wavFile.open(filename, ios_base::in | ios_base::binary );

	wavFile.read((char*)&header, sizeof(_header));

	//test the format
	if(memcmp(header._chunkID,"RIFF",4))
	{
		return false;
	}
	if(memcmp(header._format, "WAVE",4))
	{
		return false;
	}

	if(memcmp(header._fmt, "fmt ",4))
	{
		return false;
	}

	if(header._sampleRate != 44100)
	{
		return false;
	}
	if(header._bitsPerSample != 16)
	{
		return false;
	}

	if(header._numChannel != 1)
	{
		return false;
	}

	//Is this PCM
	if(header._fmtFormat != 1)
	{
		return false;
	}

	if (memcmp(header._data,"data",4))
	{
		return false;
	}

	//Now, we allocate enough memory to read the real data
	*output =(short*) new char[header._dataSize];

	//Read all the stuff

	wavFile.read((char*)*output, header._dataSize);
	size = header._dataSize>>1;

	return true;
}