#ifndef __FEATURE_EXTRACTION_H__
#define __FEATURE_EXTRACTION_H__

///This structure will contain features for a frame
typedef struct struct_features
{
	float _deltaSigma;
	float _lpAnalysis[13];
} features;

/// Compute Features Vectors for the whole signal
/// @param IN : src signal undersampled and preemphased
/// @param IN : size of the signal
/// @param OUT : features pointer to pointer of features? Will be allocated with new[], must be freed by free[]
/// @param OUT : featureNumber number of feature returned in features
void ComputeFeaturesVectors(float *src, int size, features **dst, int &featureNumber);

//Compute features from one frame
float ComputeDeltaSigma(float *src, int size, bool reset=false);
void ComputeLPC(float *src, int size, float* cesptre);
void ComputePLP(float *src, int size, float* cesptre);

//LPC tools
void Autocorrelation(float *src, int size, float *autocorr, int order);
void Schur(float* autocorr, int order, float* k, float &g);
void ki2ai(float *ki, int order, float *ai);
void ai2ci(float* ai, int order, float* ci, int NCep);


//PLP Tools
int LevinsonRecursion(unsigned int P, float *R, float *A, float *K, float &g);
void Spectrum(float *src, int size, float *spectrum);
void Barkbank(float *src, int size, float *barkbank, int barksize, int Fs);
void Loudness(float *barkbank, int barksize, float *loudness, int Fs);
void Cepstrum(float *barkbank, int barksize, int LPCOrder, float *cepstrum, int cepstrumsize, float LifteringFactor, int Fs);

template<typename T> inline void swap(T &a, T &b){T temp; temp = a; a=b; b=temp;}

#endif // __FEATURE_EXTRACTION_H__