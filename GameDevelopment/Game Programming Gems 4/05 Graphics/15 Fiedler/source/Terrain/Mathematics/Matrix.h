// Matrix class

#pragma once

#include "Vector.h"


namespace Mathematics
{
    /// 4x4 matrix class.
    /// The convention here is post-multiplication by a column vector.
    /// ie. x = Ab, where x and b are column vectors.
    /// Please note that in cases where a matrix is pre-multiplied by a 
    /// vector, we then assume that the vector is a row vector.
    /// This operation is then equivalent to post multiplying the vector
	/// treated as a column vector by the transpose of the actual matrix.
    /// If you wish to think of this matrix in terms of basis vectors,
    /// then by convention, the rows of this matrix form the set of
    /// basis vectors.
    /// When composing matrix transforms A * B * C * D, note that the
    /// actual order of operations as visible in the resultant matrix
    /// is D, C, B, A. Alternatively, you can view transforms as changing
    /// coordinate system, then the coordinate systems are changed 
    /// in order A, B, C, D.

    class Matrix
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Matrix() {};

        /// construct a matrix from three basis vectors.
        /// the x,y,z values from each of these basis vectors map to rows in the 3x3 sub matrix.
        /// note: the rest of the matrix (row 4 and column 4 are set to identity)

	    Matrix(const Vector &a, const Vector &b, const Vector &c)
	    {
		    // ax ay az 0
		    // bx by bz 0
		    // cx cy cz 0
		    // 0  0  0  1

		    m11 = a.x;
		    m12 = a.y;
		    m13 = a.z;
		    m14 = 0;
		    m21 = b.x;
		    m22 = b.y;
		    m23 = b.z;
		    m24 = 0;
		    m31 = c.x;
		    m32 = c.y;
		    m33 = c.z;
		    m34 = 0;
		    m41 = 0;
		    m42 = 0;
		    m43 = 0;
		    m44 = 1;
	    }

        /// construct a matrix from explicit values for the 3x3 sub matrix.
        /// note: the rest of the matrix (row 4 and column 4 are set to identity)

	    Matrix(float m11, float m12, float m13,
		       float m21, float m22, float m23,
		       float m31, float m32, float m33)
	    {
            this->m11 = m11;
            this->m12 = m12;
            this->m13 = m13;
            this->m14 = 0;
            this->m21 = m21;
            this->m22 = m22;
            this->m23 = m23;
            this->m24 = 0;
            this->m31 = m31;
            this->m32 = m32;
            this->m33 = m33;
            this->m34 = 0;
            this->m41 = 0;
            this->m42 = 0;
            this->m43 = 0;
            this->m44 = 1;
        }

        /// construct a matrix from explicit entry values for the whole 4x4 matrix.

        Matrix(float m11, float m12, float m13, float m14,
		       float m21, float m22, float m23, float m24,
		       float m31, float m32, float m33, float m34,
		       float m41, float m42, float m43, float m44)
	    {
            this->m11 = m11;
            this->m12 = m12;
            this->m13 = m13;
            this->m14 = m14;
            this->m21 = m21;
            this->m22 = m22;
            this->m23 = m23;
            this->m24 = m24;
            this->m31 = m31;
            this->m32 = m32;
            this->m33 = m33;
            this->m34 = m34;
            this->m41 = m41;
            this->m42 = m42;
            this->m43 = m43;
            this->m44 = m44;
	    }

        /// load matrix from raw float array.
        /// data is assumed to be stored linearly in memory in row order, from left to right, top to bottom.

	    Matrix(const float data[])
	    {
            this->m11 = data[0];
            this->m12 = data[1];
            this->m13 = data[2];
            this->m14 = data[3];
            this->m21 = data[4];
            this->m22 = data[5];
            this->m23 = data[6];
            this->m24 = data[7];
            this->m31 = data[8];
            this->m32 = data[9];
            this->m33 = data[10];
            this->m34 = data[11];
            this->m41 = data[12];
            this->m42 = data[13];
            this->m43 = data[14];
            this->m44 = data[15];
	    }

        /// set all entries in matrix to identity.

	    void zero()
        {
            m11 = 0;
            m12 = 0;
            m13 = 0;
            m14 = 0;
            m21 = 0;
            m22 = 0;
            m23 = 0;
            m24 = 0;
            m31 = 0;
            m32 = 0;
            m33 = 0;
            m34 = 0;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 0;
        }

        /// set matrix to identity.

        void identity()
        {
            m11 = 1;
            m12 = 0;
            m13 = 0;
            m14 = 0;
            m21 = 0;
            m22 = 1;
            m23 = 0;
            m24 = 0;
            m31 = 0;
            m32 = 0;
            m33 = 1;
            m34 = 0;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 1;
        }

        /// set to a translation matrix.

        void translate(float x, float y, float z)
        {
            m11 = 1;		  // 1 0 0 x 
            m12 = 0;		  // 0 1 0 y
            m13 = 0;		  // 0 0 1 z
            m14 = x;		  // 0 0 0 1
            m21 = 0;
            m22 = 1;
            m23 = 0;
            m24 = y;
            m31 = 0;
            m32 = 0;
            m33 = 1;
            m34 = z;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 1;
        }

        /// set to a translation matrix.

        void translate(const Vector &vector)
        {
            m11 = 1;		  // 1 0 0 x 
            m12 = 0;		  // 0 1 0 y
            m13 = 0;		  // 0 0 1 z
            m14 = vector.x;   // 0 0 0 1
            m21 = 0;
            m22 = 1;
            m23 = 0;
            m24 = vector.y;
            m31 = 0;
            m32 = 0;
            m33 = 1;
            m34 = vector.z;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 1;
        }

        /// set to a scale matrix.

        void scale(float s)
        {
            m11 = s;
            m12 = 0;
            m13 = 0;
            m14 = 0;
            m21 = 0;
            m22 = s;
            m23 = 0;
            m24 = 0;
            m31 = 0;
            m32 = 0;
            m33 = s;
            m34 = 0;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 1;
        }

        /// set to a diagonal matrix.

	    void diagonal(float a, float b, float c, float d = 1)
	    {
            m11 = a;
            m12 = 0;
            m13 = 0;
            m14 = 0;
            m21 = 0;
            m22 = b;
            m23 = 0;
            m24 = 0;
            m31 = 0;
            m32 = 0;
            m33 = c;
            m34 = 0;
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = d;
	    }

        /// set matrix to a rotation around the specified axis.
        
        void rotate(float angle, const Vector &axis)
	    {
		    assert(axis.normalized());

		    float sine = Float::sin(angle);
            float cosine = Float::cos(angle);
            float oneMinusCosine = 1.0f - cosine;
		    
            float xx = axis.x*axis.x;
		    float yy = axis.y*axis.y;
		    float zz = axis.z*axis.z;
		    
            float xym = axis.x*axis.y*oneMinusCosine;
		    float xzm = axis.x*axis.z*oneMinusCosine;
		    float yzm = axis.y*axis.z*oneMinusCosine;
		    
            float xSine = axis.x*sine;
		    float ySine = axis.y*sine;
		    float zSine = axis.z*sine;
        
		    m11 = xx*oneMinusCosine + cosine;
		    m12 = xym - zSine;
		    m13 = xzm + ySine;
		    m14 = 0;

		    m21 = xym + zSine;
		    m22 = yy*oneMinusCosine + cosine;
		    m23 = yzm - xSine;
		    m24 = 0;

		    m31 = xzm - ySine;
		    m32 = yzm + xSine;
		    m33 = zz*oneMinusCosine + cosine;
		    m34 = 0;

		    m41 = 0;
		    m42 = 0;
		    m43 = 0;
		    m44 = 1;
        }

        /// set to a look at matrix.
        /// left handed.

        void lookat(Vector eye, Vector at, Vector up)
        {
            Vector z_axis = at - eye;
		    Vector x_axis = up.cross(z_axis);
		    Vector y_axis = z_axis.cross(x_axis);

		    x_axis.normalize();
		    y_axis.normalize();
		    z_axis.normalize();

		    m11	= x_axis.x;
		    m12 = x_axis.y;
		    m13 = x_axis.z;
		    m14 = - x_axis.dot(eye);

		    m21	= y_axis.x;
		    m22 = y_axis.y;
		    m23 = y_axis.z;
		    m24 = - y_axis.dot(eye);

            m31	= z_axis.x;
		    m32 = z_axis.y;
		    m33 = z_axis.z;
		    m34 = - z_axis.dot(eye);

            m41	= 0;
		    m42 = 0;
		    m43 = 0;
		    m44 = 1;
        }

        /// set to an orthographic projection matrix.
        /// Left handed Direct3D style.

        void orthographic(float l, float r, float b, float t, float n, float f)
        {
            // | 2/(r-l)      0            0           0 |
            // | 0            2/(t-b)      0           0 |
            // | 0            0            1/(zf-zn)   0 |
            // | (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1 |

            m11 = 2 / (r-l);
            m12 = 0;
            m13 = 0;
            m14 = (l+r) / (l-r);
            
            m21 = 0;
            m22 = 2 / (t-b);
            m23 = 0;
            m24 = (t+b) / (b-t);
            
            m31 = 0;
            m32 = 0;
            m33 = 1 / (f-n);
            m34 = n / (n-f);
            
            m41 = 0;
            m42 = 0;
            m43 = 0;
            m44 = 1;
        }

        /// set to an orthographic projection matrix.
        /// Direct3D style.

        void orthographic(float width, float height, float n, float f)
        {
            orthographic(0, width, 0, height, n, f);
        }

        /// set to a perspective projection matrix.
        /// Left handed Direct3D style.

        void perspective(float l, float r, float b, float t, float n, float f)
        {
            // | 2*zn/(r-l)   0            0              0 |
            // | 0            2*zn/(t-b)   0              0 |
            // | (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1 |
            // | 0            0            zn*zf/(zn-zf)  0 |

		    m11	= 2*n / (r-l);
		    m12 = 0;
		    m13 = (l+r)/(l-r);
		    m14 = 0;

		    m21 = 0;
		    m22 = 2*n / (t-b);
		    m23 = (t+b)/(b-t);
		    m24 = 0;

            m31 = 0;
            m32 = 0;
            m33 = f / (f-n);
            m34 = n*f / (n-f);

            m41 = 0;
            m42 = 0;
            m43 = 1;
            m44 = 0;
        }

        /// set to a perspective projection matrix specified in terms of field of view and aspect ratio.
        /// Sortof OpenGL gluPerspective style, but mapping through a Direct3D style projection matrix underneath.

        void perspective(float fov, float aspect, float n, float f)
        {
		    const float t = Float::tan(fov*0.5f) * n;
		    const float b = -t;

		    const float l = aspect * b;
		    const float r = aspect * t;

		    perspective(l,r,b,t,n,f);
	    }

        /// calculate determinant of 3x3 sub matrix.

	    float determinant() const
        {
	        return -m13*m22*m31 + m12*m23*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 + m11*m22*m33;
        }

        /// determine if matrix is invertible.
        /// note: only checks 3x3 sub matrix determinant.

	    bool invertible() const
	    {
		    return !Float::equal(determinant(),0);
	    }

        /// calculate inverse of matrix.

	    Matrix inverse() const
	    {
		    Matrix matrix;
		    inverse(matrix);
		    return matrix;
	    }

        /// calculate inverse of matrix and write result to parameter matrix.

        void inverse(Matrix &inverse) const
        {
		    const float determinant = this->determinant();

		    assert(!Float::equal(determinant,0));

	        float k = 1.0f / determinant;

	        inverse.m11 = (m22*m33 - m32*m23) * k;
	        inverse.m12 = (m32*m13 - m12*m33) * k;
	        inverse.m13 = (m12*m23 - m22*m13) * k;
	        inverse.m21 = (m23*m31 - m33*m21) * k;
	        inverse.m22 = (m33*m11 - m13*m31) * k;
	        inverse.m23 = (m13*m21 - m23*m11) * k;
	        inverse.m31 = (m21*m32 - m31*m22) * k;
	        inverse.m32 = (m31*m12 - m11*m32) * k;
	        inverse.m33 = (m11*m22 - m21*m12) * k;

	        inverse.m14 = -(inverse.m11*m14 + inverse.m12*m24 + inverse.m13*m34);
	        inverse.m24 = -(inverse.m21*m14 + inverse.m22*m24 + inverse.m23*m34);
	        inverse.m34 = -(inverse.m31*m14 + inverse.m32*m24 + inverse.m33*m34);

	        inverse.m41 = m41;
	        inverse.m42 = m42;
	        inverse.m43 = m43;
	        inverse.m44 = m44;
        }

        /// calculate transpose of matrix.

	    Matrix transpose() const
	    {
		    Matrix matrix;
		    transpose(matrix);
		    return matrix;
	    }

        /// calculate transpose of matrix and write to parameter matrix.

	    void transpose(Matrix &transpose) const
        {
            transpose.m11 = m11;
            transpose.m12 = m21;
            transpose.m13 = m31;
            transpose.m14 = m41;
            transpose.m21 = m12;
            transpose.m22 = m22;
            transpose.m23 = m32;
            transpose.m24 = m42;
            transpose.m31 = m13;
            transpose.m32 = m23;
            transpose.m33 = m33;
            transpose.m34 = m43;
            transpose.m41 = m14;
            transpose.m42 = m24;
            transpose.m43 = m34;
            transpose.m44 = m44;
        }

        /// transform a vector by this matrix.
        /// the convention used is post-multiplication by a column vector: x=Ab.

        void transform(Vector &vector) const
        {
		    float x = vector.x * m11 + vector.y * m12 + vector.z * m13 + m14;
            float y = vector.x * m21 + vector.y * m22 + vector.z * m23 + m24;
            float z = vector.x * m31 + vector.y * m32 + vector.z * m33 + m34;
		    vector.x = x;
		    vector.y = y;
		    vector.z = z;
        }

        /// transform a vector by this matrix, store result in parameter.
        /// the convention used is post-multiplication by a column vector: x=Ab.

        void transform(const Vector &vector, Vector &result) const
        {
            result.x = vector.x * m11 + vector.y * m12 + vector.z * m13 + m14;
            result.y = vector.x * m21 + vector.y * m22 + vector.z * m23 + m24;
            result.z = vector.x * m31 + vector.y * m32 + vector.z * m33 + m34;
        }

        /// add another matrix to this matrix.

	    void add(const Matrix &matrix)
	    {
            m11 += matrix.m11;
            m12 += matrix.m12;
            m13 += matrix.m13;
            m14 += matrix.m14;
            m21 += matrix.m21;
            m22 += matrix.m22;
            m23 += matrix.m23;
            m24 += matrix.m24;
            m31 += matrix.m31;
            m32 += matrix.m32;
            m33 += matrix.m33;
            m34 += matrix.m34;
            m41 += matrix.m41;
            m42 += matrix.m42;
            m43 += matrix.m43;
            m44 += matrix.m44;
	    }

        /// subtract a matrix from this matrix.

	    void subtract(const Matrix &matrix)
	    {
            m11 -= matrix.m11;
            m12 -= matrix.m12;
            m13 -= matrix.m13;
            m14 -= matrix.m14;
            m21 -= matrix.m21;
            m22 -= matrix.m22;
            m23 -= matrix.m23;
            m24 -= matrix.m24;
            m31 -= matrix.m31;
            m32 -= matrix.m32;
            m33 -= matrix.m33;
            m34 -= matrix.m34;
            m41 -= matrix.m41;
            m42 -= matrix.m42;
            m43 -= matrix.m43;
            m44 -= matrix.m44;
	    }

        /// multiply this matrix by a scalar.

	    void multiply(float scalar)
	    {
            m11 *= scalar;
            m12 *= scalar;
            m13 *= scalar;
            m14 *= scalar;
            m21 *= scalar;
            m22 *= scalar;
            m23 *= scalar;
            m24 *= scalar;
            m31 *= scalar;
            m32 *= scalar;
            m33 *= scalar;
            m34 *= scalar;
            m41 *= scalar;
            m42 *= scalar;
            m43 *= scalar;
            m44 *= scalar;
	    }

        /// multiply two matrices.
        /// note: this is a full 4x4 matrix multiplication.

	    void multiply(const Matrix &matrix, Matrix &result)
        {
            result.m11 = m11*matrix.m11 + m12*matrix.m21 + m13*matrix.m31 + m14*matrix.m41;
            result.m12 = m11*matrix.m12 + m12*matrix.m22 + m13*matrix.m32 + m14*matrix.m42;
            result.m13 = m11*matrix.m13 + m12*matrix.m23 + m13*matrix.m33 + m14*matrix.m43;
            result.m14 = m11*matrix.m14 + m12*matrix.m24 + m13*matrix.m34 + m14*matrix.m44;
            result.m21 = m21*matrix.m11 + m22*matrix.m21 + m23*matrix.m31 + m24*matrix.m41;
            result.m22 = m21*matrix.m12 + m22*matrix.m22 + m23*matrix.m32 + m24*matrix.m42;
            result.m23 = m21*matrix.m13 + m22*matrix.m23 + m23*matrix.m33 + m24*matrix.m43;
            result.m24 = m21*matrix.m14 + m22*matrix.m24 + m23*matrix.m34 + m24*matrix.m44;
            result.m31 = m31*matrix.m11 + m32*matrix.m21 + m33*matrix.m31 + m34*matrix.m41;
            result.m32 = m31*matrix.m12 + m32*matrix.m22 + m33*matrix.m32 + m34*matrix.m42;
            result.m33 = m31*matrix.m13 + m32*matrix.m23 + m33*matrix.m33 + m34*matrix.m43;
            result.m34 = m31*matrix.m14 + m32*matrix.m24 + m33*matrix.m34 + m34*matrix.m44;
            result.m41 = m41*matrix.m11 + m42*matrix.m21 + m43*matrix.m31 + m44*matrix.m41;
            result.m42 = m41*matrix.m12 + m42*matrix.m22 + m43*matrix.m32 + m44*matrix.m42;
            result.m43 = m41*matrix.m13 + m42*matrix.m23 + m43*matrix.m33 + m44*matrix.m43;
            result.m44 = m41*matrix.m14 + m42*matrix.m24 + m43*matrix.m34 + m44*matrix.m44;
        }

        /// equals operator

	    bool operator ==(const Matrix &other) const
	    {
		    if (Float::equal(m11,other.m11) &&
			    Float::equal(m12,other.m12) &&
			    Float::equal(m13,other.m13) &&
			    Float::equal(m14,other.m14) &&
			    Float::equal(m21,other.m21) &&
			    Float::equal(m22,other.m22) &&
			    Float::equal(m23,other.m23) &&
			    Float::equal(m24,other.m24) &&
			    Float::equal(m31,other.m31) &&
			    Float::equal(m32,other.m32) &&
			    Float::equal(m33,other.m33) &&
			    Float::equal(m34,other.m34) &&
			    Float::equal(m41,other.m41) &&
			    Float::equal(m42,other.m42) &&
			    Float::equal(m43,other.m43) &&
			    Float::equal(m44,other.m44)) return true;
		    else return false;
	    }

        /// not equals operator

	    bool operator !=(const Matrix &other) const
	    {
		    return !(*this==other);
	    }

        /// cute access to matrix elements via overloaded () operator.
        /// use it like this: Matrix matrix; float element = matrix(row, column);

	    float operator()(int i, int j) const
	    {
		    assert(i>=1);
		    assert(i<=4);
		    assert(j>=1);
		    assert(j<=4);
		    const float *data = &m11;
		    return data[(i<<2) + j];
	    }

		/// cute access to basis vectors of 3x3 submatrix

		const Vector& i() const
		{
			return *((Vector*)&m11);
		}

		const Vector& j() const
		{
			return *((Vector*)&m21);
		}

		const Vector& k() const
		{
			return *((Vector*)&m31);
		}

		/// print a basic representation of the matrix data to stdout.

	    void print() const
	    {
		    printf("Matrix[(%f, %f, %f, %f) | (%f, %f, %f, %f) | (%f, %f, %f, %f) | (%f, %f, %f, %f)]", m11,m12,m13,m14,m21,m22,m23,m24,m31,m32,m33,m34,m41,m42,m43,m44);
	    }

	    friend inline Matrix operator-(const Matrix &a);
	    friend inline Matrix operator+(const Matrix &a, const Matrix &b);
	    friend inline Matrix operator-(const Matrix &a, const Matrix &b);
	    friend inline Matrix operator*(const Matrix &a, const Matrix &b);
	    friend inline Matrix& operator+=(Matrix &a, const Matrix &b);
	    friend inline Matrix& operator-=(Matrix &a, const Matrix &b);
	    friend inline Matrix& operator*=(Matrix &a, const Matrix &b);

	    friend inline Vector operator*(const Matrix &matrix, const Vector &vector);
	    friend inline Vector operator*(const Vector &vector, const Matrix &matrix);
	    friend inline Vector& operator*=(Vector &vector, const Matrix &matrix);

	    friend inline Matrix operator*(const Matrix &a, float s);
	    friend inline Matrix operator/(const Matrix &a, float s);
	    friend inline Matrix& operator*=(Matrix &a, float s);
	    friend inline Matrix& operator/=(Matrix &a, float s);
	    friend inline Matrix operator*(float s, const Matrix &a);
	    friend inline Matrix& operator*=(float s, Matrix &a);

        float m11,m12,m13,m14;			// 4x4 matrix, index m[row][column], convention: pre-multiply column vector, Ax = b
        float m21,m22,m23,m24;          // hence: (m11,m21,m31) make up the x axis of the 3x3 sub matrix,
        float m31,m32,m33,m34;          // and (m14,m24,m34) is the translation vector.
        float m41,m42,m43,m44;
    };


    inline Matrix operator-(const Matrix &a)
    {
	    return Matrix(-a.m11, -a.m12, -a.m13, -a.m14,
				      -a.m21, -a.m22, -a.m23, -a.m24,
				      -a.m31, -a.m32, -a.m33, -a.m34,
				      -a.m41, -a.m42, -a.m43, -a.m44);
    }

    inline Matrix operator+(const Matrix &a, const Matrix &b)
    {
	    return Matrix(a.m11+b.m11, a.m12+b.m12, a.m13+b.m13, a.m14+b.m14,
				      a.m21+b.m21, a.m22+b.m22, a.m23+b.m23, a.m24+b.m24,
				      a.m31+b.m31, a.m32+b.m32, a.m33+b.m33, a.m34+b.m34,
				      a.m41+b.m41, a.m42+b.m42, a.m43+b.m43, a.m44+b.m44);
    }

    inline Matrix operator-(const Matrix &a, const Matrix &b)
    {
	    return Matrix(a.m11-b.m11, a.m12-b.m12, a.m13-b.m13, a.m14-b.m14,
				      a.m21-b.m21, a.m22-b.m22, a.m23-b.m23, a.m24-b.m24,
				      a.m31-b.m31, a.m32-b.m32, a.m33-b.m33, a.m34-b.m34,
				      a.m41-b.m41, a.m42-b.m42, a.m43-b.m43, a.m44-b.m44);
    }

    inline Matrix operator*(const Matrix &a, const Matrix &b)
    {
	    return Matrix(a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.m41,
				      a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.m42,
				      a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.m43,
				      a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34 + a.m14*b.m44,
				      a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.m41,
				      a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.m42,
				      a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.m43,
				      a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34 + a.m24*b.m44,
				      a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.m41,
				      a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.m42,
				      a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.m43,
				      a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34 + a.m34*b.m44,
				      a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.m41,
				      a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.m42,
				      a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.m43,
				      a.m41*b.m14 + a.m42*b.m24 + a.m43*b.m34 + a.m44*b.m44);
    }

    inline Matrix& operator+=(Matrix &a, const Matrix &b)
    {
	    a.add(b);
	    return a;
    }

    inline Matrix& operator-=(Matrix &a, const Matrix &b)
    {
	    a.subtract(b);
	    return a;
    }

    inline Matrix& operator*=(Matrix &a, const Matrix &b)
    {
	    a = Matrix(a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31 + a.m14*b.m41,
			       a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32 + a.m14*b.m42,
			       a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33 + a.m14*b.m43,
			       a.m11*b.m14 + a.m12*b.m24 + a.m13*b.m34 + a.m14*b.m44,
			       a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31 + a.m24*b.m41,
			       a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32 + a.m24*b.m42,
			       a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33 + a.m24*b.m43,
			       a.m21*b.m14 + a.m22*b.m24 + a.m23*b.m34 + a.m24*b.m44,
			       a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31 + a.m34*b.m41,
			       a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32 + a.m34*b.m42,
			       a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33 + a.m34*b.m43,
			       a.m31*b.m14 + a.m32*b.m24 + a.m33*b.m34 + a.m34*b.m44,
			       a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + a.m44*b.m41,
			       a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + a.m44*b.m42,
			       a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + a.m44*b.m43,
			       a.m41*b.m14 + a.m42*b.m24 + a.m43*b.m34 + a.m44*b.m44);
	    return a;											 
    }

    inline Vector operator*(const Matrix &matrix, const Vector &vector)
    {
	    return Vector(vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + matrix.m14,
				      vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + matrix.m24,
				      vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + matrix.m34);
    }

    inline Vector operator*(const Vector &vector, const Matrix &matrix)
    {
	    // when we premultiply x*A we assume the vector is a row vector

	    return Vector(vector.x * matrix.m11 + vector.y * matrix.m21 + vector.z * matrix.m31 + matrix.m41,
				      vector.x * matrix.m12 + vector.y * matrix.m22 + vector.z * matrix.m32 + matrix.m42,
                      vector.x * matrix.m13 + vector.y * matrix.m23 + vector.z * matrix.m33 + matrix.m43);
    }

    inline Vector& operator*=(Vector &vector, const Matrix &matrix)
    {
	    // equivalent to: vector = matrix * vector (not vector*matrix!)

	    const float rx = vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + matrix.m14;
	    const float ry = vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + matrix.m24;
	    const float rz = vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + matrix.m34;
	    vector.x = rx;
	    vector.y = ry;
	    vector.z = rz;
	    return vector;
    }
    	
    inline Matrix operator*(const Matrix &a, float s)
    {
	    return Matrix(s*a.m11, s*a.m12, s*a.m13, s*a.m14,
				      s*a.m21, s*a.m22, s*a.m23, s*a.m24,
				      s*a.m31, s*a.m32, s*a.m33, s*a.m34,
				      s*a.m41, s*a.m42, s*a.m43, s*a.m44);
    }

    inline Matrix operator/(const Matrix &a, float s)
    {
	    assert(s!=0);
	    const float inv = 1.0f / s;
	    return Matrix(inv*a.m11, inv*a.m12, inv*a.m13, inv*a.m14,
				      inv*a.m21, inv*a.m22, inv*a.m23, inv*a.m24,
				      inv*a.m31, inv*a.m32, inv*a.m33, inv*a.m34,
				      inv*a.m41, inv*a.m42, inv*a.m43, inv*a.m44);
    }

    inline Matrix& operator*=(Matrix &a, float s)
    {
	    a.multiply(s);
	    return a;
    }

    inline Matrix& operator/=(Matrix &a, float s)
    {
	    assert(s!=0);
	    a.multiply(1.0f/s);
	    return a;
    }

    inline Matrix operator*(float s, const Matrix &a)
    {
	    return Matrix(s*a.m11, s*a.m12, s*a.m13, s*a.m14,
				      s*a.m21, s*a.m22, s*a.m23, s*a.m24,
				      s*a.m31, s*a.m32, s*a.m33, s*a.m34,
				      s*a.m41, s*a.m42, s*a.m43, s*a.m44);
    }

    inline Matrix& operator*=(float s, Matrix &a)
    {
	    a.multiply(s);
	    return a;
    }
}
