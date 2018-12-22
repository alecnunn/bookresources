#ifndef __DXSPRITE__
#define __DXSPRITE__

class Sprite
{
	public:
							Sprite();
		virtual				~Sprite();

		virtual bool		SetTexture(const char *filename);

		virtual void		Render(void);

		virtual void		SetPosition(float x, float y, float z) {m_x = x; m_y = y; m_z = z; m_isDirty = true;}
		virtual void		GetPosition(float &x, float &y, float &z) { x = m_x; y = m_y; z = m_z;}
		virtual void		SetDimensions(float  width, float  height ) {m_height = height; m_width = width; m_isDirty = true;}
		virtual void		GetDimensions(float &width, float &height ) {width = m_width; height = m_height;}

		void				SetRotation(float x, float y, float rot);
		void				GetRotation(float &x, float &y, float &rot) {x = m_centerX; y = m_centerY; rot = m_rot;}

		RECT				GetBoundingRect() {return m_boundingRect;}

	protected:
		float				m_x;
		float				m_y;
		float				m_z;
		float				m_height, m_width;

		bool				m_isDirty;

		void				UpdatePosition(void);

	private:
		LPDIRECT3DVERTEXBUFFER9		m_vb;
		LPDIRECT3DTEXTURE9			m_tex;

		float				m_centerX, m_centerY, m_rot;

		RECT				m_boundingRect;
};

#endif