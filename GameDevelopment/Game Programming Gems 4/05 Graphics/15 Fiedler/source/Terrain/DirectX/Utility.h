// DirectX Utility

#pragma once

#include "Mathematics/Matrix.h"


namespace DirectX
{
    using namespace Mathematics;

    /// Converts a Matrix into a D3DXMATRIX.
    /// Note that D3DXMATRIX convention is pre-multiplication by a row vector,
    /// hence this conversion takes the transpose of the input matrix.

	inline D3DXMATRIX ConvertMatrix(const Matrix &matrix)
	{
		return D3DXMATRIX(matrix.m11, matrix.m21, matrix.m31, matrix.m41,
						  matrix.m12, matrix.m22, matrix.m32, matrix.m42,
						  matrix.m13, matrix.m23, matrix.m33, matrix.m43,
						  matrix.m14, matrix.m24, matrix.m34, matrix.m44);
	}

    /// Converts a D3DXMATRIX into a Matrix.
    /// Note that Mathematics::Matrix convention is post-multiplication
    /// by a column vector, hence this conversion takes the transpose of the input matrix.

	inline Matrix ConvertMatrix(const D3DXMATRIX &matrix)
	{
		return Matrix(matrix._11, matrix._21, matrix._31, matrix._41, 
					  matrix._12, matrix._22, matrix._32, matrix._42, 
					  matrix._13, matrix._23, matrix._33, matrix._43, 
					  matrix._14, matrix._24, matrix._34, matrix._44);
	}
}
