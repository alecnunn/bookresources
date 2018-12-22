//==============================================================
//= CROAM.h
//= Original coders: Trent Polack (trent@codershq.com) and
//==============================================================
//= The header file for the ROAM 2.0 implementation
//==============================================================
#ifndef __Terrain_ROAM_H__
#define __Terrain_ROAM_H__

#include "..\\Framework\\Camera.h"
#include "..\\Framework\\Texture.h"

#include "HeightMap.h"

namespace Terrain {
	//frustum culling constants (bitmasks)
	#define CULL_ALLIN 0x3f
	#define CULL_OUT   0x40

	//64-bit integer type defines (for better multiplatform support)
	#ifdef _WIN32
		typedef signed   __int64 ROAMint64;
		typedef unsigned __int64 ROAMuint64;
	#else
		typedef signed   long long int ROAMint64;
		typedef unsigned long long int ROAMuint64;
	#endif

	//ROAM diamond structure
	struct ROAMDiamond {
		ROAMDiamond *m_pParent[4];					//diamond's parents (two corners)
		ROAMDiamond *m_pChild[4];					//diamond's kids
		ROAMDiamond *m_pPrevDmnd, *m_pNextDmnd;		//prev and next links on queue or free list

		float m_vert[3];							//vertex position
		float m_bound;								//radius of the diamond's bounding sphere
		float m_error;								//radius of pointwise error squared

		signed char m_childIndex[2];				//our kid index within each of our parents
		signed char m_level;						//level of resolution

		unsigned char m_lockCount;					//number of references (if 0, the diamond is free for use)
		unsigned char m_flags;
		unsigned char m_cull;
	};

	//CROAM, ROAM 2.0 implementation
	class ROAM {
		ROAMDiamond *m_pFreeDmnd[2];					//most recently unlocked diamonds
		ROAMDiamond *m_pLevel0Dmnd[3][3];				//base diamonds level 0
		ROAMDiamond *m_pLevel1Dmnd[4][4];				//base diamonds level -1,-2...

		ROAMDiamond *m_pDmndPool;						//diamond storage pool
		int m_poolSize;									//pool size (number of diamonds)

		float *m_pMDLevelSize;							//max midpoint displacement per level
		int m_maxLevel;

		DemoFramework::Texture m_texture;		

		HeightMap m_heightMap;							//the height data
		int m_size;

		float m_frustum[6][4];
		float m_camX, m_camY, m_camZ;

		int m_vertsPerFrame;							//stat variables
		int m_trisPerFrame;

		void RenderChild( ROAMDiamond *pDmnd, int i, int cull );

		ROAMDiamond *Create( void );
		ROAMDiamond *GetChild( ROAMDiamond *pDmnd, int i );

		void Lock( ROAMDiamond *pDmnd );
		void Unlock( ROAMDiamond *pDmnd );

		//shift terrain coordinates from [-3, 3] to [0, m_size-1]
		inline void ShiftCoords( float* x, float* z )
		{
			*x= ( *x+1.0f )/2.0f;	//shift X into [0, 2] then divide by two shift into [0...1]
			*z= ( *z+1.0f )/2.0f;	//shift Z into [0, 2] then divide by two shift into [0...1]

			*x*= m_size;			//translate into map-coords
			*z*= m_size;			//translate into map-coords
		}

	public:


		void Init( int maxLevel, int poolSize, DemoFramework::Camera *pCamera );
		void Shutdown( void );
		void Render( void );

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

		//update the class's frustum/camera information
		inline void SetFrustum( DemoFramework::Camera camera ) {
			for( int i= 0; i < 6; i++ ) {
				for( int j= 0; j < 4; j++ )
					m_frustum[i][j]= camera.m_viewFrustum[i][j];
			}

			m_camX= camera.m_eyePos[0];
			m_camY= camera.m_eyePos[1];
			m_camZ= camera.m_eyePos[2];
		}

		//Gets the number of vertices rendered per frame
		inline int GetVertsPerFrame( void ) {
			return m_vertsPerFrame;
		}

		//Gets the number of triangles rendered per frame
		inline int GetTrisPerFrame( void ) {
			return m_trisPerFrame;
		}

		ROAM( void ) { }
		~ROAM( void ) { }
	};
}

#endif	//__Terrain_ROAM_H__