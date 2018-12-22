// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeTile.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeTile Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeTile.h>

#include <GPG5_gpNodeManager.h>
#include <GPG5_gpNodeTexture.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTile::GPG5_gpNodeTile
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeTile::GPG5_gpNodeTile():
GPtile()
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTile::~GPG5_gpNodeTile
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeTile::~GPG5_gpNodeTile()
{
}

// +--------------------------------------------------
// |	Virtual Methods of GPtile
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTile::Load
// | Description     : 
// |									This methods loads the information which is
// |									delimited by its GPindex
// |			
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTile::Load()
{
	SetColor( GPG5_gpVector(1.0, 1.0, 1.0) );
	SetPosition( GPG5_gpVector
					(
						index_.GetX(), 
						index_.GetY(), 
						0.0) 
					);
	SetSize(index_.GetWidth(), index_.GetHeight() );

	SetFillMode( GL_FILL  );


	GPG5_gpNodeTexture* pNodeTexture = new GPG5_gpNodeTexture();
	pNodeTexture->SetName("Texture Node");
	pNodeTexture->Setup
		(
			".\\Data\\loaded_world_3x2000x1000.raw",
			3,
			2000,
			1000
		);

	pNodeTexture->Load(
											index_.GetX(), index_.GetX()+index_.GetWidth()-1, //X
											1000 - (index_.GetY()+index_.GetHeight()), (1000- (index_.GetY())) - 1//Y
										);
	SetTexture(pNodeTexture);
	EnableTexture(true);
	
	GPG5_gpNodeManager::GetInstance().AddNode(this);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeTile::Unload
// | Description     : 
// |									This methods unsloads the information which has
// |									been loaded previously
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeTile::Unload()
{
	GPG5_gpNodeManager::GetInstance().RemoveNode(this);
	
	GPG5_gpNodeManager::GetInstance().AddToDestroyList( GetTexture() );
	
	EnableTexture(false);
	SetTexture(NULL);
}


