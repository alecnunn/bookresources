#include "Advanced2D.h"

#ifndef _CAMERA_H
#define _CAMERA_H 1

//***mod--floats to doubles

class Camera
{
private:
    D3DXMATRIX p_matrixProj;
    D3DXMATRIX p_matrixView;
    D3DXVECTOR3 p_updir;

	D3DXVECTOR3 p_position;
	D3DXVECTOR3 p_target;

	double p_nearRange;
	double p_farRange;
	double p_aspectRatio;
	double p_fov;

public:
	Camera(void);
	~Camera(void);

	void setPerspective(double fov, double aspectRatio, double nearRange, double farRange);
	double getNearRange() { return p_nearRange; }
	void setNearRange(double value) { p_nearRange = value; }
	double getFarRange() { return p_farRange; }
	void setFarRange(double value) { p_farRange = value; }
	double getAspectRatio() { return p_aspectRatio; }
	void setAspectRatio(double value) { p_aspectRatio = value; }
	double getFOV() { return p_fov; }
	void setFOV(double value) { p_fov = value; }
	void Update();

	D3DXVECTOR3 getPosition() { return p_position; }
	void setPosition(double x, double y, double z);
	void setPosition(D3DXVECTOR3 position);
	double getX() { return p_position.x; }
	void setX(double value) { p_position.x = (float)value; }
	double getY() { return p_position.y; }
	void setY(double value) { p_position.y = (float)value; }
	double getZ() { return p_position.z; }
	void setZ(double value) { p_position.z = (float)value; }

	D3DXVECTOR3 getTarget() { return p_target; }
	void setTarget(D3DXVECTOR3 value) { p_target = value; }
	void setTarget(double x, double y, double z) 
	{ 
		p_target.x = (float)x; 
		p_target.y = (float)y; 
		p_target.z = (float)z; 
	}


};

#endif
