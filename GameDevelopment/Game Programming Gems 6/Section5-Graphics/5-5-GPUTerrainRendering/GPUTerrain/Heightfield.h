#ifndef __HEIGHTFIELD_H_
#define __HEIGHTFIELD_H_

class Heightfield
{
public:
	Heightfield();
	~Heightfield();

	HRESULT Initialize(IDirect3DDevice9 *pDevice, LPCWSTR szHeightfield);
	void OnDestroyDevice();

	IDirect3DTexture9* GetHeightfield() { return m_pHeightfield; }
	IDirect3DTexture9* GetVertexTexture() { return m_pVertexTexture; }

	DWORD GetWidth() const { return m_Info.Width; }
	DWORD GetHeight() const { return m_Info.Height; }

private:
	IDirect3DTexture9*		m_pHeightfield;
	IDirect3DTexture9*		m_pVertexTexture;
	D3DXIMAGE_INFO			m_Info;
};

#endif // __HEIGHTFIELD_H_
