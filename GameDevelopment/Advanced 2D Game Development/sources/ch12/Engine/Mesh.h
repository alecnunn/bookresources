#include "Advanced2D.h"
#include "Entity.h"

#pragma once


//***mod--floats to doubles

namespace Advanced2D {

	class Mesh : public Entity
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
		void move();
		void animate() { }
		void draw();
	
		int GetFaceCount();
		int GetVertexCount();
	
		bool Load(char *filename);
	
		void CreateSphere(double radius = 0.1f, int slices = 10, int stacks = 10);
		void CreateCube(double width = 1.0f, double height = 1.0f, double depth = 1.0f);
	
		void Draw();
		void Transform();
		
		void Rotate(D3DXVECTOR3 rot);
		void Rotate(double x,double y,double z);
	
		void Update();
		void LimitBoundary(double left,double right,double top,double bottom,double back,double front);
	
		void SetPosition(D3DXVECTOR3 pos) { position = pos; }
		void SetPosition(double x,double y,double z) 
		{ 
			position = D3DXVECTOR3((float)x,(float)y,(float)z); 
		}
		D3DXVECTOR3 GetPosition() { return position; }
	
		void SetVelocity(D3DXVECTOR3 vel) { velocity = vel; }
		void SetVelocity(double x,double y,double z) 
		{ 
			velocity = D3DXVECTOR3((float)x,(float)y,(float)z); 
		}
		D3DXVECTOR3 GetVelocity() { return velocity; }
	
		void SetRotation(D3DXVECTOR3 rot) { rotation = rot; }
		void SetRotation(double x,double y,double z) 
		{ 
			rotation = D3DXVECTOR3((float)x,(float)y,(float)z); 
		}
	
		void SetRotationX(double x) { rotation.x = (float)x; }
		double GetRotationX() { return rotation.x; }
	
		void SetRotationY(double y) { rotation.y = (float)y; }
		double GetRotationY() { return rotation.y; }
	
		void SetRotationZ(double z) { rotation.z = (float)z; }
		double GetRotationZ() { return rotation.z; }
	
		void SetScale(D3DXVECTOR3 value) { scale = value; }
		void SetScale(double x,double y,double z) 
		{ 
			scale = D3DXVECTOR3((float)x,(float)y,(float)z); 
		}
		D3DXVECTOR3 GetScale() { return scale; }
	
		void SetScaleX(double x) { scale.x = (float)x; }
		double GetScaleX() { return scale.x; }
	
		void SetScaleY(double y) { scale.y = (float)y; }
		double GetScaleY() { return scale.y; }
	
		void SetScaleZ(double z) { scale.z = (float)z; }
		double GetScaleZ() { return scale.z; }
		
	};

};
