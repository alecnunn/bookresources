#ifndef __WAV_HEADER_H__
#define __WAV_HEADER_H__

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

#endif //__WAV_HEADER_H__