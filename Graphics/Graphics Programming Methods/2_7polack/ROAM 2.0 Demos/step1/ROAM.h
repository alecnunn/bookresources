//==============================================================
//= ROAM.h
//= Original coders: Trent Polack (trent@codershq.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM 2.0 implementation
//==============================================================
#ifndef __Terrain_ROAM_H__
#define __Terrain_ROAM_H__

#include "..\\FrameWork\\Camera.h"
#include "..\\MathLib\\Vector.h"
#include "HeightMap.h"

namespace Terrain {
	//a ROAM 2.0 implementation
	class ROAM {
		DemoFramework::Camera *m_pCamera;

		float *m_pMDLevelSize;			//max midpoint displacement per level

		int m_maxLevel;

		unsigned int m_texID;			//id from glGenTextures

		unsigned int m_gridID;			//id from glGenTextures
		float m_gridTexCoords[3][3];	//texture coordinates for three verts

		int m_vertsPerFrame;			//stat variables
		int m_trisPerFrame;

		void RenderChild( int level, float *pV1, float *pV2, float *pV3 );

	public:

		void Init( int maxLevel, DemoFramework::Camera *pCamera );
		void Shutdown( void );
		void Render( void );

		//get the number of vertices rendered per frame
		int GetVertsPerFrame( void ) {
			return m_vertsPerFrame;
		}

		//get the number of triangles rendered per frame
		int GetTrisPerFrame( void ) {
			return m_trisPerFrame;
		}

	ROAM( void ) { }
	~ROAM( void ) { }
	};
}

#endif	//__Terrain_ROAM_H__