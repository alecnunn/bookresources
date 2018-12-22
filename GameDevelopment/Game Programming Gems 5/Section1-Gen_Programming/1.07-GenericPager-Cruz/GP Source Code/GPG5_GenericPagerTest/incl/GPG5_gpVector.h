// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpVector.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpVector Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPVECTOR_H
#define GPG5_GPVECTOR_H
#pragma once

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpVectorDoubleVector
// |  Description : 
// |
// +-----------------------------------------------------------------------
typedef double GPG5_gpVectorDoubleVector[3];

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpVectorDoubleVector
// |  Description : 
// |
// +-----------------------------------------------------------------------
typedef double GPG5_gpVectorDoubleVector2D[2];

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpVector
// |  Description : 
// |								This class is a 3D Vector
// |
// +-----------------------------------------------------------------------
class GPG5_gpVector
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpVector();
		GPG5_gpVector(double x, double y, double z);
		GPG5_gpVector(const GPG5_gpVector& ref);
		virtual ~GPG5_gpVector();

		// +---------------------------
		// |	Components
		// +---------------------------
		void SetComponents(double x, double y, double z);
		void GetComponents(double& x, double& y, double& z) const;

		void SetX(double x);
		void SetY(double y);
		void SetZ(double z);

		double GetX() const;
		double GetY() const;
		double GetZ() const;

		const GPG5_gpVectorDoubleVector& GetVector() const;
		const GPG5_gpVectorDoubleVector2D& GetVector2D() const;

		// +---------------------------
		// |	Operators
		// +---------------------------
		GPG5_gpVector& operator = (const GPG5_gpVector& vec);
		GPG5_gpVector& operator += (const GPG5_gpVector& vec);
		GPG5_gpVector& operator -= (const GPG5_gpVector& vec);
		GPG5_gpVector& operator /= (double escalar);
		GPG5_gpVector& operator *= (double escalar);
		GPG5_gpVector& operator *= (const GPG5_gpVector& vec);

		GPG5_gpVector operator + (const GPG5_gpVector& vec);
		GPG5_gpVector operator - (const GPG5_gpVector& vec);
		GPG5_gpVector operator / (double escalar); 
		GPG5_gpVector operator * (double escalar); 
		GPG5_gpVector operator * (const GPG5_gpVector& vec); // Dot Product

		GPG5_gpVector& Cross(const GPG5_gpVector& vec); // Cross Product
		GPG5_gpVector Cross(const GPG5_gpVector& vec1, const GPG5_gpVector& vec2); // Cross Product

		double Length() const;
		void Normalize();

		// +---------------------------
		// |	Is Valid
		// +---------------------------
		void SetIsValid( bool isValid );
		bool IsValid() const;

	protected:
		// +---------------------------
		// |	Components
		// +---------------------------
		double x_;
		double y_;
		double z_;

		// +---------------------------
		// |	Is Valid
		// +---------------------------
		bool isValid_;

	private:

};

#endif //GPG5_GPVECTOR_H