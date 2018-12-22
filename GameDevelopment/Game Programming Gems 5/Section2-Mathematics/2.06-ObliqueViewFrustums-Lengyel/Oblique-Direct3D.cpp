inline float sgn(float a)
{
	if (a > 0.0F) return (1.0F);
	if (a < 0.0F) return (-1.0F);
	return (0.0F);
}

struct Vector4D
{
	float	x, y, z, w;
	
	Vector4D() {}
	
	Vector4D(float a, float b, float c, float d)
	{
		x = a; y = b; z = c; w = d;
	}

	// Scalar product
	Vector4D operator *(float s) const
	{
		return (Vector4D(x * s, y * s, z * s, w * s));
	}

	// Dot product
	float operator *(const Vector4D& v) const
	{
		return (x * v.x + y * v.y + z * v.z + w * v.w);
	}
};

void ModifyProjectionMatrix(const Vector4D& clipPlane)
{
	D3DXMatrix	matrix;
	Vector4D	q;

	// Grab the current projection matrix from Direct3D
	D3DDevice.GetTransform(D3DTS_PROJECTION, &matrix);
	
	// Transform the clip-space corner point opposite the clipping plane
	// into camera space by multiplying it by the inverse of
	// the projection matrix
	q.x = (sgn(clipPlane.x) - matrix._31) / matrix._11;
	q.y = (sgn(clipPlane.y) - matrix._32) / matrix._22;
	q.z = 1.0F;
	q.w = (1.0F - matrix._33) / matrix._43;
	
	// Calculate the scaled plane vector
	Vector4D c = clipPlane * (1.0F / (clipPlane * q));
	
	// Replace the third row of the projection matrix
	matrix._13 = c.x;
	matrix._23 = c.y;
	matrix._33 = c.z;
	matrix._43 = c.w;

	// Load it back into Direct3D
	D3DDevice.SetTransform(D3DTS_PROJECTION, &matrix);
}
