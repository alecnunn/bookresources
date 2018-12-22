/*****************************************************************************
|
| IndexedTriMesh.cpp
|
| by Graham S. Rhodes
| September 2004
|
| A *very* simple class to support drawing indexed triangle meshes
| via OpenGL.
|
| This class supports companion examples to the chapter titled "Back of the
| Envelope Aerodynamics for Game Physics" in the book "Game
| Programming Gems 5" published by Charles River Media
|
*******************************************************************************/

#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "IndexedTriMesh.h"

IndexedTriMesh::IndexedTriMesh():
	m_uiNumVerts(0), m_uiNumIndices(0), m_pfVertCoords(0),
	m_pfVertNormals(0), m_pfVertTexCoords(0), m_puiIndices(0),
	m_pfColor(), m_pfLocation()
{
	m_pfColor[0] = 1.0f;
	m_pfColor[1] = 1.0f;
	m_pfColor[2] = 1.0f;
	m_pfColor[3] = 1.0f;
	m_pfLocation[0] = 0.0f;
	m_pfLocation[1] = 0.0f;
	m_pfLocation[2] = 0.0f;
}

IndexedTriMesh::~IndexedTriMesh()
{
	Reset();
}

void IndexedTriMesh::Reset()
{
	delete [] m_pfVertCoords;
	delete [] m_pfVertNormals;
	delete [] m_pfVertTexCoords;
	delete [] m_puiIndices;
	m_uiNumVerts = 0;
	m_uiNumIndices = 0;
	m_pfVertCoords = 0;
	m_pfVertNormals = 0;
	m_pfVertTexCoords = 0;
	m_puiIndices = 0;
	m_pfColor[0] = 1.0f;
	m_pfColor[1] = 1.0f;
	m_pfColor[2] = 1.0f;
	m_pfColor[3] = 1.0f;
	m_pfLocation[0] = 0.0f;
	m_pfLocation[1] = 0.0f;
	m_pfLocation[2] = 0.0f;
}

void IndexedTriMesh::Draw()
{
//	glColor4fv(m_pfColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_pfColor);
	glTranslatef(m_pfLocation[0], m_pfLocation[1], m_pfLocation[2]);
	glRotatef(m_fYRotation, 0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < m_uiNumIndices; i += 3)
		{
			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 0]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 0]]);

			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 1]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 1]]);

			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 2]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 2]]);
		}
	glEnd();
}

bool IndexedTriMesh::Load(FILE *pIn)
{
	if (0 == pIn)
		return(false);

	Reset();

	char szLine[256];

	// read and skip first line
	fgets(szLine, 255, pIn);

	// read number of vertices and triangles
	unsigned int uiNumTris = 0;
	fscanf(pIn, "%i %i\n", &m_uiNumVerts, &uiNumTris);

	m_uiNumIndices = uiNumTris * 3;

	m_pfVertCoords = new float[m_uiNumVerts * 3];
	m_pfVertNormals = new float[m_uiNumVerts * 3];
	m_pfVertTexCoords = new float[m_uiNumVerts * 2];
	m_puiIndices = new unsigned int[m_uiNumIndices];
	if (0 == m_pfVertCoords || 0 == m_pfVertNormals || 0 == m_pfVertTexCoords ||
		0 == m_puiIndices)
	{
		Reset();
		return(false);
	}

	unsigned int i = 0, uiCoordIndex = 0, uiTexCoordIndex = 0;
	for (i = 0; i < m_uiNumVerts; i++,
			uiCoordIndex +=3, uiTexCoordIndex += 2)
	{
		fscanf(pIn, "%f %f %f %f %f %f %f %f\n",
				&m_pfVertCoords[uiCoordIndex + 0],
				&m_pfVertCoords[uiCoordIndex + 1],
				&m_pfVertCoords[uiCoordIndex + 2],
				&m_pfVertNormals[uiCoordIndex + 0],
				&m_pfVertNormals[uiCoordIndex + 1],
				&m_pfVertNormals[uiCoordIndex + 2],
				&m_pfVertTexCoords[uiTexCoordIndex + 0],
				&m_pfVertTexCoords[uiTexCoordIndex + 1]);
	}

	unsigned int uiIndexIndex = 0;
	for (i = 0; i < uiNumTris; i++, uiIndexIndex += 3)
	{
		fscanf(pIn, "%i %i %i\n", &m_puiIndices[uiIndexIndex + 0],
				&m_puiIndices[uiIndexIndex + 1],
				&m_puiIndices[uiIndexIndex + 2]);

		// subtract 1 so that we have zero-based indices
		m_puiIndices[uiIndexIndex + 0]--;
		m_puiIndices[uiIndexIndex + 1]--;
		m_puiIndices[uiIndexIndex + 2]--;
	}

	return(true);
}
