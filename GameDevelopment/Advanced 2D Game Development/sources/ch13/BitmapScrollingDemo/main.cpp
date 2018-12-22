/*
  Name: BitmapScrolling
  Date: 04/2008
  Description: Demonstrates bitmap-based scrolling
*/
#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

Font *font;
bool blnConsoleFlag = false;

Texture *image;
Sprite *scrollwindow;
Sprite *fullsize;

double velx=0.0, vely=0.0;
double scrollx=0.0, scrolly=0.0;


bool game_preload() 
{
	g_engine->setAppTitle("BITMAP SCROLLING DEMO");
	g_engine->setScreenWidth(1024);
	g_engine->setScreenHeight(768);
	g_engine->setColorDepth(32);
	g_engine->setFullscreen(false);
	return true;
}

bool game_init(HWND) 
{
	//load the Verdana10 font
	font = new Font();
	if (!font->loadImage("verdana10.tga")) {
		g_engine->message("Error loading verdana10.tga");
		return false;
	}
	if (!font->loadWidthData("verdana10.dat")) {
		g_engine->message("Error loading verdana10.dat");
		return false;
	}
	font->setColumns(16);
	font->setCharSize(20,16);


	//load the background image
	image = new Texture();
	image->Load("inferno512.bmp");
	fullsize = new Sprite();
	fullsize->setImage(image);

	//load the transparent scroll window
	scrollwindow = new Sprite();
	scrollwindow->loadImage("scrollwindow.tga");


    return true;
}

void scroll(Rect rect)
{
	//set up the viewport
	double viewportX = rect.left;
	double viewportY = rect.top;
	double viewportW = rect.getWidth();
	double viewportH = rect.getHeight();

	//image is larger than viewport
	if (image->getWidth() > viewportW)
	{
		//determine how to draw the image
		if (scrollx > image->getWidth()) scrollx = 0;
		if (scrollx < 0) scrollx = image->getWidth();

		int x = scrollx;
		int y = 0;
		int w = x+viewportW;
		int h = y+viewportH;

		RECT src1 = {x, y, w, h};
		D3DXVECTOR3 pos1(viewportX,viewportY,0.0);
		g_engine->getSpriteHandler()->Draw(image->GetTexture(),&src1,NULL,&pos1,0xFFFFFFFF);

		double leftover = image->getWidth()- rect.getWidth();
		if (scrollx > leftover)
		{
			x = 0; y = 0;
			w = scrollx-leftover;
			h = y+viewportH;
			RECT src2 = {x, y, w, h};
			D3DXVECTOR3 pos2(viewportX+viewportW-w,viewportY,0.0);
			g_engine->getSpriteHandler()->Draw(image->GetTexture(),&src2,NULL,&pos2,0xFFFFFFFF);
		}
	}
}


void game_update() 
{ 
}

void game_render2d()
{
	scrollx += velx;
	scrolly += vely;

	fullsize->draw();

	int w = 400, h = 300;
	int x = 1020-w, y = 764-h;
	Rect rect(x, y, x+w, y+h);
	scroll(rect);


	std::ostringstream ostr;
	ostr << "Scroll: " << scrollx << "," << scrolly;
	font->Print(800,1,ostr.str());

	ostr.str("");
	ostr << "Velocity: " << velx << "," << vely;
	font->Print(800,20,ostr.str());

	ostr.str("");
	ostr << "Image size: " << image->getWidth() << "," << image->getHeight();
	font->Print(800,40,ostr.str());

	ostr.str("");
	ostr << "Viewport size: " << rect.getWidth() << "," << rect.getHeight();
	font->Print(800,60,ostr.str());

	ostr.str("");
	ostr << "Scroll range: " << image->getWidth()- rect.getWidth() << "," << image->getHeight()- rect.getHeight();
	font->Print(800,80,ostr.str());

}

void game_end() 
{
	delete font;
	delete fullsize;
}

void game_keyPress(int key) 
{ 
	double x;
	switch (key) {
		case DIK_LEFT:
			velx -= 0.05;
			break;

		case DIK_RIGHT:
			velx += 0.05;
			break;
	}

}

void game_keyRelease(int key) 
{ 
	switch (key) {
		case DIK_ESCAPE:
			g_engine->Close();
			break;
	}
}

void game_render3d()
{
    g_engine->ClearScene(D3DCOLOR_XRGB(0,0,80));
}

void game_mouseButton(int button) { }
void game_mouseMotion(int x,int y) { }
void game_mouseMove(int x,int y) { }
void game_mouseWheel(int wheel) { }
void game_entityRender(Advanced2D::Entity* entity) { }
void game_entityUpdate(Advanced2D::Entity* entity) { }
void game_entityCollision(Advanced2D::Entity* entity1,Advanced2D::Entity* entity2) { }
