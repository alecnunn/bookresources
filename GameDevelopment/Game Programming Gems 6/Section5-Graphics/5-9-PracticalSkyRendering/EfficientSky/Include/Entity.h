// Model.cpp: A simple model class.
// Date: 08/01/05
// Created by: Aurelio Reis

#ifndef __AR__ENTITY__H__
#define __AR__ENTITY__H__

#include "dxstdafx.h"
#include "Model.h"


namespace AURELIO_REIS
{
	class CEntity
	{
	public:
		// The entitys model.
		CD3DModel			*m_pModel;

		// The entitys (model) transform.
		D3DXMATRIX			m_matTransform;

		// Only allow drawing of this entity in the views identified by this mask.
		unsigned long		m_ulSceneViewMask;

		// Whether this Entity is visible or not.
		bool				m_bVisible;

		// Whether to apply the special view hack or not.
		bool				m_bViewHack;

		// Constructor.
		CEntity();

		// Pure Virtual Destructor.
		virtual ~CEntity() = 0;

		// Reset to default values.
		virtual void Reset();

		// Initialize this Entity.
		virtual bool Initialize() { return true; }

		// Release the Entity.
		virtual void Release();

		// Prepare any special values for rendering this entity.
		virtual void PrepareRender() {}

		// Update the Entity.
		virtual void Update() {}

		DEFINE_GETSET_ACCESSOR_POINTER( CD3DModel, m_pModel, Model )
		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXMATRIX, m_matTransform, Transform )
		DEFINE_GETSET_ACCESSOR_REFERENCE( unsigned long, m_ulSceneViewMask, SceneViewMask )
		DEFINE_GETSET_ACCESSOR( bool, m_bVisible, Visible )
		DEFINE_GETSET_ACCESSOR( bool, m_bViewHack, ViewHack )
	};


	class CSkyEntity : public CEntity
	{
	private:
		// The sun direction and color.
		D3DXVECTOR3 m_vSunDir;
		D3DXCOLOR	m_vSunColor;

		// The moon direction and color.
		D3DXVECTOR3	m_vMoonDir;
		D3DXCOLOR	m_vMoonColor;

		// Calculate the sun attributes using Preetham's method.
		void CalculateSunAttrPreetham();

	public:
		// Use the Vertex Sky Technique.
		bool		m_bUseVertexSky;

		// Constructor.
		CSkyEntity();

		// Destructor.
		~CSkyEntity();

		// Update the Entity.
		virtual void Update();

		// Prepare any special values for rendering this entity.
		virtual void PrepareRender();

		// Calculate the Sky Attributes.
		void CalculateSkyAttributes();

		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXVECTOR3, m_vSunDir, SunDirection )
		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXCOLOR, m_vSunColor, SunColor )
		
		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXVECTOR3, m_vMoonDir, MoonDirection )
		DEFINE_GETSET_ACCESSOR_REFERENCE( D3DXCOLOR, m_vMoonColor, MoonColor )
	};


	class CBoxEntity : public CEntity
	{
	public:
		// Constructor.
		CBoxEntity();

		// Destructor.
		~CBoxEntity();

		// Prepare any special values for rendering this entity.
		virtual void PrepareRender();
	};


	class CStaticEntity : public CEntity
	{
	public:
		// Constructor.
		CStaticEntity();

		// Destructor.
		~CStaticEntity();

		// Prepare any special values for rendering this entity.
		virtual void PrepareRender();
	};
};


extern CSkyEntity *g_pSkyEntity;


#endif // __AR__ENTITY__H__