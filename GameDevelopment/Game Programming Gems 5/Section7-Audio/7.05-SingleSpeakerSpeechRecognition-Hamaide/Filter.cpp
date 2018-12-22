#include "filter.h"
#include <memory.h>

void Preemphasis(float *src, int size, float &LastSample)
{
	float temp;

	temp = *src;
	*src = *src - 0.9f * LastSample;

	src++;
	for(int i=1; i<size; i++)
	{
		LastSample = temp;
		temp = *src;
        *src = *src - 0.9f * LastSample;
		src++;
	}
	LastSample = temp;
}


static const float mNum[10] = 
		{0.009524405934f, 
		 0.002127919346f,
		 0.0279841125f,
		 0.01633026637f,
		 0.03211472556f,
		 0.03211472556f,  
		 0.01633026637f,   
		 0.0279841125f, 
		 0.002127919346f, 
		 0.009524405934f
};

static const float mDen[9] = {
		-4.307374954f,    
		10.72063923f,   
		-17.80602455f,    
		21.61767006f,
		-19.51434326f,    
		13.11477757f,   
		-6.331177711f,    
		2.018088818f,  
		-0.3360900879f
};

//This filter applies a low pass filter before undersampling.
void Resample(short* src, float **dst, int size, int &dstSize, bool reset)
{
	static float LastSample[10];
	static float LastOutput[9];
	static int SampleIndex;
	static int OutputIndex;

	if(reset)
	{
		SampleIndex = 0;
		OutputIndex = 0;
		memset(LastSample, 0, 10*sizeof(float));
		memset(LastOutput, 0, 9*sizeof(float));
	}

	float temp;

	dstSize = size/5+1;

	*dst = new float[size/5 + 1];

	float *dest = *dst;

	for(int i=0; i< size; i++)
	{
		temp=0;
		//now the boring stuff
		//First push the value on the buffer
		LastSample[SampleIndex] = *src;

		//compute the filter index
		for(int j=0; j < 10; j++)
		{
			temp += LastSample[ (j+SampleIndex)%10 ]*mNum[j];
		}
		for(int k=0; k < 9; k++)
		{
			temp -= LastOutput[ (k+OutputIndex)%9 ]*mDen[k];
		}

		//take only a sample every sample
		if(!(i%5))
		{
			*dest = temp;
			dest++;
		}

		//increase src and index
		src++; SampleIndex--; OutputIndex--;

		//adjust index back
		if(SampleIndex<0) SampleIndex+=10;
		if(OutputIndex<0) OutputIndex+=9;

		//Save output
		LastOutput[OutputIndex] = temp;

	}
}