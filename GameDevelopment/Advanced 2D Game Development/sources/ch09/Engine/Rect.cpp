#include "Advanced2D.h"


namespace Advanced2D {

	Rect::Rect(int left,int top,int right,int bottom)
	{
		this->left = (double)left;
		this->top = (double)top;
		this->right = (double)right;
		this->bottom = (double)bottom;
	}

	Rect::Rect(double left,double top,double right,double bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}

	bool Rect::isInside(Vector3 point)
	{
		return this->isInside(point.getX(), point.getY());
	}

	bool Rect::isInside(int x,int y)
	{
		return this->isInside((double)x, (double)y);
	}

	bool Rect::isInside(double x,double y)
	{
		return (x > left && x < right && y > top && y < bottom);
	}

};
