#include "Advanced2d.h"

#ifndef _LIGHT_H
#define _LIGHT_H 1


namespace Advanced2D {


class Light
{
private:
	D3DLIGHT9 p_light;
	D3DLIGHTTYPE p_type;
	int p_lightNum;

public:
	Light(int lightNum, D3DLIGHTTYPE type, D3DXVECTOR3 position, D3DXVECTOR3 direction, double range);
	~Light(void);

	void setX(double value) { p_light.Position.x = (float)value; }
	double getX() { return p_light.Position.x; }

	void setY(double value) { p_light.Position.y = (float)value; }
	double getY() { return p_light.Position.y; }

	void setZ(double value) { p_light.Position.z = (float)value; }
	double getZ() { return p_light.Position.z; }

	D3DLIGHTTYPE getType() { return p_type; }

	void setColor(D3DCOLORVALUE color) { p_light.Diffuse = color; };
	D3DCOLORVALUE getColor() { return p_light.Diffuse; }

	void setDirection(D3DXVECTOR3 direction) { this->p_light.Direction = direction; }
	void setDirection(double x,double y,double z) 
	{ 
		setDirection(D3DXVECTOR3((float)x,(float)y,(float)z)); 
	}
	D3DXVECTOR3 getDirection() { return this->p_light.Direction; }

	void setPosition(D3DXVECTOR3 pos) { p_light.Position = pos; }
	void setPosition(double x,double y,double z) 
	{ 
		setPosition(D3DXVECTOR3((float)x,(float)y,(float)z)); 
	}
	D3DXVECTOR3 getPosition() { return p_light.Position; }


	void Update();
	void Show();
	void Hide();

};
};

#endif
