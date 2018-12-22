#include "Advanced2D.h"

namespace Advanced2D {

	Mesh::Mesh() : Entity(RENDER3D)
	{
		mesh = 0;
		materials = 0;
	    d3dxMaterials = 0;
	    matbuffer = 0;
	    material_count = 0;
		textures = 0;
		position = D3DXVECTOR3(0.0f,0.0f,0.0f);
		velocity = D3DXVECTOR3(0.0f,0.0f,0.0f);
		rotation = D3DXVECTOR3(0.0f,0.0f,0.0f);
		scale =	   D3DXVECTOR3(1.0f,1.0f,1.0f);
	}
	
	Mesh::~Mesh(void)
	{
	    if (materials != NULL) delete[] materials;
	
	    //remove textures from memory
	    if (textures != NULL) {
	        for( DWORD i = 0; i < material_count; i++)
	        {
	            if (textures[i] != NULL)
	                textures[i]->Release();
	        }
	        delete[] textures;
	    }
	    
	    if (mesh != NULL) mesh->Release();
	
	}
	
	int Mesh::GetFaceCount()
	{
		return this->mesh->GetNumFaces();
	}
	
	int Mesh::GetVertexCount()
	{
		return this->mesh->GetNumVertices();
	}
	
	
	bool Mesh::Load(char* filename)
	{
	    HRESULT result;
	
	    //load mesh from the specified file
	    result = D3DXLoadMeshFromX(
	        filename,               //filename
	        D3DXMESH_SYSTEMMEM,     //mesh options
	        g_engine->getDevice(),  //Direct3D device
	        NULL,                   //adjacency buffer
	        &matbuffer,             //material buffer
	        NULL,                   //special effects
	        &material_count, 		//number of materials
	        &mesh);          		//resulting mesh
	
	    if (result != D3D_OK)  {
	        return false;
	    }
	
	    //extract material properties and texture names from material buffer
	    d3dxMaterials = (LPD3DXMATERIAL)matbuffer->GetBufferPointer();
	    materials = new D3DMATERIAL9[material_count];
	    textures  = new LPDIRECT3DTEXTURE9[material_count];
	
	    //create the materials and textures
	    for(DWORD i=0; i < material_count; i++)
	    {
	        //grab the material
	        materials[i] = d3dxMaterials[i].MatD3D;
	
	        //set ambient color for material 
	        materials[i].Ambient = materials[i].Diffuse;
	        //materials[i].Emissive = materials[i].Diffuse;
	        materials[i].Power = 0.5f;
	        //materials[i].Specular = materials[i].Diffuse;
	
	        textures[i] = NULL;
	        if( d3dxMaterials[i].pTextureFilename != NULL && 
	            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
	        {
	            //load texture file specified in .x file
				result = D3DXCreateTextureFromFile(g_engine->getDevice(), d3dxMaterials[i].pTextureFilename, &textures[i]);
	
	            if (result != D3D_OK) {
	                return false;
	            }
	        }
	    }
	
	    //done using material buffer
	    matbuffer->Release();
	
		return true;
	}
	
	
	void Mesh::CreateSphere(double radius, int slices, int stacks)
	{
		D3DXCreateSphere(g_engine->getDevice(), (float)radius, slices, stacks, &mesh, NULL);
	}
	
	void Mesh::CreateCube(double width, double height, double depth)
	{
		D3DXCreateBox(g_engine->getDevice(), (float)width, (float)height, (float)depth, &mesh, NULL);
	}
	

	void Mesh::draw()
	{
		Transform();

		if (material_count == 0) {
			mesh->DrawSubset(0);
		}
		else {
			//draw each mesh subset
			for( DWORD i=0; i < material_count; i++ )
			{
				// Set the material and texture for this subset
				g_engine->getDevice()->SetMaterial( &materials[i] );
	
				if (textures[i]) 
				{
					if (textures[i]->GetType() == D3DRTYPE_TEXTURE) 
					{
						D3DSURFACE_DESC desc;
						textures[i]->GetLevelDesc(0, &desc);
						if (desc.Width > 0) {
							g_engine->getDevice()->SetTexture( 0, textures[i] );
						}
					}
				}
				
				// Draw the mesh subset
				mesh->DrawSubset( i );
			}
		}
	}
	
	void Mesh::Transform()
	{
		//set rotation matrix
		double x = D3DXToRadian(rotation.x);
		double y = D3DXToRadian(rotation.y);
		double z = D3DXToRadian(rotation.z);
		D3DXMatrixRotationYawPitchRoll(&matRotate, (float)x, (float)y, (float)z);
	
		//set scaling matrix
		D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);
	
		//set translation matrix
	    D3DXMatrixTranslation(&matTranslate, position.x, position.y, position.z);
	
		//transform the mesh
		matWorld = matRotate * matScale * matTranslate;
	    g_engine->getDevice()->SetTransform(D3DTS_WORLD, &matWorld);
	}
	
	void Mesh::Rotate(D3DXVECTOR3 rot)
	{
		Rotate(rot.x,rot.y,rot.z);
	}
	
	void Mesh::Rotate(double x,double y,double z)
	{
		rotation.x += (float)x;
		rotation.y += (float)y;
		rotation.z += (float)z;
	}
	
	void Mesh::move()
	{
		position.x += velocity.x;
	    position.y += velocity.y;
	    position.z += velocity.z;
	}
	
	void Mesh::LimitBoundary(double left,double right,double top,double bottom,double back,double front) 
	{
	    if (position.x < left || position.x > right) {
	        velocity.x *= -1;
	    }
	    if (position.y < bottom || position.y > top) {
	        velocity.y *= -1;
	    }
	    if (position.z < front || position.z > back) {
	        velocity.z *= -1;
	    }
	}

};

