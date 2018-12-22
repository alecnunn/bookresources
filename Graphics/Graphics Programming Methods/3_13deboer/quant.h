#ifndef __Quant_h__
#define __Quant_h__

/*-----------------------------------------------------------------------------
	Generalised Lloyd Codebook generator

	Simple class. Does not support quantisation of multiple images.

	Steps to take:

		InitCodebook();
		SetImage(img, width, height);
		GenerateBook();
		DumpIndexedImage(dest, true, true);
		DumpCodebook(book);
-----------------------------------------------------------------------------*/
class Quant
{
public:

	enum
	{
		// Maximum number of cycles we will ever perform
		MAX_CYCLES = 100
	};

	struct Vector
	{
		Vector()													: x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		Vector(float aX, float aY, float aZ, float aW = 1.f)		: x(aX), y(aY), z(aZ), w(aW) {}
		Vector(const Vector& aRef)									: x(aRef.x), y(aRef.y), z(aRef.z), w(aRef.w) {}
		~Vector()												{}

		static Vector CreateErrorVector(const Vector& one, const Vector& two);
		static Vector SplitWeight(const Vector& splitweight, float scale);

		float		Dist(const Vector& aRef) const;
		float		Dist(float aX, float aY, float aZ, float aW = 1.f) const;
		float		SquareDist(const Vector& aRef) const;
		float		SquareDist(float aX, float aY, float aZ, float aW = 1.f) const;
		float		PerceptualDist(float aX, float aY, float aZ, float aW = 1.f) const;

		const Vector&	operator+=(const Vector& aRef);
		const Vector&	operator-=(const Vector& aRef);
		const Vector&	operator/=(float aVal);
		Vector			operator*(float aVal) const;
		Vector			operator/(float aVal) const;
		Vector			operator+(const Vector& aRef) const;
		Vector			operator-(const Vector& aRef) const;
		Vector			Interpolated(const Vector& aRef, float frac) const;

		float		Length() const;
		float		Dot(const Vector& aRef) const;

		static const Vector RV_ZERO;
		static const Vector RV_ZERO_HOMOGENEOUS;

		// Red, Green, Blue, Alpha
		float x, y, z, w;

		const static float AlphaPriority;
	};

	// Code book entry
	struct bookEntry
	{
		bookEntry()													: numEntries(0), deviation(0.f), popularity(0)	{}
		~bookEntry()												{}

		// current weight vector
		Vector	weight;
		// accumulated source vectors in this iteration
		Vector	accWeight;
		// number of source vectors assigned to weight
		int			numEntries;

		int			popularity;

		// this is the maximum variance
		float		deviation;
		Vector	mostDistant;
	};

	// construction/destruction
	Quant();
	~Quant();

	// Initialise codebook, set its size (256 for 8bits, 16 for 4bits, etc)
	void		InitCodebook(int aBooksize);

	// Set RGBA 4xfloat source image range [0.f, 1.f]. If we want to quantise a 3xfloat image (ie, 24-bit image)
	// set all its alpha components to 1.f.
	void		SetImage(const float* aData, int aWidth, int aHeight);

	// Generate codebook from source image
	void		GenerateBook();

	// Write out 8bit indexed image to [aImg]
	// NOTE: The [alphadithering] parameter is only considered when [dithering] is true
	void		DumpIndexedImage(unsigned char* aImg, bool dithering = false, bool alphadithering = false);
	// Write XYZW code book to [aBook]
	void		DumpCodebook(float* aBook);

	// Range [0,1]. 0=No Priority, 1=Full Priority.
	static float GetAlphaPriority()											   { return Vector::AlphaPriority; }

private:

	// Private functions
	bool		IsVeryClose(const Vector& aLhs, const Vector& aRhs) const;
	int			FindMostPopularVector() const;
	int			FindHighestDeviationVector() const;
	int			FindClosestBookEntry(float& dev, float aX, float aY, float aZ, float aW = 1.f) const;

	void		SpreadError(const Vector& error, int x, int y, bool alphadithering);

	// private data members
	bookEntry*	 mBook;
	int			 mBookSize;
	const float* mData;
	int			 mWidth, mHeight;
	int			 mLen;
};


/*-----------------------------------------------------------------------------
	Generalised Lloyd Vector inline implementation
-----------------------------------------------------------------------------*/

inline const Quant::Vector& Quant::Vector::operator+=(const Vector& aRef)
{
	x += aRef.x;
	y += aRef.y;
	z += aRef.z;
	w += aRef.w;

	return *this;
}

inline const Quant::Vector& Quant::Vector::operator-=(const Vector& aRef)
{
	x -= aRef.x;
	y -= aRef.y;
	z -= aRef.z;
	w -= aRef.w;

	return *this;
}

inline const Quant::Vector& Quant::Vector::operator/=(float aVal)
{
	register float frac = 1.f/aVal;
	x *= frac; y *= frac; z *= frac; w *= frac;
	return *this;
}

inline Quant::Vector Quant::Vector::operator*(float aVal) const
{
	Vector tmp;
	tmp.x = x * aVal; tmp.y = y * aVal; tmp.z = z * aVal; tmp.w = w * aVal;
	return tmp;
}

inline Quant::Vector Quant::Vector::operator/(float aVal) const
{
	Vector tmp;
	aVal = 1.f/aVal;
	tmp.x = x * aVal; tmp.y = y * aVal; tmp.z = z * aVal; tmp.w = w * aVal;
	return tmp;
}

inline Quant::Vector Quant::Vector::operator+(const Quant::Vector& aRef) const
{
	Vector tmp;
	tmp.x = x + aRef.x; tmp.y = y + aRef.y; tmp.z = z + aRef.z; tmp.w = w + aRef.w;
	return tmp;
}

inline Quant::Vector Quant::Vector::operator-(const Quant::Vector& aRef) const
{
	Vector tmp;
	tmp.x = x - aRef.x; tmp.y = y - aRef.y; tmp.z = z - aRef.z; tmp.w = w - aRef.w;
	return tmp;
}

inline Quant::Vector Quant::Vector::Interpolated(const Quant::Vector& aRef, float frac) const
{
	Vector tmp;
	tmp.x = x*(1.f-frac) + aRef.x*frac;
	tmp.y = y*(1.f-frac) + aRef.y*frac;
	tmp.y = z*(1.f-frac) + aRef.z*frac;
	tmp.w = w*(1.f-frac) + aRef.w*frac;
	return tmp;
}

inline float Quant::Vector::Dist(const Quant::Vector& aRef) const
{
	Vector tmp;
	tmp.x = x - aRef.x; tmp.y = y - aRef.y; tmp.z = z - aRef.z; tmp.w = w - aRef.w;
	return tmp.Length();
}

inline float Quant::Vector::Dist(float aX, float aY, float aZ, float aW) const
{
	Vector tmp;
	tmp.x = x - aX; tmp.y = y - aY; tmp.z = z - aZ; tmp.w = w - aW;
	return tmp.Length();
}

inline float Quant::Vector::SquareDist(const Quant::Vector& aRef) const
{
	Vector tmp;
	tmp.x = x - aRef.x; tmp.y = y - aRef.y; tmp.z = z - aRef.z; tmp.w = w - aRef.w;
	return tmp.Dot(tmp);
}

inline float Quant::Vector::SquareDist(float aX, float aY, float aZ, float aW /*=1.f*/) const
{
	Vector tmp;
	tmp.x = x - aX; tmp.y = y - aY; tmp.z = z - aZ; tmp.w = w - aW;
	return tmp.Dot(tmp);
}

inline float Quant::Vector::PerceptualDist(float aX, float aY, float aZ, float aW /*=1.f*/) const
{
	Vector tmp;

	tmp.x = fabsf(x - aX);
	tmp.y = fabsf(y - aY);
	tmp.z = fabsf(z - aZ);
	tmp.w = fabsf(w - aW);
	return ((1.f - Quant::GetAlphaPriority()) * (tmp.x*0.299f + tmp.y*0.587f + tmp.z*0.114f) + Quant::GetAlphaPriority() * tmp.w);	// Y component
}

inline float Quant::Vector::Length() const
{
	return sqrtf(Dot(*this));
}

inline float Quant::Vector::Dot(const Quant::Vector& aRef) const
{
	return (x*aRef.x + y*aRef.y + z*aRef.z + w*aRef.w);
}

inline bool Quant::IsVeryClose(const Vector& aLhs, const Vector& aRhs) const
{
	if (aLhs.SquareDist(aRhs) < (0.005f*0.005f))
		return true;

	return false;
}


#endif // __Quant_h__

//
// End.
