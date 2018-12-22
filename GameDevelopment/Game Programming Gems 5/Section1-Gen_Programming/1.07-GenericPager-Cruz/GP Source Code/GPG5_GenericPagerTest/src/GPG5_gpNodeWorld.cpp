// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeWorld.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeWorld Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpNodeWorld.h>

#include <GPG5_gpNodeTile.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::GPG5_gpNodeWorld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeWorld::GPG5_gpNodeWorld():
GPG5_gpNode(),

left_		(0),
right_	(0),
bottom_	(0),
top_		(0),

texturePath_ (""),

showTexture_ (false),
showGrid_		 (false)
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::~GPG5_gpNodeWorld
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpNodeWorld::~GPG5_gpNodeWorld()
{
}

// +---------------------------
// |	World Limits
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::SetWorldLimits
// | Description     : 
// |
// | Parameters : 
// |   left          : 
// |   right         : 
// |   bottom        : 
// |   top           : 
// |   tilesX        : 
// |   tilesY        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWorld::SetWorldLimits(int left, int right, int bottom, int top, int tilesX, int tilesY)
{
	left_		= left;
	right_	= right;
	bottom_ = bottom;
	top_		= top;

	//Grid
	grid_.SetName("World Grid Node");
	grid_.SetColor( GPG5_gpVector(0.4, 0.4, 0.4) );
	grid_.SetPosition( GPG5_gpVector(0.0, 0.0, 0.0) );
	grid_.SetSize(
								tilesY, tilesX, 
								(float)(top-bottom)/(float)tilesY,
								(float)(right-left)/(float)tilesX
							 );

	//Quad
	quad_.SetName("World Quad Node");
	quad_.SetColor( GPG5_gpVector(1.0, 1.0, 1.0) );
	quad_.SetPosition( GPG5_gpVector(0.0, 0.0, -1.0) );
	quad_.SetSize( (right-left), (top-bottom));
	quad_.SetFillMode( GL_FILL  );
}

// +---------------------------
// |	Texture
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::SetTexture
// | Description     : 
// |
// | Parameters : 
// |   path          : 
// |   numComponents : 
// |   totalWidth    : 
// |   totalHeight   : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWorld::SetTexture(const std::string& path, int numComponents, int totalWidth, int totalHeight)
{
	texture_.SetName("World Texture Node");
	texture_.Setup
		(	path,
			numComponents,
			totalWidth,
			totalHeight
		);
	texture_.Load();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::ShowTexture
// | Description     : 
// |
// | Parameters : 
// |   showTexture   : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWorld::ShowTexture(bool showTexture)
{
	showTexture_ = showTexture;

	if ( showTexture_  )
	{
		quad_.SetTexture(&texture_);
		quad_.EnableTexture(true);
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::ShowGrid
// | Description     : 
// |
// | Parameters : 
// |   showGrid      : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWorld::ShowGrid(bool showGrid)
{
	showGrid_ = showGrid;
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpNode
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpNodeWorld::Process()
{
	if ( showGrid_ )
	{
		grid_.ProcessNode();
	}

	quad_.ProcessNode();
}

// +--------------------------------------------------
// |	Virtual Methods of GPworld
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::IsValidIndex
// | Description     : 
// |										This methods determines is an index is 
// |										valid in this world or not.
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpNodeWorld::IsValidIndex(const GPindex& index)
{
	if (
			( ( index.GetX() >= left_) && ( index.GetX() < right_) ) &&
			( ( index.GetY() >= bottom_) && ( index.GetY() < top_) )
		 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpNodeWorld::BuildTile
// | Description     : 
// |										This methos creates a GPGP5_tile, which is
// |										a derived class of GPtile.
// |
// | Parameters : 
// |   index         : 
// | 
// +-------------------------------------------------------------
GPtile* GPG5_gpNodeWorld::BuildTile(const GPindex& index)
{
	char x[256];
	itoa(index.GetX(),x,10);

	char y[256];
	itoa(index.GetY(),y,10);

	std::string name = std::string("World_Tile_X_") + std::string(x) + std::string("_Y_") + std::string(y);
	GPG5_gpNodeTile* pTile = new GPG5_gpNodeTile();
	pTile->SetName( name );
	
	return pTile;
}
