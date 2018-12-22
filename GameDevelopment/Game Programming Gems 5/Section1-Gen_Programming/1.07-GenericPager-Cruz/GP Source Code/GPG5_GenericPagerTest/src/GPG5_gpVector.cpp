// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpVector.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpVector Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpVector.h>

#include <math.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GPG5_gpVector
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector::GPG5_gpVector():
x_ (0.0),
y_ (0.0),
z_ (0.0),

isValid_ ( true )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GPG5_gpVector
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// |   y             : 
// |   z             : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector::GPG5_gpVector(double x, double y, double z):
x_ (x),
y_ (y),
z_ (z),

isValid_ ( true )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GPG5_gpVector
// | Description     : 
// |
// | Parameters : 
// |   ref           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector::GPG5_gpVector(const GPG5_gpVector& ref):
x_ (ref.x_),
y_ (ref.y_),
z_ (ref.z_),

isValid_ ( ref.isValid_ )
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::~GPG5_gpVector
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector::~GPG5_gpVector()
{
}

// +---------------------------
// |	Components
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::SetComponents
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// |   y             : 
// |   z             : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::SetComponents(double x, double y, double z)
{
	x_ = x;
	y_ = y;
	z_ = z;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetComponents
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// |   y             : 
// |   z             : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::GetComponents(double& x, double& y, double& z) const
{
	x = x_;
	y = y_;
	z = z_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::SetX
// | Description     : 
// |
// | Parameters : 
// |   x             : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::SetX(double x)
{
	x_ = x;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::SetY
// | Description     : 
// |
// | Parameters : 
// |   y             : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::SetY(double y)
{
	y_ = y;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::SetZ
// | Description     : 
// |
// | Parameters : 
// |   z             : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::SetZ(double z)
{
	z_ = z;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetX
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
double GPG5_gpVector::GetX() const
{
	return x_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetY
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
double GPG5_gpVector::GetY() const
{
	return y_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetZ
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
double GPG5_gpVector::GetZ() const
{
	return z_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetVector
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVectorDoubleVector& GPG5_gpVector::GetVector() const
{
	static GPG5_gpVectorDoubleVector vec;
	
	vec[0] = x_;
	vec[1] = y_;
	vec[2] = z_;

	return vec;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::GetVector2D
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const GPG5_gpVectorDoubleVector2D& GPG5_gpVector::GetVector2D() const
{
	static GPG5_gpVectorDoubleVector2D vec;
	
	vec[0] = x_;
	vec[1] = y_;

	return vec;
}

// +---------------------------
// |	Operators
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator= 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator = (const GPG5_gpVector& vec)
{
	x_ = vec.x_;
	y_ = vec.y_;
	z_ = vec.z_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator+= 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator += (const GPG5_gpVector& vec)
{
	x_ += vec.x_;
	y_ += vec.y_;
	z_ += vec.z_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator-= 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator -= (const GPG5_gpVector& vec)
{
	x_ -= vec.x_;
	y_ -= vec.y_;
	z_ -= vec.z_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator/= 
// | Description     : 
// |
// | Parameters : 
// |   escalar       : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator /= (double escalar)
{
	if( (escalar > 0.0) || ((escalar < 0.0)) )
	{
		x_ /= escalar;
		y_ /= escalar;
		z_ /= escalar;
	}
	else
	{
		x_ = 0.0;
		y_ = 0.0;
		z_ = 0.0;
	}

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator*= 
// | Description     : 
// |
// | Parameters : 
// |   escalar       : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator *= (double escalar)
{
	x_ *= escalar;
	y_ *= escalar;
	z_ *= escalar;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator*= 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::operator *= (const GPG5_gpVector& vec)
{
	x_ *= vec.x_;
	y_ *= vec.y_;
	z_ *= vec.z_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator+ 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::operator + (const GPG5_gpVector& vec)
{
	return GPG5_gpVector
		(
			x_ + vec.x_,
			y_ + vec.y_,
			z_ + vec.z_
		);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator- 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::operator - (const GPG5_gpVector& vec)
{
	return GPG5_gpVector
		(
			x_ - vec.x_,
			y_ - vec.y_,
			z_ - vec.z_
		);
}


// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator/
// | Description     : 
// |
// | Parameters : 
// |   escalar       : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::operator / (double escalar)
{
	if( (escalar > 0.0) || ((escalar < 0.0)) )
	{
		return GPG5_gpVector
			(
				x_ / escalar,
				y_ / escalar,
				z_ / escalar
			);
	}
	else
	{
		return GPG5_gpVector
			(
				0.0,
				0.0,
				0.0
			);
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator* 
// | Description     : 
// |
// | Parameters : 
// |   escalar       : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::operator * (double escalar)
{
	return GPG5_gpVector
		(
			x_ * escalar,
			y_ * escalar,
			z_ * escalar
		);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::operator* 
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::operator * (const GPG5_gpVector& vec)
{
	return GPG5_gpVector
		(
			x_*vec.x_,
			y_*vec.y_,
			z_*vec.z_
		);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::Cross
// | Description     : 
// |
// | Parameters : 
// |   vec           : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector& GPG5_gpVector::Cross(const GPG5_gpVector& vec)
{
	x_ = y_*vec.z_ - z_*vec.y_;
	y_ = z_*vec.x_ - x_*vec.z_;
	z_ = x_*vec.y_ - y_*vec.x_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::Cross
// | Description     : 
// |
// | Parameters : 
// |   vec1          : 
// |   vec2          : 
// | 
// +-------------------------------------------------------------
GPG5_gpVector GPG5_gpVector::Cross(const GPG5_gpVector& vec1, const GPG5_gpVector& vec2)
{
	x_ = vec1.y_*vec2.z_ - vec1.z_*vec2.y_;
	y_ = vec1.z_*vec2.x_ - vec1.x_*vec2.z_;
	z_ = vec1.x_*vec2.y_ - vec1.y_*vec2.x_;

	return (*this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::Length
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
double GPG5_gpVector::Length() const
{
    return sqrt(x_*x_ + y_*y_ + z_*z_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::Normalize
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::Normalize()
{
  double length = Length();
    
	if (length > 0.0f)
	{
		length  = 1.0 / length;
	}
	else
	{
    length  = 0.0f;
	}

  x_ *= length ;
  y_ *= length ;
  z_ *= length ;
}
		
// +---------------------------
// |	Is Valid
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::SetIsValid
// | Description     : 
// |
// | Parameters : 
// |   isValid       : 
// | 
// +-------------------------------------------------------------
void GPG5_gpVector::SetIsValid( bool isValid )
{
	isValid_ = isValid;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpVector::IsValid
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpVector::IsValid() const
{
	return isValid_;
}
