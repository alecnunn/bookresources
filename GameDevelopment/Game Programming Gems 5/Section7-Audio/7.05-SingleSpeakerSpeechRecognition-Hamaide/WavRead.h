#ifndef __WAVREAD_H__
#define __WAVREAD_H__


///  Open a wav file
bool wavOpen(const char *filename, short **output, int &size);

#endif //__WAVEREAD_H__