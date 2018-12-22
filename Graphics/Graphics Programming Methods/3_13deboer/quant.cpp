#include "precomp.h"
//
//
//
#include "quant.h"

const Quant::Vector Quant::Vector::RV_ZERO = Quant::Vector(0.f, 0.f, 0.f, 0.f);
const Quant::Vector Quant::Vector::RV_ZERO_HOMOGENEOUS = Quant::Vector(0.f, 0.f, 0.f, 1.f);

const float Quant::Vector::AlphaPriority = 0.5f;

// constructor
Quant::Quant() :
	mBook		(NULL),
	mBookSize	(0),
	mData		(NULL),
	mLen		(0),
	mWidth		(0),
	mHeight		(0)
{
}


// destructor
Quant::~Quant()
{
	if (mBook != NULL)
		delete [] mBook;
	mLen = 0;
	mData = NULL;
}


//
//
//
void Quant::InitCodebook(int aBooksize)
{
	if (mBook != NULL)
		delete [] mBook;

	mBook = new bookEntry[aBooksize];
	mBookSize = aBooksize;

	for (int i = 0; i < mBookSize; i++)
	{
		// Initialise weight to white, and full opacity
		mBook[i].weight = Vector(1.f, 1.f, 1.f, 1.f);

		mBook[i].accWeight = Vector::RV_ZERO;
		mBook[i].numEntries = 0;
		mBook[i].deviation = 0.f;
		mBook[i].popularity = 0;
		mBook[i].mostDistant = Vector::RV_ZERO;
	}
}


//
// Set source-image
//
void Quant::SetImage(const float* aData, int aWidth, int aHeight)
{
	assert(mBookSize > 0);
	assert(mBook != NULL);
	assert(aWidth > 0 && aHeight > 0);

	mData = aData;
	mWidth = aWidth;
	mHeight = aHeight;
	mLen = aWidth*aHeight * 4;
}


//
// Generate code book
//
void Quant::GenerateBook()
{
	assert(mBook != NULL);
	assert(mData != NULL);
	assert(mBookSize > 0);
	assert(mLen > 0);
	assert(mWidth > 0 && mHeight > 0);

	int i = 0, j;
	float percentagechange = 0.f;
	float learningrate = 1.f;
	int cycles = 0;

	bool finished = false;
	// We only ever want to do a maximum number of [MAX_CYCLES] cycles
	while (i < MAX_CYCLES && (!finished))
	{
		// Accumulate weights
		const float* src = mData;
		{
			while (src < (mData+mLen))
			{
				float dev;
				register int closest = FindClosestBookEntry(dev, src[0], src[1], src[2], src[3]);
				mBook[closest].accWeight += Vector(src[0], src[1], src[2], src[3]);
				mBook[closest].numEntries++;

				// Get maximum deviation for weight-vector so far
				if (mBook[closest].deviation < dev)
				{
					mBook[closest].deviation = dev;
					mBook[closest].mostDistant = Vector(src[0], src[1], src[2], src[3]);
				}

				src += 4;//mBytesPerPixel;

				// TODO: Increment [src] in other steps, depending on quality we want
			}
		}

		// Make [numEntries] equal to the popularity of a weight-vector
		for (j = 0; j < mBookSize; j++)
		{
			mBook[j].popularity = mBook[j].numEntries;
		}

		unsigned int numvecchange = 0;

		// Now let's adjust our code book weight vectors
		for (j = 0; j < mBookSize; j++)
		{
			// We only want to adjust weight vectors that had vectors assigned to them this iteration
			if (mBook[j].numEntries > 0)
			{
				Vector oldweight = mBook[j].weight;
				// Update weight
				mBook[j].weight = mBook[j].accWeight / (float)mBook[j].numEntries;

				// Let's see if this weight vector's changed much
				if (!IsVeryClose(mBook[j].weight, oldweight))
				{
					// It has changed much enough
					numvecchange++;
				}
			}
		}

		// Split all vectors that didn't get any vectors assigned to them this iteration
		// Apparently they weren't good enough..
		for (j = 0; j < mBookSize; j++)
		{
			if (mBook[j].numEntries == 0)
			{
				int splitvec;

				// Split
				// 
				if (((int)UNSIGNED_frand(439043.f)) & 1)
					// Use the weight-vector that is most popular, and split it
					splitvec = FindMostPopularVector();
				else
					// Split the weight-vector with the maximum variance
					splitvec = FindHighestDeviationVector();

				assert(mBook[splitvec].numEntries > 0);

				mBook[j].weight = Vector::SplitWeight(mBook[splitvec].weight, learningrate);
				numvecchange++;
			}
		}

		// Accumulate percentage of vectors changed this iteration
		percentagechange += (float)numvecchange / (float)mBookSize * 100.f;
		cycles++;

		// Find a random number between 3 and 7
		int rnd = (int)unsigned_frand(4) + 3;
		if (cycles > rnd)
		{
			// Get average percentage of change over N iterations
			percentagechange /= (float)cycles;

			// Let's just cancel updating codebook when less than 1 percent of
			// vectors got updated over the last N iterations
			if (percentagechange < 1.f)
				finished = true;

			cycles = 0;
			percentagechange = 0.f;
		}


		// Reset accumulated vectors for next iteration
		for (j = 0; j < mBookSize; j++)
		{
			mBook[j].accWeight = Vector::RV_ZERO;
			mBook[j].mostDistant = Vector::RV_ZERO;
			mBook[j].numEntries = mBook[j].popularity = 0;
			mBook[j].deviation = 0.f;
		}

		i++;

		// Scale down severity of split changes with every iteration
		learningrate *= 0.9f;	// Just take off 10%
	}
}


//
//
//
Quant::Vector Quant::Vector::CreateErrorVector(const Vector& one, const Vector& two)
{
	// Note that this vector can have negative components!
	Vector error = Vector((one.x - two.x) * 0.299f,
						  (one.y - two.y) * 0.587f,
						  (one.z - two.z) * 0.114f,
						  (one.w - two.w));
	return error;
}


#define CLAMP(x, min, max)	(x) = ((x) > max ? max : ((x) < min ? min : (x)))

//
//
//
void Quant::SpreadError(const Vector& error, int x, int y, bool alphadithering)
{
	assert(mWidth); assert(mHeight);

	int pitch = mWidth * 4;
	int offset = (y * mWidth + x) * 4;
	float* pixel = const_cast<float*>(mData);
	pixel += offset;

	if (x < (mWidth-1))
	{
		// Pixel directly to the right (7/16)
		pixel[4+0] += (7.f/16.f) * error.x;
		pixel[4+1] += (7.f/16.f) * error.y;
		pixel[4+2] += (7.f/16.f) * error.z;
		if (alphadithering)
			pixel[4+3] += (7.f/16.f) * error.w;

		if (y < (mHeight-1))
		{
			// Pixel directly below and one to the right (1/16)
			pixel[pitch+4+0] += error.x - ( ((7.f/16.f)*error.x)+((3.f/16.f)*error.x)+((5.f/16.f)*error.x) );
			pixel[pitch+4+1] += error.y - ( ((7.f/16.f)*error.y)+((3.f/16.f)*error.y)+((5.f/16.f)*error.y) );
			pixel[pitch+4+2] += error.z - ( ((7.f/16.f)*error.z)+((3.f/16.f)*error.z)+((5.f/16.f)*error.z) );
			if (alphadithering)
				pixel[pitch+4+3] += error.w - ( ((7.f/16.f)*error.w)+((3.f/16.f)*error.w)+((5.f/16.f)*error.w) );
		}
	}

	if (y < (mHeight-1))
	{
		// Pixel directly below (5/16)
		pixel[pitch+0] += (5.f/16.f) * error.x;
		pixel[pitch+1] += (5.f/16.f) * error.y;
		pixel[pitch+2] += (5.f/16.f) * error.z;
		if (alphadithering)
			pixel[pitch+3] += (5.f/16.f) * error.w;

		if (x > 0)
		{
			// Pixel directly below and one to the left (3/16)
			pixel[pitch-4+0] += (3.f/16.f) * error.x;
			pixel[pitch-4+1] += (3.f/16.f) * error.y;
			pixel[pitch-4+2] += (3.f/16.f) * error.z;
			if (alphadithering)
				pixel[pitch-4+3] += (3.f/16.f) * error.w;
		}
	}
}


//
// Dump 8-bit indexed image
//
void Quant::DumpIndexedImage(unsigned char* aImg, bool dithering /*=false*/, bool alphadithering /*=false*/)
{
	assert(mBook != NULL);
	assert(mData != NULL);
	assert(mBookSize > 0);
	assert(mLen > 0);

	// Accumulate weights
	float* src = const_cast<float*>(mData);
	unsigned char* dst = aImg;
	int x = 0, y = 0;

	while (src < (mData+mLen))
	{
		float dev;
		CLAMP(src[0], 0.f, 1.f); 
		CLAMP(src[1], 0.f, 1.f); 
		CLAMP(src[2], 0.f, 1.f); 
		CLAMP(src[3], 0.f, 1.f);
		
		// Find book entry that is perceptually closest to current source vector
		register int closest = FindClosestBookEntry(dev, src[0], src[1], src[2], src[3]);
		*dst = static_cast<unsigned char>(closest);

		if (dithering)
		{
			// Calculate perceptual error
			Vector error = Vector::CreateErrorVector(Vector(src[0], src[1], src[2], src[3]),
															mBook[closest].weight);

			// Diffuse error to neighbouring pixels
			SpreadError(error, x, y, alphadithering);
		}

		src += 4;
		dst++;

		x++;
		if (x == mWidth)
		{
			x = 0; y++;
		}
	}
}


//
//
//
void Quant::DumpCodebook(float* aBook)
{
	for (int i = 0; i < mBookSize; i++)
	{
		aBook[i*4+0] = mBook[i].weight.x;
		aBook[i*4+1] = mBook[i].weight.y;
		aBook[i*4+2] = mBook[i].weight.z;
		aBook[i*4+3] = mBook[i].weight.w;
	}
}



//
// Find the weight vector that has the most vectors assigned to it
//
int Quant::FindMostPopularVector() const
{
	int biggest = 0;

	for (int i = 1; i < mBookSize; i++)
	{
		if (mBook[i].popularity > mBook[biggest].popularity)
		{
			biggest = i;
		}
	}

	return biggest;
}


//
// Find the weight vector that has the largest deviation. Deviation in this context
// means maximum variance.
//
int Quant::FindHighestDeviationVector() const
{
	int highest = 0;

	for (int i = 1; i < mBookSize; i++)
	{
		if (mBook[i].deviation > mBook[highest].deviation)
			highest = i;
	}

	return highest;
}

//
// Find entry in book that is closest to vector (aX, aY, aZ)
//
int Quant::FindClosestBookEntry(float& dev, float aX, float aY, float aZ, float aW /*= 1.f*/) const
{
	// Distance is luminance (factors from RGB->YIQ colour space transformation). 
	// Gives for a perceptually far more pleasing result if I may say so.

	Vector vec = Vector(aX, aY, aZ, aW);

	register int winner = 0, oldwinner = 0;
	register float dist = mBook[0].weight.PerceptualDist(aX, aY, aZ, aW);
	register float newdist;

	for (int i = 1; i < mBookSize; i++)
	{
		float newdist = mBook[i].weight.PerceptualDist(aX, aY, aZ, aW);

		if (dist > newdist)
		{
			dist = newdist;
			oldwinner = winner;
			winner = i;
		}
	}

	dev = dist;
	return winner;
}


#pragma optimize( "", off )


//
//
//
Quant::Vector Quant::Vector::SplitWeight(const Quant::Vector& splitweight, float scale)
{
    Vector rnd;

    rnd.x = splitweight.x + signed_frand(scale);
    CLAMP(rnd.x, 0.f, 1.f);

    rnd.y = splitweight.y + signed_frand(scale);
    CLAMP(rnd.y, 0.f, 1.f);

    rnd.z = splitweight.z + signed_frand(scale);
    CLAMP(rnd.z, 0.f, 1.f);

    rnd.w = splitweight.w;

    return rnd;
}


#pragma optimize( "", on )



//
// End.