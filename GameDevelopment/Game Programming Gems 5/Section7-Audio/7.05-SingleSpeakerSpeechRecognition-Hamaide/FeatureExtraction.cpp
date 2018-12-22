#include "FeatureExtraction.h"
#include "HammingWindow.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>


void ComputeFeaturesVectors(float *src, int size, features **dst, int &featureNumber)
{

	//We take 256 points every 100 points
	const int sliceSize = 256;
	const int slicestep = 100;
	float temp[sliceSize];

	featureNumber = (size - sliceSize + slicestep)/slicestep;

	*dst = new features[featureNumber];

	(*dst)[0]._deltaSigma = ComputeDeltaSigma(src, sliceSize, true);
	Hamming(src, temp, sliceSize);
	ComputePLP(temp,sliceSize, (*dst)[0]._lpAnalysis);

	for(int i=1; i< featureNumber; i++)
	{
		(*dst)[i]._deltaSigma = ComputeDeltaSigma(src + i*slicestep, sliceSize);
		Hamming(src+i*slicestep, temp, sliceSize);
		ComputePLP(temp, sliceSize, (*dst)[i]._lpAnalysis);
	}
}

float ComputeDeltaSigma(float *src, int size, bool reset)
{

	static float LastSigma = 0;

	if(reset) LastSigma = 0;

    //First compute the mean of the signal.
	float mean=0;
	int i;

	float *current = src;

	for(i=0; i<size; i++)
	{
		mean+=*current;
		current++;
	}

	mean/=size;


	//And now sigma
	float sigma = 0;
	float temp;

	current = src;
	for(i=0; i<size; i++)
	{
		temp = *current - mean;
		sigma += temp * temp; 
		current++;
	}

	sigma /= (size-1);

	//And finally deltaSigma
	if(LastSigma)
		temp = (sigma-LastSigma)/(LastSigma+sigma);
	else
		temp = 0;

	LastSigma = sigma;

	//just lower the dynamic
	return 0.1f*temp;
}

void ComputeLPC(float *src, int size, float* cepstre)
{
	float autocorr[14];
	float a[14];
	float k[14];
	float g;

	Autocorrelation(src, size, autocorr, 14);
	Schur(autocorr,13,k,g);
	ki2ai(k,13,a);
	ai2ci(a,13,cepstre,13);

}

void ComputePLP(float *src, int size, float* cesptre)
{
	const int Fs = 8820; //OK, this is not a good idea
	float barkbank[15];
	float loudness[15];

	float *spectrum = new float[size/2 +1];

	Spectrum(src, size,spectrum);
	Barkbank(spectrum, size/2+1,barkbank,15, Fs);
	Loudness(barkbank, 15, loudness, Fs);
	Cepstrum(loudness,15,10,cesptre,13,0.6f, Fs);

}


///Autocorrelation 
void Autocorrelation(float *src, int size, float *autocorr, int order)
{
	float *src_1, *src_2;

	for(int i=0; i< order; i++)
	{
		src_1 = src;
		src_2 = src + i;
		autocorr[i]=0;
		for(int j=0; j< size - i; j++)
		{
			autocorr[i] += *src_1 * *src_2;
			src_1++; src_2++;
		}
	}
}

int LevinsonRecursion(float *R, unsigned int order, float *A, float *K, float &g)
{
	double Am1[62];
	double Em;
	if(R[0]==0.0) { 
		for(unsigned int i=1; i<=order; i++) 
		{
			K[i]=0.0; 
			A[i]=0.0;
		}}
	else {
		double km,Em1;
		unsigned int k,s,m;
		for (k=0;k<=order;k++){
			A[0]=0;
			Am1[0]=0; }
		A[0]=1;
		Am1[0]=1;
		km=0;
		Em1=R[0];
		for (m=1;m<=order;m++)                    //m=2:N+1
		{
			double err=0.0f;                    //err = 0;
			for (k=1;k<=m-1;k++)            //for k=2:m-1
				err += Am1[k]*R[m-k];        // err = err + am1(k)*R(m-k+1);
			km = (R[m]-err)/Em1;            //km=(R(m)-err)/Em1;
			K[m-1] = -float(km);
			A[m]=(float)km;                        //am(m)=km;
			for (k=1;k<=m-1;k++)            //for k=2:m-1
				A[k]=float(Am1[k]-km*Am1[m-k]);  // am(k)=am1(k)-km*am1(m-k+1);
			Em=(1-km*km)*Em1;                //Em=(1-km*km)*Em1;
			for(s=0;s<=order;s++)                //for s=1:N+1
				Am1[s] = A[s];                // am1(s) = am(s)
			Em1 = Em;                        //Em1 = Em;
		}
	}
	g = float(Em);
	for(unsigned int i=1; i<=order; i++)
		A[i] = -A[i];
	return 0;
}


//Schur algorithm, k must be of size order +1
void Schur(float* autocorr, int order, float* k, float &g)
{

	int i;
	int j;
	float* U;
	float* V;
	float* tmp;
	float* tmpu;
	float* tmpv;
	U=new float[order+1];
	V=new float[order+1];
	if (*autocorr==0) *autocorr=0.000001f;
	tmpu=U; tmpv=V;
	for (i=0;i<order;i++) {*(tmpu++)=*(autocorr++);*(tmpv++)=*(autocorr);}
	*(tmpu)=*(autocorr);
	tmpu=U;tmpv=V;
	for (i=1;i<=order;i++)
	{
		U=tmpu; V=tmpv;
		*k=-*V / *U;
		*(U++)+=*k * *(V++);
		tmp=tmpv;
		for (j=1;j<=(order-i);j++)
		{
			*tmp=*k * *U+*V;
			*U+= *k * *V;
			U++; V++; tmp++;
		}
		k++;
	}
	
	g = tmpu[order];

	delete[] tmpu;
	delete[] tmpv;
}

void ki2ai(float *ki, int order, float *ai)
{
	int i;
	int m;
	float* aint;
	float* atmp3;
	float* atmp2;
	float* atmp;
	float tmp;
	aint=new float [order+1];
	atmp=aint; *(atmp++)=1.0;
	for (i=1;i<=order;i++) *(atmp++)=0.0;
	for (m=1;m<=order;m++)   {
		atmp=ai;
		atmp2=aint+1;
		atmp3=aint+m-1;
		*(atmp++)=1;
		tmp=ki[m-1];
		for (i=1;i<m;i++) *(atmp++)=*(atmp2++)+tmp* *(atmp3--);
		*(atmp)=tmp;
		atmp=ai+1; atmp2=aint+1;
		for (i=1;i<=order;i++) *(atmp2++)=*(atmp++);
	}
	delete[] aint;
}

void ai2ci(float* ai, int order, float* ci, int NCep)
{
	int    i, k;
	float sum;
	float *ptr1;
	float *ptr2;

	*(ci) = -*(++ai);
	for (k=2; k<=NCep; k++)
	{
		ptr1 = ai;
		ptr2 = ci;
		sum = 0.0;
		if (k < order)
		{
			for (i=1; i<k; i++)
				sum += (1-(float)i/(float)k) * (*(ptr1++)) * (*(ptr2--));
			*(++ci) = -(*ptr1) - sum;
		}
		else
		{
			for (i=1; i<order; i++)
				sum += (1-(float)i/(float)k) * (*(ptr1++)) * (*(ptr2--));
			*(++ci) = -sum;
		}
	}

}
///Spectrum computes the absolute value of the fft of src. Spectrum must have a size of size/2 + 1
void Spectrum(float *src, int size, float *spectrum)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta; 
	float tempr,tempi;


	float *data = new float[2*size];
	float *dst = data;

	memset(dst, 0, 2*size*sizeof(float));

	for(int k=0; k<size; k++)
	{
		*dst = *src;
		src++;dst+=2;
	}
	
	dst = data;
	data --;

	//FFT the signal
	n=size << 1;
	j=1;
	for (i=1;i<n;i+=2) 
	{ 
		if (j > i) 
		{
			swap(data[j],data[i]);
			swap(data[j+1],data[i+1]);
		}
		m=size;
		while (m >= 2 && j > m) 
		{
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	
	mmax=2;
	while (n > mmax)
	{
		istep=mmax << 1;
		theta=(2*M_PI)/mmax;
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) 
		{ 
			for (i=m;i<=n;i+=istep) 
			{
				j=i+mmax; 
				tempr=(float)(wr*data[j]-wi*data[j+1]);
				tempi=(float)(wr*data[j+1]+wi*data[j]);
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
			data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}

	//OK, now compute the norm

	data = dst;

	for(i=0; i<(unsigned int)(size/2 +1); i++)
	{
		*spectrum = (*dst)*(*dst)+(*(dst+1))*(*(dst+1)) +0.1f; //avoid zeros

		dst+=2; spectrum++;
	}
	delete [] data;
}

void Barkbank(float *src, int size, float *barkbank, int barksize, int Fs)
{
	float temp = (Fs/2) / 600.0f;
	float fnyq = 6 * logf( temp + sqrtf( temp*temp + 1 ) );

	float first = floorf((barksize-1)/(fnyq-2.0f) + 0.5f);
	float fstep = fnyq/(barksize+2.0f*first-1.0f);

	float fmid;
	float fstart;
	float fstop;

	int istart;
	int istop ;

	int Nfft = (size -1) *2;

	float inv_Fs = 1.0f/Fs;

	for(int i=0; i<barksize; i++)
	{
		fmid = (first + i) * fstep;
		fstart = fmid -2.5f;

		fstart = 300 *( expf(fstart/6.0f)- expf(-fstart/6.0f));

		fstop = fmid+1.3f;

		fstop = 300*( expf(fstop/6.0f) - expf(-fstop/6.0f));

		istart = (int)(Nfft * fstart*inv_Fs);
		if( istart < 0) istart = 0;
		
		istop = (int)(Nfft * fstop*inv_Fs);
		if(istop> (Nfft/2)) istop = Nfft/2;

		float delta;
		float ftmp;
		float inv_Nfft =  1.0f/Nfft;
		barkbank[i] = 0;
		for(int index = istart; index <=istop; index ++)
		{
            ftmp = index *Fs*inv_Nfft;
			ftmp = ftmp/600.0f;
			ftmp = 6*logf(ftmp + sqrtf(ftmp*ftmp + 1));
			delta = ftmp - fmid;
			if(delta < -0.5f) 
				delta = 1.0f * (delta + 0.5f);
			else if (delta >= 0.5)
				delta = -2.5f * (delta - 0.5f);
			else
				delta = 0;

			barkbank[i] += powf(10.0f,delta) * src[index];
		}
	}
}

void Loudness(float *barkbank, int barksize, float *loudness, int Fs)
{
	float temp = (Fs/2) / 600.0f;
	float fnyq = 6 * logf( temp + sqrtf( temp*temp + 1 ) );

	float first = floorf((barksize-1)/(fnyq-2.0f) + 0.5f);
	float fstep = fnyq/(barksize+2.0f*first-1.0f);

	float fmid;
	float L;

	for(int index = 0; index < barksize; index ++)
	{
		fmid = (index  + first)* fstep;

		fmid = 300.0f * (expf(fmid/6.0f) - expf(-fmid/6.0f));
		
		fmid *= fmid;

		L = ( fmid*fmid * (fmid + 1.44e6f))/( (fmid + 1.65e5f)*(fmid+1.65e5f) * (fmid + 9.61e6f) );

		loudness[index] = powf(L*barkbank[index], 1/3.0f);
	}

}

void Cepstrum(float *barkbank, int barksize, int LPCOrder, float *cepstrum, int cepstrumsize, float LifteringFactor, int Fs)
{
	int i;
	float temp = (Fs/2) / 600.0f;
	float fnyq = 6 * logf( temp + sqrtf( temp*temp + 1 ) );

	int first = int(floorf((barksize-1)/(fnyq-2.0f) + 0.5f));

	//IDFT : power spectrum -> autocorrelation
	int Ne = barksize + 2*first;

	float *R = new float[LPCOrder + 1];
	float W;

	float inv_NE_1 = 1.0f/(Ne-1);
	for(i = 0; i<= LPCOrder; i++)
	{
		R[i] = 0;
        
		for(int j = first; j< Ne-first; j++)
		{
			W = cosf(i * j * float(M_PI) * inv_NE_1);
			if( j >= 1) W *=2;

			R[i]+= W*barkbank[j-first];
		}

		R[i] /= 2*barksize;
	}

	//Ok, we have the autocorrellation, now, LPC it
	float g;
	float *k = new float[LPCOrder+1];
	float *a = new float[LPCOrder+1];

	LevinsonRecursion(R, LPCOrder,a,k,g);
	

	//Transform LPC coeff to cepstral coeff.

	cepstrum[0] = logf(g);
	cepstrum[1] = -a[1];
	for (i=1; i<LPCOrder; i++)
	{
        cepstrum[i+1] = -a[i+1];
		for(int j= 0; j<=i-1; j++)
		{
			cepstrum[i+1] -= (j+1)/float(i+1) * cepstrum[j+1] * a[i-j];
		}
	}

	for(i = LPCOrder+1; i<= cepstrumsize-1; i++)
	{
		cepstrum[i] = 0;
		for(int j=LPCOrder; j >0; j--)
		{
			cepstrum[i] -= (i-j)/float(i) * cepstrum[i-j] * a[j];
		}
	}

	delete[] k;
	delete[] a;


	//Liftering

	if( LifteringFactor != 0)
	{
		for(int i =1; i< cepstrumsize; i++)
		{
			cepstrum[i] *=(powf(float(i),LifteringFactor));
		}

	}
    

}
