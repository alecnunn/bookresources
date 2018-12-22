//==============================================================
//= ROAM.h
//= Original coders: Trent Polack (trent@codershq.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM implementation
//==============================================================
#ifndef __Terrain_ROAM_H__
#define __Terrain_ROAM_H__

#include "..\\Framework\\Camera.h"
#include "..\\Framework\\Texture.h"

#include "..\\MathLib\\Vector.h"

#include "HeightMap.h"

namespace Terrain {
	#define IQMAX         4096		//number of buckets in priority queue
	#define TRI_IMAX     65536		//number of triangle-chunk slots
	#define DMCHECKLISTN 65536		//check list size

	//frustum bitmasks
	#define CULL_ALLIN 0x3f
	#define CULL_OUT   0x40

	//misc. diamond flags
	#define ROAM_SPLIT    0x01
	#define ROAM_TRI0     0x04
	#define ROAM_TRI1     0x08
	#define ROAM_CLIPPED  0x10
	#define ROAM_SPLITQ   0x40
	#define ROAM_MERGEQ   0x80
	#define ROAM_ALLQ     0xc0
	#define ROAM_UNQ      0x00

	//parent/child split flags
	#define SPLIT_K0      0x01
	#define SPLIT_K       0x0f
	#define SPLIT_P0      0x10
	#define SPLIT_P       0x30

	struct ROAMDiamond {
		ROAMDiamond *m_pParent[4];					//diamond's parents (two corners)
		ROAMDiamond *m_pChild[4];					//diamond's kids
		ROAMDiamond *m_pPrevDmnd, *m_pNextDmnd;		//prev and next links on queue or free list

		float m_fVert[3];							//vertex position
		float m_fBoundRad;							//radius of the diamond's bounding sphere
		float m_fErrorRad;							//radius of pointwise error squared

		short m_sQueueIndex;
		unsigned short m_usTriIndex[2];

		char m_cChildIndex[2];						//our kid index within each of our parents
		char m_cLevel;								//level of resolution
		unsigned char m_ucLockCount;				//number of references (if 0, the diamond is free for use)
		unsigned char m_ucFrameCount;
		unsigned char m_ucCull;
		unsigned char m_ucFlags;
		unsigned char m_ucSplitFlags;
		unsigned char m_ucPadding[3];
	};


	//--------------------------------------------------------------
	//--------------------------------------------------------------
	//- CLASS ------------------------------------------------------
	//--------------------------------------------------------------
	//--------------------------------------------------------------
	class ROAM
	{
		float m_frustum[6][4];
		float m_camX, m_camY, m_camZ;

		ROAMDiamond *m_pDmndPool;			//diamond storage pool
		int m_poolSize;						//pool size (number of diamonds)

		ROAMDiamond *m_pFreeDmnd[2];		//most recently unlocked diamonds
		ROAMDiamond *m_pLevel0Dmnd[4][4];	//base diamonds level 0
		ROAMDiamond *m_pLevel1Dmnd[4][4];	//base diamonds level -1,-2...

		ROAMDiamond *m_pSplitQueue[IQMAX];	//split priority queue
		ROAMDiamond *m_pMergeQueue[IQMAX];	//merge priority queue
		int m_PQMin, m_PQMax;				//min/max occupied bucket

		int *m_PDmndIS;						//packed diamond index and side

		int m_freeElements;					//number of elements on free list
		int m_frameCount;					//current frame count
		int m_qCoarse;						//coarseness limit on priority index
		int m_maxTriChunks;					//max number of tri chunks allowed
		int m_freeTriCount;					//number of triangles on free list
		int m_maxTris;						//target triangle count
		int m_freeTri;						//first free tri chunk

		int m_log2Table[256];				//correction to float->int conversions

		float *m_buffer;

		float *m_pLevelSize;				//max midpoint displacement per level
		int m_maxLevel;

		HeightMap m_heightMap;
		int m_size;

		MathLib::Vector m_scale;

		DemoFramework::Texture m_textureMap;

		int m_vertsPerFrame, m_trisPerFrame;

		//--------------------------------------------------------------
		// Name:		 CROAM::ShiftCoords - private
		// Description:  Scale an (x, z) vertex
		// Arguments:	 -x, z: coordinates to shift
		// Return Value: None
		//--------------------------------------------------------------
		inline void ShiftCoords( float *x, float *z ) {
			*x= ( *x+1.0f )/2.0f;	//shift X into [0, 2] then divide by two shift into [0...1]
			*z= ( *z+1.0f )/2.0f;	//shift Z into [0, 2] then divide by two shift into [0...1]

			*x*= m_size;			//translate into map-coords
			*z*= m_size;			//translate into map-coords
		}

		ROAMDiamond *Create( void );
		ROAMDiamond *GetChild( ROAMDiamond *pDmnd, int iIndex );

		void Lock( ROAMDiamond *pDmnd );
		void Unlock( ROAMDiamond *pDmnd );

		void AllocateTri( ROAMDiamond *dm, int j );
		void FreeTri( ROAMDiamond *dm, int j );
		void AddTri( ROAMDiamond *dm, int j );
		void RemoveTri( ROAMDiamond *dm, int j );

		void UpdateDiamondTri( ROAMDiamond *dm,int j );
		void UpdateChildCull( ROAMDiamond *dm );
		void Split( ROAMDiamond *dm );
		void Merge( ROAMDiamond *dm );
		void UpdateCull( ROAMDiamond *dm );
		void UpdatePriority( ROAMDiamond *dm );
		void Enqueue( ROAMDiamond *dm,int qflags,int iq_new );

		public:


		void Init( int maxLevel, int poolSize, DemoFramework::Camera *pCamera );
		void Shutdown( void );

		void Update( void );
		void Render( void );

		bool LoadDataSet( char *filename, int size ) {
			m_size= size;
			return( m_heightMap.Load( filename, size ) );
		}

		void ReleaseDataSet( void ) {
			m_heightMap.Release( );
		}

		bool LoadTexture( const char *filename ) {
			return( m_textureMap.Load( filename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true ) );
		}

		//update the class's frustum/camera information
		void SetFrustum( DemoFramework::Camera camera ) {
			for( int i= 0; i < 6; i++ ) {
				for( int j= 0; j < 4; j++ )
					m_frustum[i][j]= camera.m_viewFrustum[i][j];
			}

			m_camX= camera.m_eyePos[0];
			m_camY= camera.m_eyePos[1];
			m_camZ= camera.m_eyePos[2];
		}

		//set the scale for the engine
		void SetScale( float x, float y, float z ) {
			m_scale.Set( x, y, z );
		}

		void SetMaxTris( int maxTris ) {
			m_maxTris= maxTris;
		}

		//Gets the number of vertices rendered per frame
		int GetVertsPerFrame( void ) {
			return m_vertsPerFrame;
		}

		//Gets the number of triangles rendered per frame
		int GetTrisPerFrame( void ) {
			return m_trisPerFrame;
		}

	ROAM( void ) : m_scale( 1.0f, 1.0f, 1.0f ), m_maxTris( 30000 ) {
	}
	~ROAM( void ) {
		if( m_textureMap.IsLoaded( ) )	m_textureMap.Release( );
	}
	};
}

#endif	//__Terrain_ROAM_H__