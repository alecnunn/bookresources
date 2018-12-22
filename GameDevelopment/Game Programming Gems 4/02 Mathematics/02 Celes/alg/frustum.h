// frustum.h
// Tecgraf/PUC-Rio
// celes@tecgraf.puc-rio.br
// Jul 2003

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/


#ifndef ALG_FRUSTUM_H
#define ALG_FRUSTUM_H

#include "plane.h"

class AlgFrustum
{
public:
 // Creates a new AlgFrustum object
 AlgFrustum (int n)
	{
		m_pl = new AlgPlane[n];
		m_npl = n;
	}

	// Deletes object
	virtual ~AlgFrustum ()
	{
		delete [] m_pl;
	}

	// Sets canonical plane equations
	void SetCanonicalPlane (int id, float a, float b, float c, float d)
	{
  if (id>=0 && id<m_npl)
			m_pl[id].Set(a,b,c,d);
	}

 // Sets vertex transformation matrix
 void SetVertexMatrix (float* m)
	{
		for (int i=0; i<16; i++)
			m_mat[i] = m[i];
	}

	// Gets plane equation in original space
 AlgPlane GetPlane (int id)
	{
		AlgPlane p(0.0f,0.0f,0.0f,0.0f);
  if (id>=0 && id<m_npl)
		{
			// Multiply by the transpose matrix
			p.Set (m_pl[id].a*m_mat[0]+m_pl[id].b*m_mat[1]+
					     m_pl[id].c*m_mat[2]+m_pl[id].d*m_mat[3],
										m_pl[id].a*m_mat[4]+m_pl[id].b*m_mat[5]+
										m_pl[id].c*m_mat[6]+m_pl[id].d*m_mat[7],
										m_pl[id].a*m_mat[8]+m_pl[id].b*m_mat[9]+
										m_pl[id].c*m_mat[10]+m_pl[id].d*m_mat[11],
										m_pl[id].a*m_mat[12]+m_pl[id].b*m_mat[13]+
										m_pl[id].c*m_mat[14]+m_pl[id].d*m_mat[15]
									);
	 }
	 return p;
	}

private:
	int m_npl;
	AlgPlane* m_pl;
 float m_mat[16];
};

#endif
