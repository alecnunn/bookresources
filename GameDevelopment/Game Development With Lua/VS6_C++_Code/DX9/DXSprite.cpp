#include "stdafx.h"
#include <WinConsole.h>

#define SPRITE_VERTS (4)

static const unsigned int SpriteVertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct	SpriteVertex
{
	float		x, y, z, rhw;
	D3DCOLOR	diffuse;
	float		u0, v0;
};



Sprite::Sprite()
{
	m_vb = NULL;
	m_tex = NULL;
	m_centerX = m_centerY = 0.0f;
	m_rot = 0.0f;
	m_isDirty = true;
}

Sprite::~Sprite()
{
	SAFE_RELEASE(m_vb);
	SAFE_RELEASE(m_tex);
}


bool Sprite::SetTexture(const char *filename)
{
	if(!m_vb)
	{
		HRESULT ret = DXBase::GetInstance()->Get3DDevice()->CreateVertexBuffer(sizeof(SpriteVertex)*SPRITE_VERTS, 0, SpriteVertexFVF, D3DPOOL_MANAGED, &m_vb, NULL);
		if (D3D_OK != ret)
		{
			char buf[1024];
			sprintf(buf, "Failed to Create Vertex Buffer:%s Error:0x%x\n", filename, ret);
			CWinConsole::Write(buf);
			return false;
		}
	}

	SAFE_RELEASE(m_tex);

	// prepend the scripts directory name to the filename for loading
	char path[254];
	sprintf(path, "Textures\\%s", filename);
	HRESULT hr;
	if(FAILED(hr = D3DXCreateTextureFromFileEx(DXBase::GetInstance()->Get3DDevice(), path, 0, 0, 0, 0,
                            D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
                            D3DX_DEFAULT , 0, NULL, NULL, &m_tex)))
	{
		char buf[1024];
		sprintf(buf, "Failed to load Texture:%s Error:0x%x\n", path, hr);
		CWinConsole::Write(buf);
		return false;
	}


	return true;

}

void Sprite::UpdatePosition(void)
{
	if(m_vb)
	{
		SpriteVertex* pVertices;
		m_vb->Lock(0, 0, (void **)&pVertices, 0);
		// since there are not 6 verts for a sprite, assume it is a triangle stip
		// with 4 verts in a specific order.
		pVertices[0].diffuse = pVertices[1].diffuse = pVertices[2].diffuse = pVertices[3].diffuse = D3DCOLOR_ARGB(255,255,255,255);
		pVertices[0].rhw = pVertices[1].rhw = pVertices[2].rhw = pVertices[3].rhw = 1.0f;

		pVertices[0].u0		= 0.0f;
		pVertices[0].v0		= 0.0f;
		pVertices[1].u0		= 1.0f;
		pVertices[1].v0		= 0.0f;
		pVertices[2].u0		= 0.0f;
		pVertices[2].v0		= 1.0f;
		pVertices[3].u0		= 1.0f;
		pVertices[3].v0		= 1.0f;

		pVertices[0].x		= m_x - 0.5f;
		pVertices[0].y		= m_y - 0.5f;

		pVertices[1].x		= m_x + m_width - 0.5f;
		pVertices[1].y		= m_y - 0.5f;

		pVertices[2].x		= m_x - 0.5f;
		pVertices[2].y		= m_y + m_height - 0.5f;

		pVertices[3].x		= m_x + m_width - 0.5f;
		pVertices[3].y		= m_y + m_height - 0.5f;

		if (m_rot != 0)
		{ // apply rotation of m_rot radians around m_centerX, m_centerY

			m_centerX = m_x + m_width/2;
			m_centerY = m_y + m_height/2;

			float sn = sinf(m_rot);
			float cs = cosf(m_rot);
			// top left
			float x = m_x - m_centerX;
			float y = m_y - m_centerY;
			pVertices[0].x = (x * cs) - (y * sn) + m_centerX;
			pVertices[0].y = (x * sn) + (y * cs) + m_centerY;
			// bottom left
			x = m_x + m_width - m_centerX;
			y = m_y - m_centerY;
			pVertices[1].x = (x * cs) - (y * sn) + m_centerX;
			pVertices[1].y = (x * sn) + (y * cs) + m_centerY;
			// bottom right
			x = m_x - m_centerX;
			y = m_y + m_height - m_centerY;
			pVertices[2].x = (x * cs) - (y * sn) + m_centerX;
			pVertices[2].y = (x * sn) + (y * cs) + m_centerY;
			// top right
			x = m_x + m_width - m_centerX;
			y = m_y + m_height - m_centerY;
			pVertices[3].x = (x * cs) - (y * sn) + m_centerX;
			pVertices[3].y = (x * sn) + (y * cs) + m_centerY;
		}

		pVertices[0].z		= m_z - 0.5f;
		pVertices[1].z		= m_z - 0.5f;
		pVertices[2].z		= m_z - 0.5f;
		pVertices[3].z		= m_z - 0.5f;

		m_boundingRect.top = (long) ( __min (__min (pVertices[0].y, pVertices[1].y), __min (pVertices[2].y, pVertices[3].y)));
		m_boundingRect.left = (long) ( __min (__min (pVertices[0].x, pVertices[1].x), __min (pVertices[2].x, pVertices[3].x)));
		m_boundingRect.bottom = (long) ( __max (__max (pVertices[0].y, pVertices[1].y), __max (pVertices[2].y, pVertices[3].y)));
		m_boundingRect.right = (long) ( __max (__max (pVertices[0].x, pVertices[1].x), __max (pVertices[2].x, pVertices[3].x)));

		m_vb->Unlock();
	}
}



void Sprite::Render(void)
{
	if(m_vb && m_tex)
	{
		if(m_isDirty)
		{
			UpdatePosition();
			m_isDirty = false;
		}

		DXBase::GetInstance()->Get3DDevice()->SetTexture(0, m_tex);
		DXBase::GetInstance()->Get3DDevice()->SetVertexShader(NULL);
		DXBase::GetInstance()->Get3DDevice()->SetFVF(SpriteVertexFVF);
		DXBase::GetInstance()->Get3DDevice()->SetStreamSource(0, m_vb, 0, sizeof(SpriteVertex));
		DXBase::GetInstance()->Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
}


void Sprite::SetRotation(float x, float y, float rot) 
{
	if(x >= 0)
		m_centerX = x; 
	if(y >= 0)
		m_centerY = y; 
	if(rot >= 0)
		m_rot = rot; 

	m_isDirty = true;
}

