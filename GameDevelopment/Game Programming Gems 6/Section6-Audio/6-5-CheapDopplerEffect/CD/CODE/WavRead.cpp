#include "WavRead.h"
#include "WavHeader.h"

#include <fstream>
using namespace std;


///  We are not reading other file than 16 bit, 44100Hz, 1 channel PCM file.
///  @param IN : filename name of the wav file to open
///  @return true if successed, false if failed
bool CWavReader::Open(const char *filename)
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
	m_Buffer =(short*) new char[header._dataSize];

	//Read all the stuff

	wavFile.read((char*)m_Buffer, header._dataSize);
	m_BufferSize = header._dataSize>>1;

	return true;
}