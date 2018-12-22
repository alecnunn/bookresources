// WorldView.cpp : implementation file
//

#include "stdafx.h"
#include "FoliageDemo.h"
#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	VIEWHEIGHT	(5.0f)


static const WIDGETVTX	_aWidgetVerts[] =
{
	//	0 2 4			6 2 8
	//	+-+-+	<x>		+-+-+	<z>
	//	|\|\|	^y^		|\|\|	^y^
	//	+-+-+			+-+-+
	//	1 3 5			7 3 9
/*0*/	{	{	-1.0f,	2.0f,	0.0f	},	{	1.0f,	0.0f	},	{0,0}	},
/*1*/	{	{	-1.0f,	0.0f,	0.0f	},	{	1.0f,	1.0f	},	{0,0}	},
/*2*/	{	{	 0.0f,	2.0f,	0.0f	},	{	0.0f,	0.0f	},	{0,0}	},
/*3*/	{	{	 0.0f,	0.0f,	0.0f	},	{	0.0f,	1.0f	},	{0,0}	},
/*4*/	{	{	 1.0f,	2.0f,	0.0f	},	{	1.0f,	0.0f	},	{0,0}	},
/*5*/	{	{	 1.0f,	0.0f,	0.0f	},	{	1.0f,	1.0f	},	{0,0}	},
/*6*/	{	{	 0.0f,	2.0f,	-1.0f	},	{	1.0f,	0.0f	},	{0,0}	},
/*7*/	{	{	 0.0f,	0.0f,	-1.0f	},	{	1.0f,	1.0f	},	{0,0}	},
/*8*/	{	{	 0.0f,	2.0f,	1.0f	},	{	1.0f,	0.0f	},	{0,0}	},
/*9*/	{	{	 0.0f,	0.0f,	1.0f	},	{	1.0f,	1.0f	},	{0,0}	}
};

static const u16 _aWidgetIndices[] =
{
	0,1,2,3,4,5,
	5,6,			// degenerates
	6,7,2,3,8,9
};

static const char *_aWidgetTextures[] =
{
	"data\\bush.tga",
	"data\\bush2.tga"
};

static const float _aWidgetScales[] =
{
	0.5f,0.75f
};

/////////////////////////////////////////////////////////////////////////////
// CWorldView

CWorldView::CWorldView()
{
	m_bInit = false;
	m_uMouseButtons = 0;
	m_uNumWidgets = 0;
	m_fov[0] = PI*0.5f;
	m_fov[1] = PI*0.5f;

	m_lighting.lightDir[0] = 1.0f;
	m_lighting.lightDir[1] = 0.75f;
	m_lighting.lightDir[2] = 0.5f;
	m_lighting.lightDir[3] = 0.0f;
	m_lighting.lightColor[0] = 0.9f;
	m_lighting.lightColor[1] = 0.8f;
	m_lighting.lightColor[2] = 0.7f;
	m_lighting.lightColor[3] = 0.0f;
	m_lighting.ambientColor[0] = 0.1f;
	m_lighting.ambientColor[1] = 0.1f;
	m_lighting.ambientColor[2] = 0.1f;
	m_lighting.ambientColor[3] = 0.0f;

	D3DXVec4Normalize(&m_lighting.lightDir,&m_lighting.lightDir);
}

CWorldView::~CWorldView()
{
}


BEGIN_MESSAGE_MAP(CWorldView, CStatic)
	//{{AFX_MSG_MAP(CWorldView)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldView message handlers

bool CWorldView::Init()
{
	u32		i;
	if( m_bInit==false )
	{
		do
		{
			m_uNumWidgets = 0;

			// initialize D3D
			if( !m_d3d.Create(m_hWnd) )
			{
				break;
			}

			// initialize the world
			if( !m_world.Create(&m_d3d) )
			{
				break;
			}

			// initialize the widget mesh
			for( i=0;i<countof(m_widgetMesh);i++ )
			{
				if( !m_widgetMesh[i].Create(	&m_d3d,
											countof(_aWidgetVerts),_aWidgetVerts,
											countof(_aWidgetIndices),_aWidgetIndices,
											_aWidgetTextures[i], _aWidgetScales[i] ) )
				{
					break;
				}
			}
			SetDrawDistance(100.0f);

			D3DXMatrixPerspectiveFovLH(	&m_projMtx,
										m_fov[1],
										m_fov[0]/m_fov[1],
										0.1f,
										1000.0f );


			m_viewPos[0] = 0.0f;
			m_viewPos[1] = 50.0f;
			m_viewPos[2] = 0.0f;
			m_euler[0] = m_euler[1] = m_euler[2] = 0.0f;
			UpdateViewMatrix();

			SetTimer(0x123,20,0);

			m_bInit = true;
		} while(0);

		if( !m_bInit )
		{
			Uninit();
		}
	}
	return m_bInit;
}

void CWorldView::Uninit()
{
	u32		i;
	m_uNumWidgets = 0;
	for( i=0;i<countof(m_widgetMesh);i++ )
	{
		m_widgetMesh[i].Destroy();
		m_widgetBSP[i].DestroyTree();
	}
	m_world.Destroy();
	m_d3d.Destroy();
	m_bInit = false;
}

void CWorldView::UpdateViewMatrix()
{
	D3DXVECTOR3	extents[2];

	m_world.GetBoundingBox(extents);

	m_viewPos[0] = min(max(extents[0][0],m_viewPos[0]),extents[1][0]);
	m_viewPos[2] = min(max(extents[0][2],m_viewPos[2]),extents[1][2]);
	m_viewPos[1] = m_world.GetHeight(m_viewPos) + VIEWHEIGHT;

	D3DXMatrixRotationYawPitchRoll(&m_viewMtx,m_euler[0],m_euler[1],m_euler[2]);
	m_viewMtx(3,0) = m_viewPos[0];
	m_viewMtx(3,1) = m_viewPos[1];
	m_viewMtx(3,2) = m_viewPos[2];
}

//---------------------------------------------------------------------------
//	mouse controls
//---------------------------------------------------------------------------
void CWorldView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	UpdateMouse(2,0,point);
	CStatic::OnRButtonDown(nFlags, point);
}

void CWorldView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	UpdateMouse(0,2,point);
	CStatic::OnRButtonUp(nFlags, point);
}

void CWorldView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UpdateMouse(1,0,point);
	CStatic::OnLButtonDown(nFlags, point);
}

void CWorldView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateMouse(0,1,point);
	CStatic::OnLButtonUp(nFlags, point);
}

void CWorldView::UpdateMouse( DWORD buttonsDown, DWORD buttonsUp, CPoint point )
{
	DWORD	oldButtons = m_uMouseButtons;

	m_lastMousePos = point;
	m_uMouseButtons = (m_uMouseButtons|buttonsDown)&~buttonsUp;
	if( oldButtons!=0 && m_uMouseButtons==0 )
	{
		::SetCapture(NULL);
	}
	else if( oldButtons==0 && m_uMouseButtons!=0 )
	{
		::SetCapture(m_hWnd);
	}
}

void CWorldView::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT	movement;

	movement.x = point.x - m_lastMousePos.x;
	movement.y = point.y - m_lastMousePos.y;
	m_lastMousePos = point;

	// TODO: Add your message handler code here and/or call default
	if( m_uMouseButtons&2 )
	{
		m_euler[0] += movement.x*1.0f*PI/180.0f;
		m_euler[1] += movement.y*1.0f*PI/180.0f;
		UpdateViewMatrix();
		Draw();
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CWorldView::Draw()
{
	u32		i;
	if( !Init() )
	{
		PostQuitMessage(-1);
	}
	else
	{
		if( m_d3d.BeginFrame() )
		{
			m_d3d.SetProjectionMatrix(&m_projMtx,FALSE);
			m_d3d.SetViewMatrix(&m_viewMtx,TRUE);
			m_d3d.SetVertexShaderConstants(&m_lighting,4,3);

			m_world.Draw();

			for( i=0;i<countof(m_widgetMesh);i++ )
			{
				m_widgetMesh[i].SetFadeDistance(m_drawDistance*0.9f,m_drawDistance);
				m_widgetBSP[i].Draw(m_viewMtx,m_fov,m_drawDistance);
			}

			m_d3d.EndFrame();
		}
	}
}

//---------------------------------------------------------------------------
//	draw the world
//---------------------------------------------------------------------------
void CWorldView::OnPaint() 
{
	CPaintDC	dc(this); // device context for painting
	Draw();
}


BOOL CWorldView::DestroyWindow() 
{
	Uninit();
	return CStatic::DestroyWindow();
}

void CWorldView::OnTimer(UINT nIDEvent) 
{
	static BOOL bInTimer = FALSE;

	if( !bInTimer )
	{
		bInTimer = TRUE;

		if( m_uMouseButtons&1 )
		{
			m_viewPos[0] += 1.0f*m_viewMtx(2,0);
			m_viewPos[1] += 1.0f*m_viewMtx(2,1);
			m_viewPos[2] += 1.0f*m_viewMtx(2,2);
			UpdateViewMatrix();
		}
	
		// force a redraw
		Draw();

		bInTimer = FALSE;
	}

	CStatic::OnTimer(nIDEvent);
}

//---------------------------------------------------------------------------
//	create widgets
//---------------------------------------------------------------------------
void CWorldView::CreateWidgets( u32 uNumWidgets )
{
	u32			i,j;
	float		color;
	float		x,z,a,s;
	D3DXVECTOR3	bbox[2];
	WIDGETLEAF	*pLeaf;
	u32			uNum;

	if( m_uNumWidgets==uNumWidgets )
		return;
	m_uNumWidgets = uNumWidgets;

	pLeaf = (WIDGETLEAF*)malloc(uNumWidgets*sizeof(WIDGETLEAF));
	if( pLeaf )
	{
		m_world.GetBoundingBox(bbox);

		for( j=0;j<countof(m_widgetMesh);j++ )
		{
			m_widgetBSP[j].DestroyTree();

			for( i=0,uNum=0;i<uNumWidgets;i++ )
			{
				x = rand()/32767.0f;
				z = rand()/32767.0f;

				// get a random position in the world
				pLeaf[uNum].position[0] = Lerp(bbox[0][0],bbox[1][0],x);
				pLeaf[uNum].position[1] = bbox[0][1];
				pLeaf[uNum].position[2] = Lerp(bbox[0][2],bbox[1][2],z);

				// is the landscape at this position something
				// that can hold a piece of foliage?
				color = m_world.GetColorChannel(pLeaf[uNum].position,0);
				if( (rand()/32767.0f)<color )
				{
					// get the height of the landscape at this position
					pLeaf[uNum].position[1] = m_world.GetHeight(pLeaf[uNum].position);

					// get a random angleand scale
					a = rand()*2*PI/32767.0f;
					s = 0.5f + rand()/32767.0f;

					pLeaf[uNum].sinAngle = (s8)(sin(a)*127.0f);
					pLeaf[uNum].cosAngle = (s8)(cos(a)*127.0f);
					pLeaf[uNum].scale = (u8)(s*32);

					uNum++;
				}
			}

			if( uNum )
			{
				m_widgetBSP[j].CreateTree(&m_widgetMesh[j],pLeaf,uNum);
			}
		}

		free(pLeaf);
	}
}

void CWorldView::SetDrawDistance( float fDistance )
{
	m_drawDistance = fDistance;
}
