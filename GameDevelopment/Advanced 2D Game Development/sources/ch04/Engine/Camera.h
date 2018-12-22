#include "Advanced2D.h"

#ifndef _CAMERA_H
#define _CAMERA_H 1

namespace Advanced2D {

class Camera
{
private:
    D3DXMATRIX p_matrixProj;
    D3DXMATRIX p_matrixView;
    D3DXVECTOR3 p_updir;

	D3DXVECTOR3 p_position;
	D3DXVECTOR3 p_target;

	float p_nearRange;
	float p_farRange;
	float p_aspectRatio;
	float p_fov;

public:
	Camera(void);
	~Camera(void);

	void setPerspective(float fov, float aspectRatio, float nearRange, float farRange);
	float getNearRange() { return p_nearRange; }
	void setNearRange(float value) { p_nearRange = value; }
	float getFarRange() { return p_farRange; }
	void setFarRange(float value) { p_farRange = value; }
	float getAspectRatio() { return p_aspectRatio; }
	void setAspectRatio(float value) { p_aspectRatio = value; }
	float getFOV() { return p_fov; }
	void setFOV(float value) { p_fov = value; }
	void Update();

	D3DXVECTOR3 getPosition() { return p_position; }
	void setPosition(float x, float y, float z);
	void setPosition(D3DXVECTOR3 position);
	float getX() { return p_position.x; }
	void setX(float value) { p_position.x = value; }
	float getY() { return p_position.y; }
	void setY(float value) { p_position.y = value; }
	float getZ() { return p_position.z; }
	void setZ(float value) { p_position.z = value; }

	D3DXVECTOR3 getTarget() { return p_target; }
	void setTarget(D3DXVECTOR3 value) { p_target = value; }
	void setTarget(float x, float y, float z) { p_target.x = x; p_target.y = y; p_target.z = z; }

};

};

#endif
