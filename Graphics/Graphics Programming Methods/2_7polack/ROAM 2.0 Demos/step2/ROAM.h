//==============================================================
//= ROAM.h
//= Original coders: Trent Polack (trent@codershq.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM 2.0 implementation
//==============================================================
#ifndef __Terrain_ROAM_H__
#define __Terrain_ROAM_H__

#include "..\\Framework\\Camera.h"
#include "..\\Framework\\Texture.h"
#include "..\\MathLib\\Vector.h"
#include "HeightMap.h"

namespace Terrain {
	//frustum culling constants
	#define CULL_ALLIN 0x3f
	#define CULL_OUT   0x40

	//a ROAM 2.0 implementation
	class ROAM {
		DemoFramework::Camera *m_pCamera;

		float *m_pMDLevelSize;			//max midpoint displacement per level

		int m_maxLevel;

		HeightMap m_heightMap;
		int m_size;

		DemoFramework::Texture m_texture;

		float m_frustum[6][4];
		float m_camX, m_camY, m_camZ;

		int m_vertsPerFrame;			//stat variables
		int m_trisPerFrame;

		void RenderChild( int level, float *pV1, float *pV2, float *pV3, int cull );

	public:

		void Init( int maxLevel, DemoFramework::Camera *pCamera );
		void Shutdown( void );
		void Render( void );

		void SetFrustum( DemoFramework::Camera camera ) {
			for( int i= 0; i < 6; i++ ) {
				for( int j= 0; j < 4; j++ )
					m_frustum[i][j]= camera.m_viewFrustum[i][j];
			}

			m_camX= camera.m_eyePos[0];
			m_camY= camera.m_eyePos[1];
			m_camZ= camera.m_eyePos[2];
		}

		bool LoadDataSet( char *filename, int size ) {
			m_size= size;
			return( m_heightMap.Load( filename, size ) );
		}

		void ReleaseDataSet( void ) {
			m_heightMap.Release( );
		}

		bool LoadTexture( const char *filename ) {
			return( m_texture.Load( filename, GL_LINEAR, GL_LINEAR, true ) );
		}

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