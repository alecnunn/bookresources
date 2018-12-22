#include "Advanced2D.h"

namespace Advanced2D {


Light::Light(int lightNum, D3DLIGHTTYPE type, D3DXVECTOR3 position, D3DXVECTOR3 direction, float range)
{
	this->p_lightNum = lightNum;
	ZeroMemory( &p_light, sizeof(D3DLIGHT9) );

	switch(type)
	{
		case D3DLIGHT_POINT:
			p_light.Type = D3DLIGHT_POINT;
			p_light.Diffuse.r = 1.0f;
			p_light.Diffuse.g = 1.0f;
			p_light.Diffuse.b = 1.0f;
			p_light.Position = position;
			p_light.Attenuation0 = 0.1f;
			p_light.Range = range;
			break;

		case D3DLIGHT_SPOT:
			p_light.Type = D3DLIGHT_SPOT;
			p_light.Diffuse.r = 1.0f;
			p_light.Diffuse.g = 1.0f;
			p_light.Diffuse.b = 1.0f;
			p_light.Diffuse.a = 1.0f;
			p_light.Position = position;
			p_light.Direction = direction;
			p_light.Range = range;
			p_light.Theta = 0.5f;
			p_light.Phi = 1.0f;
			p_light.Falloff = 1.0f;
			p_light.Attenuation0 = 1.0f;
			break;

		case D3DLIGHT_DIRECTIONAL:
		default:
			p_light.Type = D3DLIGHT_DIRECTIONAL;
			p_light.Diffuse.r = 1.0f;
			p_light.Diffuse.g = 1.0f;
			p_light.Diffuse.b = 1.0f;
			p_light.Diffuse.a = 1.0f;
			p_light.Range = range;
			//create a normalized direction 
			D3DXVec3Normalize( (D3DXVECTOR3*)&p_light.Direction, &direction );
			break;
	}

	//enable the light
	Show();
	Update();
}

Light::~Light(void)
{
}

void Light::Update()
{
	g_engine->getDevice()->SetLight(p_lightNum, &p_light);
}

void Light::Show()
{
	g_engine->getDevice()->LightEnable(p_lightNum,TRUE);
}

void Light::Hide()
{
	g_engine->getDevice()->LightEnable(p_lightNum,FALSE);
}



}
