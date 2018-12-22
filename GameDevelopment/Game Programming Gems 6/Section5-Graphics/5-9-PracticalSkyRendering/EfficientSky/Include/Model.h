// Model.h: A simple model class.
// Date: 08/01/05
// Created by: Aurelio Reis

#ifndef __AR__MODEL__H__
#define __AR__MODEL__H__

#include "dxstdafx.h"


namespace AURELIO_REIS
{
	// Just a simple model class.
	class CD3DModel
	{
	private:
		LPDIRECT3DVERTEXBUFFER9		m_pVB;
		LPDIRECT3DINDEXBUFFER9		m_pIB;
		int							m_iNumPolys;
		int							m_iNumVerts;
		int							m_iNumIndices;
		int							m_iVertexSize;

		D3DXVECTOR3					m_vCenter;
		float						m_fRadius;

		IDirect3DVertexDeclaration9 *m_pDecl;

	public:
		// Constructor.
		CD3DModel();

		// Destructor.
		~CD3DModel();

		// Load the model into this Object, returning false for failure.
		bool LoadModel( const WCHAR *strFileName, const D3DVERTEXELEMENT9 VertexElements[] );

		// Prepare the Models states for rendering.
		void PrepareForRender();

		// Render the Model.
		void Render();

		// Release the Model.
		void Release();

		// Reset to default values.
		void Reset();

		// Lock Vertices for writing to.
		void LockVertices( void **ppVertices );

		// Unlock the Vertices.
		void UnlockVertices();

		DEFINE_GETSET_ACCESSOR( int, m_iNumPolys, NumPolys )
		DEFINE_GETSET_ACCESSOR( int, m_iNumVerts, NumVerts )
		DEFINE_GETSET_ACCESSOR( int, m_iNumIndices, NumIndices )
		DEFINE_GETSET_ACCESSOR( int, m_iVertexSize, VertexSize )
		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXVECTOR3, m_vCenter, Center )
		DEFINE_GETSET_ACCESSOR( float, m_fRadius, Radius )
	};
};


#endif // __AR__MODEL__H__