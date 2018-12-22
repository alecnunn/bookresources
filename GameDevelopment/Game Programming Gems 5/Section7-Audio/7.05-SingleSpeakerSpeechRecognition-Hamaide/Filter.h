#ifndef __FILTER_H__
#define __FILTER_H__


///Simple filter, just apply y(n)=x(n)-0.9*y(n);
///For memory purpose, just save the output in the input
///
///@param src INOUT : data buffer
///@param size IN : size of the buffer (in samples)
///@param LastSample INOUT : Memory, for each new signal set it to zero, otherwise, if you filter samples by bloc, give the last output value
void Preemphasis(float *src, int size, float &LastSample);

///Resampling filter, convert 44100Hz to 8820Hz
///
///@param src IN : samples given in int32
///@param dst OUT : pointer to data pointer, allocate with new[], so delete it with delete[]
///@param size IN : Size of the input. NOT EQUAL TO SIZE OF THE OUTPUT
///@param dstSize IN : Size of the output buffer
///@param reset IN : Set it to true every time a new signal is passed. It allows you to filter the signal in small pieces
///
void Resample(short *src, float **dst, int size, int &dstSize, bool reset=false);

#endif //__FILTER_H__