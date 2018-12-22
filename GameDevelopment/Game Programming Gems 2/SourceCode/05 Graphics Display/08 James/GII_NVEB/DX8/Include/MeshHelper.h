/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#ifndef _MeshHelper_H_
#define _MeshHelper_H_

#include <d3dx8.h>
#include <string>
#include <vector>

#include <AssociativeArray.h>
#include <ComAssociativeArray.h>
#include <SimpleString.h>
#include <RefStar.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This class represents a D3DXMESH and its associated textures and materials
// 
//  This is useful for several reasons :
//
//  1) The textures and materials are automatically cleaned up when the MeshHelper object is destroyed
// 
//  2) There is a static member of the class that tracks the meshes
// 
//  3) They can be looked up from a custom repository by name, rather than having to keep track of
//     pointers
//

class MeshHelper : public RefCounted
{
	public :
		typedef std::vector< D3DMATERIAL8 > MaterialMap;
		typedef AssociativeArray< SimpleString, RefStar< MeshHelper > >			  MeshMap;
		typedef std::vector< LPDIRECT3DTEXTURE8 > TextureVector;
		typedef ComAssociativeArray< SimpleString, LPDIRECT3DTEXTURE8 >	  TextureMap;

	private :

		static MeshMap		smMeshes;
		MaterialMap			mMaterials;
		SimpleString			mName;
		TextureMap			mTextures;
		TextureVector		mTextureVector;
		RefStar< ID3DXMesh > mpMesh;

		MeshHelper() : mpMesh( 0 )
		{
		}

		virtual ~MeshHelper()
		{
			if ( mpMesh ) mpMesh->Release(), mpMesh = 0;
		}

	public :

		static bool Erase( const SimpleString& theFileName )
		{
			return smMeshes.erase( theFileName );
		}

		static ID3DXMesh*  Lookup( const SimpleString& theFileName )
		{
			RefStar< MeshHelper > pMesh = 0;

			if ( smMeshes.find( theFileName, pMesh ) )
			{
				return pMesh->mpMesh;
			}
			return 0;
		}

		static D3DMATERIAL8* LookupMaterial( const SimpleString& theMeshName, const unsigned int& theIndex )
		{
			RefStar< MeshHelper > pMesh = 0;

			if ( smMeshes.find( theMeshName, pMesh ) )
			{
				if ( theIndex < pMesh->mMaterials.size() )
				{
					return &( pMesh->mMaterials[ theIndex ] );
				}
			}
			return 0;
		}

		static LPDIRECT3DTEXTURE8 LookupTexture( const SimpleString& theMeshName, const SimpleString& theName )
		{
			RefStar< MeshHelper > pMesh = 0;
			LPDIRECT3DTEXTURE8 pTexture = 0;

			if ( smMeshes.find( theMeshName, pMesh ) )
			{
				if ( pMesh->mTextures.find( theName, pTexture ) )
				{
					return pTexture;
				}
			}
			return 0;
		}

		static LPDIRECT3DTEXTURE8 LookupTexture( const SimpleString& theMeshName, const unsigned int& anIndex )
		{
			RefStar< MeshHelper > pMesh = 0;
			LPDIRECT3DTEXTURE8 pTexture = 0;

			if ( smMeshes.find( theMeshName, pMesh ) )
			{
				if ( anIndex < pMesh->mTextureVector.size() )
				{
					return ( pMesh->mTextureVector[ anIndex ] );
				}
			}
			return 0;
		}

		static unsigned int GetMaterialCount( const SimpleString& theMeshName ) 
		{ 
			RefStar< MeshHelper > pMesh = 0;
			if ( smMeshes.find( theMeshName, pMesh ) )
			{
				return pMesh->mMaterials.size();
			}
			return 0;
		}

		static bool LoadMesh( const SimpleString& theFileName,
							  IDirect3DDevice8*  pDevice )
		{
			Erase( theFileName );

			RefStar< MeshHelper > pCM = new MeshHelper();

			pCM->mName = theFileName;

			LPD3DXBUFFER pD3DXMtrlBuffer = 0;
			ID3DXMesh* pMesh = 0;

			DWORD theNumMaterials = 0;

			// Load the mesh from the specified file
			if( FAILED( D3DXLoadMeshFromX( const_cast< char* >( &theFileName[ 0 ] ), 
										   D3DXMESH_SYSTEMMEM, 
										   pDevice, 
										   NULL, 
										   &pD3DXMtrlBuffer, 
										   &theNumMaterials, 
										   &pMesh ) ) )
			{
				delete pCM;
				return false;
			}

			// We need to extract the material properties and texture names from the 
			// pD3DXMtrlBuffer
			D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

			LPDIRECT3DTEXTURE8 pMeshTexture = 0;

			pCM->mMaterials.reserve( theNumMaterials );
			pCM->mTextureVector.reserve( theNumMaterials );

			for( DWORD i = 0; i < theNumMaterials; ++i )
			{
				// Copy the material
				pCM->mMaterials.push_back( d3dxMaterials[i].MatD3D );

				// Set the ambient color for the material (D3DX does not do this)
				pCM->mMaterials[ i ].Ambient = pCM->mMaterials[ i ].Diffuse;
     
				if ( d3dxMaterials[ i ].pTextureFilename != 0 )
				{
					// Create the texture
					if( FAILED( D3DXCreateTextureFromFile( pDevice, 
														   d3dxMaterials[ i ].pTextureFilename, 
														   &pMeshTexture ) ) )
					{
						OutputDebugString( "Error Loading Texture\n" );
						assert( false );
						pCM->mTextureVector.push_back( 0 );
					}
					else
					{
						pCM->mTextures.insert( d3dxMaterials[ i ].pTextureFilename, pMeshTexture );
						pCM->mTextureVector.push_back( pMeshTexture );
					}
				}
				else
				{
					pCM->mTextureVector.push_back( 0 );
				}
			}

			// Done with the material buffer
			pD3DXMtrlBuffer->Release(), pD3DXMtrlBuffer = 0;

			pCM->mName = theFileName;
			pCM->mpMesh = pMesh;

			smMeshes.insert( pCM->mName, pCM );

			return true;
		}

};

/*static*/ MeshHelper::MeshMap		MeshHelper::smMeshes;


#endif  _MeshHelper_H_
