// SceneManager.h: A Scene Manager Class.
// Date: 08/02/05
// Created by: Aurelio Reis

#ifndef __AR__SCENEMANAGER__H__
#define __AR__SCENEMANAGER__H__

#include "dxstdafx.h"


namespace AURELIO_REIS
{
	// Class pre-defines.
	class CEntity;


	// The View ID's used by an Entities View Mask.
	enum
	{
		VIEWID_INVALID	= 0,
		VIEWID_0		= SETBIT( 0 ),
		VIEWID_1		= SETBIT( 1 ),
		VIEWID_2		= SETBIT( 2 ),
		VIEWID_3		= SETBIT( 3 ),
		MAX_SCENE_VIEWS = 4
	};


	// Used for skipping cube faces in a cube render.
	enum
	{
		SKIP_CUBEFACE_0 = SETBIT( 0 ),
		SKIP_CUBEFACE_1 = SETBIT( 1 ),
		SKIP_CUBEFACE_2 = SETBIT( 2 ),
		SKIP_CUBEFACE_3 = SETBIT( 3 ),
		SKIP_CUBEFACE_4 = SETBIT( 4 ),
		SKIP_CUBEFACE_5 = SETBIT( 5 )
	};


	// The maximum view recursions allowed.
	#define MAX_VIEW_RECURSIONS		4


	enum ViewType
	{
		// Just render a normal view.
		VIEWTYPE_NORMAL,
		// A render-to-texture view.
		VIEWTYPE_RTT,
		// A render-to-(cubemap)texture view.
		VIEWTYPE_CUBE_RTT,
		// This view convolves a cube map by converting a cubemap into a paraboloid,
		// projecting into spherical harmonic space, then convolving before converting
		// back to cube space.
		VIEWTYPE_PARABOLOID_CONVOLUTION
	};


	class CViewRenderTarget
	{
	public:
		IDirect3DBaseTexture9	*m_pRenderTarget;
		IDirect3DSurface9		*m_pRenderTargetDepthStencil;

		// Constructor.
		CViewRenderTarget()
		{
			m_pRenderTarget = NULL;
			m_pRenderTargetDepthStencil = NULL;
		}

		// Destructor.
		~CViewRenderTarget() {}
	};


	// A specific view in a scene.
	class CSceneView
	{
	public:
		// The view this view is dependent on.
		CSceneView *m_pParentView;

		// Any Sub-Views that depend on this View.
		vector< CSceneView * > m_SubViews;

		// The View ID.
		unsigned long m_ulViewID;

		// How to render this view.
		ViewType m_ViewType;

		// The view matrix for this view.
		D3DXMATRIX m_matView;

		// The projection matrix for this view.
		D3DXMATRIX m_matProj;

		// The Cube Texture (if this is a cube render view).
		CViewRenderTarget m_RenderTarget;

		// A mask of values representing which cube faces to skip for
		// a cube map render (default is 0, or skip no faces).
		// FIXME: NYI!
		unsigned short m_usSkipCubeFaceMask;

		// The color to clear the view with.
		unsigned long m_ulClearColor;

		// The next time the view should be rendered.
		float m_fNextRender;

		// How much time between renders.
		float m_fNextRenderDelta;

		// Whether this view is enabled or not.
		bool m_bEnabled;

		// Constructor.
		CSceneView();

		// Destructor.
		~CSceneView();

		// Render this View.
		void RenderView( bool bForceRender = false );

		// Render a normal view.
		void RenderNormalView();

		// Render to a cubemap.
		void RenderCubeRTTView();

		// Render to a Paraboloid then convolve.
		void RenderParaboloidConvolveView();

		// Render the Depth for any Entities available for this view.
		void RenderEntitiesDepth();

		// Render the Color for any Entities available for this view.
		void RenderEntitiesColor();

		// Render a Fog pass for any Entities available for this view.
		void RenderEntitiesFog();

		// Reset to default values.
		void Reset();

		// Add a Sub-View to this View. Sub-Views are always rendered before the primary view.
		void AddSubView( CSceneView *pView )
		{
			m_SubViews.push_back( pView );
			pView->m_pParentView = this;
		}

		// Clear all Sub-Views.
		void ClearSubViews()
		{
			vector< CSceneView * >::iterator iterSubView = m_SubViews.begin();
			for ( ; iterSubView != m_SubViews.end(); ++iterSubView )
			{
				(*iterSubView)->m_pParentView = NULL;
			}
			m_SubViews.clear();
		}

		// Accessors.
		DEFINE_GETSET_ACCESSOR_POINTER( CSceneView, m_pParentView, ParentView )
		DEFINE_GETSET_ACCESSOR( float, m_fNextRenderDelta, NextRenderDelta )
		DEFINE_GETSET_ACCESSOR( float, m_fNextRender, NextRender )
		DEFINE_GETSET_ACCESSOR( bool, m_bEnabled, Enabled )
	};


	// The Scene Manager.
	class CSceneManager
	{
	public:
		// Static singleton instance.
		static CSceneManager *m_pInstance;

		// The globally managed entities.
		vector< CEntity * > m_Entities;

		// The views to render for this scene.
		vector< CSceneView * > m_Views;

		// The current view recursion level.
		int m_iViewRecursionLevel;

		// The currently being rendered view.
		CSceneView *m_pCurrentView;

		// Constructor.
		CSceneManager();

		// Destructor.
		~CSceneManager();

		// Add a View to the Scene.
		void AddView( CSceneView *pView );

		// TODO: Remove this View from the Scene.
		void RemoveView( CSceneView *pView );

		// Add an entity to be managed.
		void AddEntity( CEntity *pEnt );

		// Remove this Entity from the manager.
		void RemoveEntity( CEntity *pEnt );

		// Release all the managed views.
		void ReleaseViews();

		// Release all the managed entities.
		void ReleaseEntities();

		// Release everything in the scene (views, entities, etc...).
		void ReleaseScene();

		// Render the Scene.
		void RenderScene();

		// Update the Scene.
		void UpdateScene();

		// Allow the scene view to access our privates.
		friend class CSceneView;
	};
};


#endif // __AR__SCENEMANAGER__H__