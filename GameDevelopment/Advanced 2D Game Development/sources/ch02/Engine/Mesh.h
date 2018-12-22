#pragma once

#include "Advanced2D.h"
namespace Advanced2D {
    
class Mesh
{
private:
    LPD3DXMESH mesh;

	D3DMATERIAL9* materials;
    LPD3DXMATERIAL d3dxMaterials;
    LPD3DXBUFFER matbuffer;
    DWORD material_count;

	LPDIRECT3DTEXTURE9* textures;

	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 scale;
	
    D3DXMATRIX matWorld;
    D3DXMATRIX matTranslate;
	D3DXMATRIX matRotate;
	D3DXMATRIX matScale;

public:
	Mesh(void);
	~Mesh(void);

	int GetFaceCount();
	int GetVertexCount();

	bool Load(char *filename);

	void CreateSphere(float radius = 0.1f, int slices = 10, int stacks = 10);
	void CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

	void Draw();
	void Transform();
	
	void Rotate(D3DXVECTOR3 rot);
	void Rotate(float x,float y,float z);

	void Update();
	void LimitBoundary(float left,float right,float top,float bottom,float back,float front);

	void SetPosition(D3DXVECTOR3 pos) { position = pos; }
	void SetPosition(float x,float y,float z) { position = D3DXVECTOR3(x,y,z); }
	D3DXVECTOR3 GetPosition() { return position; }

	void SetVelocity(D3DXVECTOR3 vel) { velocity = vel; }
	void SetVelocity(float x,float y,float z) { velocity = D3DXVECTOR3(x,y,z); }
	D3DXVECTOR3 GetVelocity() { return velocity; }

	void SetRotation(D3DXVECTOR3 rot) { rotation = rot; }
	void SetRotation(float x,float y,float z) { rotation = D3DXVECTOR3(x,y,z); }

	void SetRotationX(float x) { rotation.x = x; }
	float GetRotationX() { return rotation.x; }

	void SetRotationY(float y) { rotation.y = y; }
	float GetRotationY() { return rotation.y; }

	void SetRotationZ(float z) { rotation.z = z; }
	float GetRotationZ() { return rotation.z; }

	void SetScale(D3DXVECTOR3 value) { scale = value; }
	void SetScale(float x,float y,float z) { scale = D3DXVECTOR3(x,y,z); }
	D3DXVECTOR3 GetScale() { return scale; }

	void SetScaleX(float x) { scale.x = x; }
	float GetScaleX() { return scale.x; }

	void SetScaleY(float y) { scale.y = y; }
	float GetScaleY() { return scale.y; }

	void SetScaleZ(float z) { scale.z = z; }
	float GetScaleZ() { return scale.z; }
	
};
}; //namespace


