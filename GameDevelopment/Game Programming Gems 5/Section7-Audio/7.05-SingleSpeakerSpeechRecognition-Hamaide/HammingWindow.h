#ifndef __HAMMING_WINDOW_H__
#define __HAMMING_WINDOW_H__

/// Apply a Hamming's window to the data
/// Pay attention, each time you change window size, a new window is computed, so try not to change it frequently
/// You must allocate memory yourself
void Hamming(float *src, float *dst, int size);

#endif //__HAMMING_WINDOW_H__