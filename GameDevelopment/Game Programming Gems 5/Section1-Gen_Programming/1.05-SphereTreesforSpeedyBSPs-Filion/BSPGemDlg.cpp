// BSPGemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BSPGem.h"
#include "BSPGemDlg.h"
#include "MathStructs.h"
#include "PolyConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD BUFFER_LENGTH = 255;

BEGIN_MESSAGE_MAP(CBSPGemDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOADBTN, OnBnClickedLoadBtn)
	ON_BN_CLICKED(IDC_GOBTN, OnBnClickedGobtn)
END_MESSAGE_MAP()

CBSPGemDlg::CBSPGemDlg( CWnd* pParent ) : 
	CDialog			( CBSPGemDlg::IDD, pParent	),
	m_pD3D			( NULL						),
	m_pD3DDevice	( NULL						)
{
}

CBSPGemDlg::~CBSPGemDlg()
{
	// Release Direct3D interfaces
	if ( m_pD3DDevice )
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
	if ( m_pD3D )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

void CBSPGemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERTEXCOUNT, m_VertexCountLabel);
	DDX_Control(pDX, IDC_TRIANGLECOUNT, m_TriCountLabel);
	DDX_Control(pDX, IDC_GOBTN, m_GoBtn);
	DDX_Control(pDX, IDC_FILESTATIC, m_FilenameLabel);
	DDX_Control(pDX, IDC_LOGEDT, m_LogEdt);
	DDX_Control(pDX, IDC_STATUSEDT, m_StatusLabel);
	DDX_Control(pDX, IDC_OCTREENODECOUNT, m_SphereTreeNodeCountLabel);
	DDX_Control(pDX, IDC_OCTREELEAFCOUNT, m_SphereTreeLeavesLabel);
	DDX_Control(pDX, IDC_OCTREEDEPTH, m_SphereTreeDepthLabel);
	DDX_Control(pDX, IDC_OCTREEAVGDEPTH, m_AvgSphereTreeDepthLabel);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_AVGBSPDEPTH, m_AvgBSPDepth);
	DDX_Control(pDX, IDC_BSPDEPTH, m_BSPDepth);
	DDX_Control(pDX, IDC_BSPLEAVES, m_BSPLeaves);
	DDX_Control(pDX, IDC_BSPNODES, m_BSPNodes);
	DDX_Control(pDX, IDC_BSPSPLITS, m_BSPSplits);
}

BOOL CBSPGemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// This Gem does not require actual rendering
	// However, the D3DXLoadMeshFromX() call used in LoadFile() below requires a IDirect3DDevice9 interface as a parameter
	// Thus, we are forced to create a rendering interface, though we will not actually render anything
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !m_pD3D )
	{
		AfxMessageBox( "Could not create Direct3D rendering interface." );
		return FALSE;
	}

	D3DPRESENT_PARAMETERS PresentParameters;
	PresentParameters.AutoDepthStencilFormat		= D3DFMT_UNKNOWN;
	PresentParameters.BackBufferCount				= 1;
	PresentParameters.BackBufferFormat				= D3DFMT_UNKNOWN;
	PresentParameters.BackBufferHeight				= 0;
	PresentParameters.BackBufferWidth				= 0;
	PresentParameters.EnableAutoDepthStencil		= FALSE;
	PresentParameters.Flags							= 0;
	PresentParameters.FullScreen_RefreshRateInHz	= 0;
	PresentParameters.hDeviceWindow					= 0;
	PresentParameters.MultiSampleQuality			= 0;
	PresentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE ;
	PresentParameters.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;
	PresentParameters.SwapEffect					= D3DSWAPEFFECT_COPY;
	PresentParameters.Windowed						= TRUE;
	// A reference driver will be sufficient for our purposes
	if ( m_pD3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
								&PresentParameters, &m_pD3DDevice ) != D3D_OK )
	{
		AfxMessageBox( "Could not create Direct3DDevice rendering interface." );
		return FALSE;
	}
	return TRUE; 
}

bool CBSPGemDlg::LoadFile( const char* szFilename )
{
	m_StatusLabel.SetWindowText( "Loading file..." );

	m_pXMesh = NULL;
	// Load .x file mesh
	if ( D3DXLoadMeshFromX( szFilename, D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM, m_pD3DDevice, NULL, NULL, NULL, NULL, &m_pXMesh ) != D3D_OK )
		return false;

	// Update dialog
	char szText[BUFFER_LENGTH];

	// Vertex count
	sprintf( szText, "%d vertices.", m_pXMesh->GetNumVertices() );
	m_VertexCountLabel.SetWindowText( szText );

	// Face count
	sprintf( szText, "%d triangles.", m_pXMesh->GetNumFaces() );
	m_TriCountLabel.SetWindowText( szText );
	
	// Filename
	m_FilenameLabel.SetWindowText( szFilename );

	// Update log
	sprintf( szText, "File '%s' loaded.\r\n", szFilename );
	m_LogEdt.SetSel( -1, -1 );
	m_LogEdt.ReplaceSel( szText );

	// Status
	m_StatusLabel.SetWindowText( "Idle." );
	m_SphereTreeNodeCountLabel.SetWindowText( "0 sphere tree nodes." );

	m_GoBtn.EnableWindow( TRUE );
	return true;
}

void CBSPGemDlg::OnBnClickedLoadBtn()
{
	CFileDialog FileDialog( TRUE, NULL, NULL, 0, "DirectX Files (*.x)|*.x|All files (*.*)|*.*||" );
	if ( FileDialog.DoModal() == IDOK )
	{
		if ( LoadFile( FileDialog.GetPathName() ) )
		{
		}
		else AfxMessageBox( "Could not load X file." );
	}
}

void CBSPGemDlg::OnBnClickedGobtn()
{
	// Retrieve vertices
	BYTE* pBufferData;
	if ( m_pXMesh->LockVertexBuffer( D3DLOCK_READONLY, (void**)&pBufferData ) != D3D_OK )
	{
		AfxMessageBox( "Could not lock vertex buffer." );
		return;
	}
	DWORD dwVertexSize = m_pXMesh->GetNumBytesPerVertex();

	// Copy vertex positions only
	CVector* pVertices = new CVector[m_pXMesh->GetNumVertices()];
	CVector* pCurVertex = pVertices;
	for ( DWORD i = 0; i < m_pXMesh->GetNumVertices(); i++ )
	{
		CVector* pVector = (CVector*)pBufferData;
		*pCurVertex++ = *pVector;
		pBufferData += dwVertexSize;
	}
	m_pXMesh->UnlockVertexBuffer();

	// Retrieve triangle indices
	DWORD* pIndexData;
	if ( m_pXMesh->LockIndexBuffer( D3DLOCK_READONLY, (void**)&pIndexData ) != D3D_OK )
	{
		AfxMessageBox( "Could not lock index buffer." );
		return;
	}
	m_SphereTree.AddTriangles( m_pXMesh->GetNumFaces(), m_pXMesh->GetNumVertices(), pIndexData, pVertices );
	delete[] pVertices;
	m_pXMesh->UnlockIndexBuffer();

	m_StatusLabel.SetWindowText( "Building sphere tree..." );
	m_SphereTree.Build( &m_Progress );

	char szText[BUFFER_LENGTH];

	sprintf( szText, "%d sphere tree nodes.", m_SphereTree.GetNodeCount() );
	m_SphereTreeNodeCountLabel.SetWindowText( szText );
	sprintf( szText, "%d sphere tree leaves.", m_SphereTree.GetLeafCount() );
	m_SphereTreeLeavesLabel.SetWindowText( szText );
	sprintf( szText, "Max sphere tree depth: %d.", m_SphereTree.GetDepth() );
	m_SphereTreeDepthLabel.SetWindowText( szText );
	sprintf( szText, "Average sphere tree depth: %.1f.", m_SphereTree.GetAverageDepth() );
	m_AvgSphereTreeDepthLabel.SetWindowText( szText );

	m_StatusLabel.SetWindowText( "Building BSP tree..." );
	m_BSPTreeBuilder.Build( &m_SphereTree, &m_Progress );

	sprintf( szText, "%d BSP nodes.",	m_BSPTreeBuilder.GetBackNodeCount() + m_BSPTreeBuilder.GetBranchNodeCount() +
										m_BSPTreeBuilder.GetFrontNodeCount() );
	m_BSPNodes.SetWindowText( szText );
	sprintf( szText, "%d BSP leaves.",	m_BSPTreeBuilder.GetLeafCount() );
	m_BSPLeaves.SetWindowText( szText );
	sprintf( szText, "Max BSP depth: %d.",	m_BSPTreeBuilder.GetBSPDepth() );
	m_BSPDepth.SetWindowText( szText );
	sprintf( szText, "Average BSP depth: %.1f.",	m_BSPTreeBuilder.GetAvgDepth() );
	m_AvgBSPDepth.SetWindowText( szText );
	sprintf( szText, "BSP splits: %d.",	m_BSPTreeBuilder.GetLeafCount() - m_pXMesh->GetNumFaces() );
	m_BSPSplits.SetWindowText( szText );

	m_StatusLabel.SetWindowText( "Idle." );

}
