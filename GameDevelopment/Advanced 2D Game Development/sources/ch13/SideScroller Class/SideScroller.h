#include "..\Engine\Advanced2D.h"
using namespace Advanced2D;

#pragma once

class SideScroller
{
private:
	double scrollx;
	Texture *image;
	Rect viewport;
	D3DCOLOR color;

public:
	SideScroller(void);
	virtual ~SideScroller(void);
	bool load(std::string filename);
	void setViewport(Rect rect) { viewport = rect; }
	void setViewport(double left,double top,double right,double bottom);
	void setColor(D3DCOLOR col) { color = col; }
	void update(double velocity);
	void draw();
	double getScrollPosition() { return scrollx; }
};

