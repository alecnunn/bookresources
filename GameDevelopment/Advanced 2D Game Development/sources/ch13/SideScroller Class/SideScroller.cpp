#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#include "SideScroller.h"

SideScroller::SideScroller(void)
{
	scrollx = 0.0;
	image = NULL;
	color = 0xFFFFFFFF;
	viewport.left = 0;
	viewport.top = 0;
	viewport.right = 10;
	viewport.bottom = 10;
}

SideScroller::~SideScroller(void)
{
	if (image) delete image;
}

void SideScroller::setViewport(double left,double top,double right,double bottom)
{
	viewport.left = left;
	viewport.top = top;
	viewport.right = right;
	viewport.bottom = bottom;
}


bool SideScroller::load(std::string filename)
{
	if (image) delete image;
	image = new Texture();
	return image->Load(filename);
}


void SideScroller::update(double velocity)
{
	scrollx += velocity;
}


void SideScroller::draw()
{
	//set up the viewport
	double viewportX = viewport.left;
	double viewportY = viewport.top;
	double viewportW = viewport.getWidth();
	double viewportH = viewport.getHeight();

	//image is larger than viewport
	if (image->getWidth() < viewportW) return;
	
	//determine how to draw the image
	if (scrollx > image->getWidth()) scrollx = 0;
	if (scrollx < 0) scrollx = image->getWidth();

	int x = (int)scrollx;
	int y = 0;
	int w = x + (int)viewportW;
	int h = y + (int)viewportH;

	RECT src1 = {x, y, w, h};
	D3DXVECTOR3 pos1((float)viewportX,(float)viewportY,0.0);
	g_engine->getSpriteHandler()->Draw(image->GetTexture(),&src1,NULL,&pos1,color);

	double leftover = image->getWidth()- viewport.getWidth();
	if (scrollx > leftover)
	{
		x = 0; y = 0;
		w = (int)(scrollx-leftover);
		h = (int)(y+viewportH);
		RECT src2 = {x, y, w, h};
		D3DXVECTOR3 pos2((float)(viewportX+viewportW-w),(float)viewportY,0.0);
		g_engine->getSpriteHandler()->Draw(image->GetTexture(),&src2,NULL,&pos2,color);
	}
}
