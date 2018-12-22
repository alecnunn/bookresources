//-----------------------------------------------------------------------------
// File: Tree.cpp
//
// Desc: 
//
// Author: Gabor Szijarto
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <d3dx9shader.h>
//#include <D3DFont.h>
#include <stdio.h>
#include "resource.h"




//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D			= NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pBlockVB		= NULL; // Buffer to hold impostors position
LPDIRECT3DVERTEXBUFFER9 g_pBranchesVB	= NULL; // Vertex buffer to hold branch
LPDIRECT3DINDEXBUFFER9  g_pBranchesIB	= NULL; // Index buffer to hold brench
LPDIRECT3DVERTEXBUFFER9 g_pLeavesVB		= NULL; // Vertex buffer to hold leaves
LPDIRECT3DINDEXBUFFER9  g_pLeavesIB		= NULL;	// Index buffer to hold leaves

IDirect3DTexture9*		pLeavesRenderTarget = NULL;	// inpostor texture
IDirect3DSurface9*		pLeavesDepthBuffer	= NULL; // depth buffer for impostor texture
IDirect3DVertexShader9*	pVShaderTree		= NULL; 
IDirect3DVertexShader9*	pVShaderLeaves		= NULL;
IDirect3DVertexShader9*	pVShaderObjects		= NULL;
IDirect3DPixelShader9*	pPShaderTree		= NULL;
IDirect3DPixelShader9*	pPShaderLeaves		= NULL;
IDirect3DPixelShader9*	pPShaderObjects		= NULL;
LPDIRECT3DVERTEXDECLARATION9    m_pBlockVertexDeclaration	= NULL;
LPDIRECT3DVERTEXDECLARATION9    m_pLeavesVertexDeclaration	= NULL;
LPDIRECT3DVERTEXDECLARATION9    m_pObjectsVertexDeclaration	= NULL;

D3DXVECTOR3 g_vEyePt( 0.0f, 2.0f, -20.f );
D3DXVECTOR3 g_vLookatPt( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 g_vUpVec( 0.0f, 1.0f, 0.0f );
float		g_FOV = D3DX_PI / 7;

//CD3DFont*     g_pFont;


 
int			cntBranch		= 0;
int			cntIndexBranch  = 0;

LONGLONG	lastTime		= 0;

int			cntLeaves		= 512;
int			cntIndexLeaves	= 0;
int			cntpLeaves		= 0;

int			cntBlock		= 0;
int			leavesBlockSize	= 256;

int			cntTree			= 1;
float		WoodRange		= 0.01f;

float		fps				= 0;

float		gBlockSize		= 0;

bool	doScreenshot	= false;

// A structure for our custom vertex type
struct BLOCKVERTEX
{
    D3DXVECTOR3 position; // The position
    FLOAT       tu, tv;   // The texture coordinates
    D3DCOLOR    color;    // The color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_BLOCKVERTEX (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE)

struct LEAVESVERTEX
{
    D3DXVECTOR3 position;	// The position
    D3DXVECTOR3 normal;		// The normal
    D3DCOLOR    color;		// The color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_LEAVESVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

// A structure for our custom vertex type
struct OBJVERTEX
{
    D3DXVECTOR3 position;	// The position
    D3DXVECTOR3 normal;		// The normal vector
    FLOAT       tu, tv;		// The texture coordinates
    D3DCOLOR	color;	// The position
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_OBJVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_DIFFUSE)





//-----------------------------------------------------------------------------
// Name: SaveBitmap()
// Desc: Load obj file
//-----------------------------------------------------------------------------
struct BitmapFileHeader {
    unsigned short	type;
    unsigned long	size;
    unsigned short	reserved1;
    unsigned short	reserved2;
    unsigned long	offbits;
};


struct BitmapInfoHeader {
    unsigned long	size;
    long			width;
    long			height;
    short			planes;
    short			bitcount;
    unsigned long	compression;
    unsigned long	sizeimage;
    unsigned long	xpelspermeter;
    unsigned long	ypelspermeter;
    unsigned long	clrused;
    unsigned long	clrimportant;
};

// write Bitmap File Header()
#define writeelm(fp,x) fwrite(&(x),sizeof(x),1,fp)

bool SaveToFile(const char *fname, int width, int height, unsigned char *lpTexture, int bytealignment, int scale = 1)
{

	FILE *fp = fopen(fname, "wb");
    if (fp) {
        int w = width * scale;
        int h = height * scale;

		BitmapFileHeader bfh;
		BitmapInfoHeader bih;

        // compute number of bytes per line (must be 4 byte aligned)
        long line_numbytes = (w*3+3) & (~3);
        unsigned char *line_buffer = (unsigned char *) malloc(line_numbytes);

        // fill out bitmap file header
        bfh.type = 0x4d42;
        bfh.size = 0x36 + (line_numbytes * h);
        bfh.reserved1 = 0;
        bfh.reserved2 = 0;
        bfh.offbits   = 0x36;
                        

        // fill out the bitmap info header
        bih.size          = sizeof(BitmapInfoHeader);
        bih.width         = w;
        bih.height        = h;
        bih.planes        = 1;
        bih.bitcount      = 24;
        bih.compression   = 0;
        bih.sizeimage     = 0;
        bih.xpelspermeter = 0;
        bih.ypelspermeter = 0;
        bih.clrused       = 0;
        bih.clrimportant  = 0;

        // write bitmap header and bitmap info to file
		writeelm(fp,bfh.type);
		writeelm(fp,bfh.size);
		writeelm(fp,bfh.reserved1);
		writeelm(fp,bfh.reserved2);
		writeelm(fp,bfh.offbits);

		// write Bitmap Info Header
		writeelm(fp,bih.size);
		writeelm(fp,bih.width);
		writeelm(fp,bih.height);
		writeelm(fp,bih.planes);
		writeelm(fp,bih.bitcount);
		writeelm(fp,bih.compression);
		writeelm(fp,bih.sizeimage);
		writeelm(fp,bih.xpelspermeter);
		writeelm(fp,bih.ypelspermeter);
		writeelm(fp,bih.clrused);
		writeelm(fp,bih.clrimportant);

		for (long y = h-1; y >= 0; y--) {
			for (long x = 0; x < w; x++) {
				unsigned char red;
				unsigned char green;
				unsigned char blue;
				red		= lpTexture[((x/scale)+width*(y/scale)) * bytealignment+0];
				green	= lpTexture[((x/scale)+width*(y/scale)) * bytealignment+1];
				blue	= lpTexture[((x/scale)+width*(y/scale)) * bytealignment+2];
				line_buffer[x * 3 + 0] = red;
				line_buffer[x * 3 + 1] = green;
				line_buffer[x * 3 + 2] = blue;
			}
			fwrite(line_buffer, line_numbytes, 1, fp);
		}

		free( line_buffer);
		fclose(fp);

        return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
// Name: LoadObj()
// Desc: Load obj file
//-----------------------------------------------------------------------------
OBJVERTEX *LoadObj(	const char *fname, 
					DWORD &num,
					D3DXMATRIXA16 &trsM				= D3DXMATRIXA16(), 
					bool genTwoSide					= false, 
					bool indexed					= false, 
					unsigned long **indexbuffer		= NULL, 
					DWORD *indexNum					= NULL)
{
	FILE *fin = fopen( fname, "rt");
	if (!fin)
		return NULL;
	
	char line[1024];
	int vertexnum		= 0;
	int vertexnormalnum = 0;
	int vertexuvnum		= 0;
	int facenum			= 0;

	int vertID	= -1;
	int normID	= -1;
	int uvID	= -1;
	int tagID	= 0;

	while (fgets(line, sizeof(line), fin)) {
		char *kw = strtok(line," \t\n");
		if (kw) {
			if (!strcmp(kw, "v")) {
				if (vertID < 0)
					vertID = tagID++;
				vertexnum++;
			} else if (!strcmp(kw, "vn")) {
				if (normID < 0)
					normID = tagID++;
				vertexnormalnum++;
			} else if (!strcmp(kw, "vt")) {
				if (uvID < 0)
					uvID = tagID++;
				vertexuvnum++;
			} else if (!strcmp(kw, "f")) {
				facenum++;
			}
		}
	}
	facenum	*= 3;

	int *vertsidx = new int[facenum * 3];
	memset( vertsidx, 0, sizeof(int) * facenum * 3);
	
	D3DXVECTOR3 *verts		= new D3DXVECTOR3[vertexnum];
	D3DXVECTOR3 *vertnorms	= new D3DXVECTOR3[vertexnormalnum];
	D3DXVECTOR3 *vertuv		= new D3DXVECTOR3[vertexuvnum];
	
	D3DXMATRIXA16 trsMR = trsM;
	trsMR(3,0) = 0;
	trsMR(3,1) = 0;
	trsMR(3,2) = 0;
	trsMR(0,3) = 0;
	trsMR(1,3) = 0;
	trsMR(2,3) = 0;

	fseek( fin, 0, SEEK_SET);	
	vertexnum		= 0;
	vertexnormalnum = 0;
	vertexuvnum		= 0;
	int face		= 0;
	
	while (fgets(line, sizeof(line), fin)) {
		char *kw = strtok(line," \t\n");
		if (kw) {
			if (!strcmp(kw, "v")) {
				D3DXVECTOR3 v;
				char *f;
				f = strtok(NULL," \t\n");
				if (f) v.x = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.y = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.z = (float)atof( f);

			    D3DXVec3TransformCoord( &(verts[vertexnum]), &v, &trsM);
				vertexnum++;

			} else if (!strcmp(kw, "vn")) {
				D3DXVECTOR3 v;
				char *f;
				f = strtok(NULL," \t\n");
				if (f) v.x = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.y = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.z = (float)atof( f);
				
			    D3DXVec3TransformCoord( &(vertnorms[vertexnormalnum]), &v, &trsMR);
				D3DXVec3Normalize( &vertnorms[vertexnormalnum], &vertnorms[vertexnormalnum]);
				vertexnormalnum++;
			} else if (!strcmp(kw, "vt")) {
				D3DXVECTOR3 v;
				char *f;
				f = strtok(NULL," \t\n");
				if (f) v.x = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.y = (float)atof( f);
				f = strtok(NULL," \t\n");
				if (f) v.z = (float)atof( f);

				vertuv[vertexuvnum++] = v;
			} else if (!strcmp(kw, "f")) {
				char *f;
				
				f = strtok(NULL," \t\n");
				if (!f) continue;
				vertsidx[face * 9 + 0] = atoi( f);
				
				while (f[0] != '\0' && f[0] != '/') f++;
				if (f[0] == '/') {
					// normal index
					while (f[0] == '/') f++;
					vertsidx[face * 9 + 1] = atoi( f);
					
					while (f[0] != '\0' && f[0] != '/') f++;
					if (f[0] == '/') {
						// uv index
						while (f[0] == '/') f++;
						vertsidx[face * 9 + 2] = atoi( f);
					}
				}
				
				
				f = strtok(NULL," \t\n");
				if (!f) continue;
				vertsidx[face * 9 + 3] = atoi( f);
				
				while (f[0] != '\0' && f[0] != '/') f++;
				if (f[0] == '/') {
					// normal index
					while (f[0] == '/') f++;
					vertsidx[face * 9 + 4] = atoi( f);
					
					while (f[0] != '\0' && f[0] != '/') f++;
					if (f[0] == '/') {
						// uv index
						while (f[0] == '/') f++;
						vertsidx[face * 9 + 5] = atoi( f);
					}
				}
				
				f = strtok(NULL," \t\n");
				if (!f) continue;
				vertsidx[face * 9 + 6] = atoi( f);
				
				while (f[0] != '\0' && f[0] != '/') f++;
				if (f[0] == '/') {
					// normal index
					while (f[0] == '/') f++;
					vertsidx[face * 9 + 7] = atoi( f);
					
					while (f[0] != '\0' && f[0] != '/') f++;
					if (f[0] == '/') {
						// uv index
						while (f[0] == '/') f++;
						vertsidx[face * 9 + 8] = atoi( f);
					}
				}
				
				face++;
			}
		}
	}
	
	fclose(fin);
	
	int last = face*3;
	num = genTwoSide ? last*2 : last;
	OBJVERTEX *res = new OBJVERTEX[ num];
	memset( res, 0, sizeof(OBJVERTEX) * num);
	
	for (int idx = 0; idx < last; idx++) {
		int datatag = 0;
		res[idx].position		= verts[vertsidx[idx*3 + vertID]-1];
		if (vertexnormalnum)
			res[idx].normal		= vertnorms[vertsidx[idx*3 + normID]-1];
		if (vertexuvnum) {
			res[idx].tu			= vertuv[vertsidx[idx*3 + uvID] - 1].x;
			res[idx].tv			= vertuv[vertsidx[idx*3 + uvID] - 1].y;
		}
	}
	
	if (genTwoSide) {
		for (int idx = 0; idx < last; idx++) {
			
			int midx;
			if ((idx % 3) == 0) {
				midx = idx + 1;
			} else if ((idx % 3) == 1) {
				midx = idx - 1;
			} else {
				midx = idx;
			}

			midx += last;
			int datatag = 0;
			res[midx].position			= verts[vertsidx[idx*3 + vertID]-1];
			if (vertexnormalnum)
				res[midx].normal		= -vertnorms[vertsidx[idx*3 + normID]-1];
			if (vertexuvnum) {
				res[midx].tu			= vertuv[vertsidx[idx*3 + uvID] - 1].x;
				res[midx].tv			= vertuv[vertsidx[idx*3 + uvID] - 1].y;
			}
		}
	}
	if (indexed && (vertexnum == vertexnormalnum)) {
		delete res;
		int numi = genTwoSide ? 2 * last : last;
		unsigned long *index = new unsigned long[ numi];
		memset( index, 0, sizeof(unsigned long) * numi);
		
		for (int idx = 0; idx < last; idx++) {
			index[idx] = vertsidx[idx*3 + vertID]-1;
		}
		if (genTwoSide) {
			for (idx = 0; idx < last; idx++) {
				int midx;
				if ((idx % 3) == 0) {
					midx = idx + 1;
				} else if ((idx % 3) == 1) {
					midx = idx - 1;
				} else {
					midx = idx;
				}
				
				index[midx + last] = vertsidx[idx*3 + vertID]-1 + vertexnum;
			}
		}
		
		int numv = genTwoSide ? 2 * vertexnum : vertexnum;
		res = new OBJVERTEX[ numv];
		memset( res, 0, sizeof(OBJVERTEX) * numv);
		
		for (int idx = 0; idx < vertexnum; idx++) {
			int datatag = 0;
			res[idx].position		= verts[idx];
			if (vertexnormalnum)
				res[idx].normal		= vertnorms[idx];
			if (vertexuvnum) {
				res[idx].tu			= vertuv[idx].x;
				res[idx].tv			= vertuv[idx].y;
			}
		}
		if (genTwoSide) {
			for (int idx = 0; idx < vertexnum; idx++) {
				int midx;
				midx = idx + vertexnum;
				
				int datatag = 0;
				res[midx].position			= verts[idx];
				if (vertexnormalnum)
					res[midx].normal		= -vertnorms[idx];
				if (vertexuvnum) {
					res[midx].tu			= vertuv[idx].x;
					res[midx].tv			= vertuv[idx].y;
				}
			}
		}
		*indexNum = numi;
		(*indexbuffer) = index;
		num = numv;
	}
	
	delete [] vertnorms;
	delete [] verts;
	delete [] vertuv;
	delete [] vertsidx;
	

return res;
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	D3DCAPS9 Caps;
	g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);

	D3DDEVTYPE devType		= D3DDEVTYPE_HAL;
	DWORD behaviourFlag		= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	if (( Caps.PixelShaderVersion < D3DPS_VERSION(2,0) ) &&
		( Caps.VertexShaderVersion < D3DVS_VERSION(2,0) )) {
		
		devType				= D3DDEVTYPE_REF;
		behaviourFlag		= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		
		if (IDNO == ::MessageBox( hWnd, "Your video card is not supported at least 2.0 pixel and vertex shader!\n\tDo you want to run with reference driver?", "Tree - Question!", MB_YESNO | MB_ICONQUESTION))
	        return E_FAIL;
	}

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed					= TRUE;
    d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat			= D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil	= TRUE;
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	d3dpp.BackBufferCount			= 1;
	d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, devType, hWnd, behaviourFlag,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	// create render targets
	g_pd3dDevice->CreateTexture( leavesBlockSize, leavesBlockSize, 1, 
		D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, 
		&pLeavesRenderTarget, 
		NULL);

	g_pd3dDevice->CreateDepthStencilSurface( leavesBlockSize, leavesBlockSize, 
		D3DFMT_D16, 
		D3DMULTISAMPLE_NONE, 
		0,
		true,
		&pLeavesDepthBuffer, 
		NULL);

	// set rendert state
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// texture render state
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	{ // load vertex shader program for impostor drawing

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\blocks.vsh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &pVShaderTree);
	   
		D3DVERTEXELEMENT9 declBlock[] =
		{
			// First stream is first mesh
			{ 0,  0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0}, 
			{ 0, 12, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0}, 
			{ 0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0}, 
			D3DDECL_END()
		};

		if( FAILED( g_pd3dDevice->CreateVertexDeclaration( declBlock, &m_pBlockVertexDeclaration) ) )
		{
			return -1;
		}

	}
	
	{ // load pixel shader program for impostor drawing

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\blocks_ps.psh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &pPShaderTree);
   
	}

	{ // load vertex shader program for impostor

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\leaves.vsh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &pVShaderLeaves);
	   
		D3DVERTEXELEMENT9 declLeaves[] =
		{
			// First stream is first mesh
			{ 0,  0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0}, 
			{ 0, 12, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0}, 
			{ 0, 24, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0}, 
			D3DDECL_END()
		};

		if( FAILED( g_pd3dDevice->CreateVertexDeclaration( declLeaves, &m_pLeavesVertexDeclaration) ) )
		{
			return -1;
		}

	}
	
	{ // load pixel shader program for impostor

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\leaves_ps.psh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &pPShaderLeaves);
   
	}



	{ // load vertex shader program for objects

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\objects.vsh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &pVShaderObjects);
	   
		D3DVERTEXELEMENT9 declObjects[] =
		{
			// First stream is first mesh
			{ 0,  0, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0}, 
			{ 0, 12, D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0}, 
			{ 0, 24, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0}, 
			{ 0, 32, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,			0}, 
			D3DDECL_END()
		};

		if( FAILED( g_pd3dDevice->CreateVertexDeclaration( declObjects, &m_pObjectsVertexDeclaration) ) )
		{
			return -1;
		}

	}
	
	{ // load pixel shader program for objects

		LPD3DXBUFFER		pCode = NULL;
		D3DXAssembleShaderFromFile( ".\\objects_ps.psh", NULL, NULL, 0, &pCode, NULL);

		g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &pPShaderObjects);
   
	}


//	{ // create font
//		g_pFont        = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
//		g_pFont->InitDeviceObjects( g_pd3dDevice );
//		g_pFont->RestoreDeviceObjects();
//	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{


	{ // load leaves
		DWORD			vertexNum;
		D3DXMATRIXA16	trMatrix;

		unsigned long	*indexBuffer;
		DWORD			indexNum;
		
		// scale loaded object
		D3DXMatrixScaling( &trMatrix, 0.0003f, 0.0003f, 0.0003f);

		OBJVERTEX *ptrLeaf = LoadObj( "../objects/Leaf.obj", vertexNum, trMatrix, true, true, &indexBuffer, &indexNum);
		
		if (!ptrLeaf) {
			ptrLeaf = LoadObj( "Leaf.obj", vertexNum, trMatrix, true, true, &indexBuffer, &indexNum);
		}

		if (ptrLeaf) {

			// create vertex buffer
			cntpLeaves = vertexNum * cntLeaves;
			
			if( FAILED( g_pd3dDevice->CreateVertexBuffer( cntpLeaves * sizeof(LEAVESVERTEX),
														D3DUSAGE_WRITEONLY, D3DFVF_LEAVESVERTEX,
														D3DPOOL_DEFAULT, &g_pLeavesVB, NULL ) ) )
				{
					return E_FAIL;
				}

			VOID* pVertices;
			if( FAILED( g_pLeavesVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
				return E_FAIL;

			LEAVESVERTEX *leaves = (LEAVESVERTEX*)pVertices;
			
			unsigned int color = 0;
			for (int lidx = 0; lidx < cntLeaves; lidx++) {
			
				D3DXVECTOR3 offset( ((float)rand() / (float)RAND_MAX - 0.5f),
									((float)rand() / (float)RAND_MAX - 0.5f),
									((float)rand() / (float)RAND_MAX - 0.5f));
									
				float fmax		= (float)max( fabs(offset.x), max( fabs(offset.y), fabs(offset.z)));
				float scale		= D3DXVec3Length( &offset) * 1.f / fmax;
				offset			= offset * 1.0f / scale;
				
				unsigned long color = 0xff300010;
				float green	= (float)rand() / (float)RAND_MAX * 100 + 134;
				color		+= (unsigned long)(green) * 0x00000100;
				color		+= (unsigned long)((float)rand() / (float)RAND_MAX * 50 * green / 255.f + 80) * 0x00010000;
				
				D3DXMATRIX matRot;
				D3DXMATRIX matRotRes;
				D3DXMatrixIdentity( &matRot );
				D3DXMatrixIdentity( &matRotRes );
				D3DXMatrixRotationX( &matRot, (float)rand() * 2 * D3DX_PI / (float)RAND_MAX );
				D3DXMatrixMultiply( &matRotRes, &matRotRes, &matRot);
				D3DXMatrixRotationY( &matRot, (float)rand() * 2 * D3DX_PI / (float)RAND_MAX );
				D3DXMatrixMultiply( &matRotRes, &matRotRes, &matRot);
				D3DXMatrixRotationZ( &matRot, (float)rand() * 2 * D3DX_PI / (float)RAND_MAX );
				D3DXMatrixMultiply( &matRotRes, &matRotRes, &matRot);
				D3DXVECTOR4 res;
			
				for (unsigned int lpidx = 0; lpidx < vertexNum; lpidx++) {
					D3DXVec3Transform( &res, &(ptrLeaf[lpidx].position), &matRotRes);
					leaves[lidx * vertexNum + lpidx].position	= (D3DXVECTOR3)res + offset;
					D3DXVec3Transform( &res, &(ptrLeaf[lpidx].normal), &matRotRes);
					leaves[lidx * vertexNum + lpidx].normal		= (D3DXVECTOR3)res;
					leaves[lidx * vertexNum + lpidx].color		= color;
				}
			}
			g_pLeavesVB->Unlock();

			// create index buffer
			if( FAILED( g_pd3dDevice->CreateIndexBuffer( cntLeaves * indexNum * sizeof(WORD),
														D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
														D3DPOOL_DEFAULT, &g_pLeavesIB, NULL ) ) )
			{
				return E_FAIL;
			}
			
			VOID* pIndices;
			if( FAILED( g_pLeavesIB->Lock( 0, 0, (void**)&pIndices, 0 ) ) )
				return E_FAIL;
				
			WORD *bindices = (WORD*)pIndices;
			
			for (lidx = 0; lidx < cntLeaves; lidx++) {
				for (unsigned int iidx = 0; iidx < indexNum; iidx++) {
					bindices[lidx * indexNum + iidx] = 
						(WORD)(indexBuffer[iidx] + vertexNum * lidx);
				}
			}
				
			cntIndexLeaves = cntLeaves * indexNum;
			
			g_pLeavesIB->Unlock();
		}
		
		delete ptrLeaf;
	}





	{
		DWORD num;
		D3DXMATRIXA16 trM, matTmp;
		
		D3DXMatrixScaling( &trM, 0.02f, 0.02f, 0.02f);
		D3DXMatrixTranslation( &matTmp, 00.f, 30.f, -150.f);
		D3DXMatrixMultiply( &trM, &matTmp, &trM);
		D3DXMatrixRotationX( &matTmp, -3.141f/2.f);
		D3DXMatrixMultiply( &trM, &trM, &matTmp);


		OBJVERTEX *ptrBlocks = LoadObj( "../objects/Impostors.obj", num, trM);
		
		if (!ptrBlocks) {
			ptrBlocks = LoadObj( "Impostors.obj", num, trM);
		}

		cntBlock = num;

		if (ptrBlocks) {

			if( FAILED( g_pd3dDevice->CreateVertexBuffer( cntBlock * sizeof(BLOCKVERTEX),
														0, D3DFVF_BLOCKVERTEX,
														D3DPOOL_DEFAULT, &g_pBlockVB, NULL ) ) )
			{
				return E_FAIL;
			}

			VOID* pVertices;
			if( FAILED( g_pBlockVB->Lock( 0, cntBlock * sizeof(BLOCKVERTEX), (void**)&pVertices, 0 ) ) )
				return E_FAIL;

			BLOCKVERTEX *blocks = (BLOCKVERTEX*)pVertices;
		    
			unsigned int color = 0;
			for (int bidx = 0; bidx < cntBlock; bidx++) {
				blocks[bidx].position	= ptrBlocks[bidx].position;
				blocks[bidx].tu			= ptrBlocks[bidx].tu;
				blocks[bidx].tv			= ptrBlocks[bidx].tv;
				
				if ((bidx % 6) == 0) {
					int fact = (int)((float)rand() / (float)RAND_MAX * 50 + 205);
					color = 0xff000000 + 0x010101 * fact;
				}
				
				blocks[bidx].color	= color;
			}
	//		for (int bidx = 0; bidx < 6; bidx++)
	//			blocks[bidx].position	= D3DXVECTOR3(0,0,0);
			g_pBlockVB->Unlock();
		}
		delete ptrBlocks;




		{		
			num				= 0;
			unsigned long	*indexbuffer;
			DWORD			indexNum;
			
			OBJVERTEX *ptrBranches = LoadObj( "../objects/Tree.obj", num, trM, false, true, &indexbuffer, &indexNum);

			if (!ptrBranches) {
				ptrBranches = LoadObj( "Tree.obj", num, trM, false, true, &indexbuffer, &indexNum);
			}
			
			cntBranch = num;

			if (ptrBranches) {

				if( FAILED( g_pd3dDevice->CreateVertexBuffer( cntBranch * sizeof(OBJVERTEX),
															0, D3DFVF_OBJVERTEX,
															D3DPOOL_DEFAULT, &g_pBranchesVB, NULL ) ) )
				{
					return E_FAIL;
				}

				VOID* pVertices;
				if( FAILED( g_pBranchesVB->Lock( 0, cntBranch * sizeof(OBJVERTEX), (void**)&pVertices, 0 ) ) )
					return E_FAIL;

				OBJVERTEX *branch = (OBJVERTEX*)pVertices;
			    
				for (int bidx = 0; bidx < cntBranch; bidx++) {
					branch[bidx]		= ptrBranches[bidx];
					branch[bidx].color	= 0xff7f652f;
				}

				g_pBranchesVB->Unlock();

				if( FAILED( g_pd3dDevice->CreateIndexBuffer( indexNum * sizeof(WORD),
															D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
															D3DPOOL_DEFAULT, &g_pBranchesIB, NULL ) ) )
				{
					return E_FAIL;
				}
				
				VOID* pIndices;
				if( FAILED( g_pBranchesIB->Lock( 0, 0, (void**)&pIndices, 0 ) ) )
					return E_FAIL;
					
				WORD *bindices = (WORD*)pIndices;
				
				for (unsigned int iidx = 0; iidx < indexNum; iidx++)
					bindices[iidx] = (WORD)indexbuffer[iidx];
					
				cntIndexBranch = indexNum;
				
				g_pBranchesIB->Unlock();
			}
			
			delete ptrBranches;
		}
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pBlockVB != NULL )        
        g_pBlockVB->Release();

    if( g_pLeavesVB != NULL )        
        g_pLeavesVB->Release();

    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )       
        g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Name: SetTfMatrix()
// Desc: Sets up transformation Matrix.
//-----------------------------------------------------------------------------
inline VOID SetTfMatrix(D3DXMATRIXA16 &tfMatrix, D3DXMATRIXA16 &matModel)
{
    D3DXVECTOR3		vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16	matView;

    D3DXMatrixLookAtLH( &matView, &g_vEyePt, &g_vLookatPt, &vUpVec );
    
	D3DXMatrixMultiply( &tfMatrix, &matModel, &matView);
}

//-----------------------------------------------------------------------------
// Name: SetLeavesTfMatrix()
// Desc: 
//-----------------------------------------------------------------------------
inline VOID SetLeavesTfMatrix(D3DXMATRIXA16 &tfMatrix)
{
    float BlockSize	= 1.1f;
    D3DXVECTOR4 blockPos( 0, 0, 0, 1);

    D3DXMATRIXA16 matProj;
    D3DXMATRIXA16 rtfMatrix;
    
    D3DXVec3Transform( &blockPos, &D3DXVECTOR3( 0, 0, 0), &tfMatrix);
    float BlockZ = blockPos.z;
    
    D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 1.0f, BlockZ-BlockSize/2, BlockZ+BlockSize*3/2 );
	D3DXMatrixMultiply( &rtfMatrix, &tfMatrix, &matProj);
		
    D3DXVec3Transform( &blockPos, &D3DXVECTOR3( 0, 0, 0), &rtfMatrix);
    blockPos	= blockPos / -blockPos.w;
    blockPos.w	= 4.0f;

	g_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&rtfMatrix, 4 );
	g_pd3dDevice->SetVertexShaderConstantF( 4, (float*)&blockPos, 1 );	
}


//-----------------------------------------------------------------------------
// Name: SetBlocksTfMatrix()
// Desc: 
//-----------------------------------------------------------------------------
inline VOID SetBlocksTfMatrix(D3DXMATRIXA16 &tfMatrix)
{
	D3DCOLOR TreeColor(0xffffffff);

    float BlockSize			= 1.f;
	float blockScreen		= 0.25f;
	float constans[4];
    D3DXMATRIXA16 matProj;
    D3DXMATRIXA16 rtfMatrix;

	D3DXVECTOR3 treePosP( 0, 0, 0);
    D3DXVec3TransformCoord( &treePosP, &treePosP, &tfMatrix);
    
    D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 1.0f, 1.0f, 1000.0f );
	D3DXMatrixMultiply( &rtfMatrix, &tfMatrix, &matProj);
		
	D3DXVECTOR3 treePosC = D3DXVECTOR3( fabsf( treePosP.x), fabsf( treePosP.y), treePosP.z);
	D3DXVECTOR3 treePosM = treePosC + D3DXVECTOR3( BlockSize/2.f, BlockSize/2.f, -BlockSize/2.f);
		
    D3DXVec3TransformCoord( &treePosC, &treePosC, &matProj);
    D3DXVec3TransformCoord( &treePosM, &treePosM, &matProj);
    treePosC = (treePosM - treePosC);
	
	float blockSize = max( treePosC.x, treePosC.y);
	if (blockSize > 0.25) blockSize = 0.25;
	if (treePosC.z >= 0) blockSize = 0;
	blockSize /= blockScreen;
	constans[0] = (blockSize * blockScreen * 2.f);
	constans[1] = -constans[0];
	constans[2] = (blockSize);
	constans[3] = -(blockSize) * 0.5f + 0.5f;

	D3DXVECTOR4 size( 0.5, 0, 0, 0);
	g_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&tfMatrix, 4 );	
	g_pd3dDevice->SetVertexShaderConstantF( 4, (float*)&matProj,  4 );	
	g_pd3dDevice->SetVertexShaderConstantF( 8, (float*)&constans, 1 );	
	g_pd3dDevice->SetVertexShaderConstantF( 9, (float*)&size, 1 );	
	float color[4];
	color[0] = ((TreeColor & 0xff0000) >> 16)/255.f;
	color[1] = ((TreeColor & 0xff00) >> 8)/255.f;
	color[2] = ((TreeColor & 0xff))/255.f;
	color[3] = 0;
	g_pd3dDevice->SetVertexShaderConstantF( 10, (float*)&color, 1 );
	float	data[4] = {0, 0, 1.f, 1.f};
	g_pd3dDevice->SetVertexShaderConstantF( 11, (float*)&data, 1 );	
	
	BlockSize = 2.f;
	treePosC = D3DXVECTOR3( 0, 0, 0);
		
    D3DXVec3TransformCoord( &treePosC, &treePosC, &tfMatrix);
	treePosM = treePosC + D3DXVECTOR3( 0, 0, BlockSize);
    D3DXVec3TransformCoord( &treePosC, &treePosC, &matProj);
    D3DXVec3TransformCoord( &treePosM, &treePosM, &matProj);
    BlockSize = treePosM.z - treePosC.z;
    
	g_pd3dDevice->SetPixelShaderConstantF( 1, (float*)&BlockSize, 1 );
}

//-----------------------------------------------------------------------------
// Name: SetObjectsTfMatrix()
// Desc: 
//-----------------------------------------------------------------------------
inline VOID SetObjectsTfMatrix(D3DXMATRIXA16 &tfMatrix)//, D3DXVECTOR3 &treePos)
{

    D3DXMATRIXA16 matProj;
    D3DXMATRIXA16 rtfMatrix;

    D3DXMatrixPerspectiveFovLH( &matProj, g_FOV, 1.0f, 1.0f, 1000.0f );
	D3DXMatrixMultiply( &rtfMatrix, &tfMatrix, &matProj);
		
	g_pd3dDevice->SetVertexShaderConstantF( 0, (float*)&rtfMatrix, 4 );
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID RenderTrees()
{
	// transformation matrix
	D3DXMATRIXA16 tfMatrix;

	// tree model matrix
	D3DXMATRIXA16	matTreeModel;
	D3DXMatrixIdentity( &matTreeModel);

	// set ModelView matrix
	SetTfMatrix( tfMatrix, matTreeModel);

	// get original render target
	LPDIRECT3DSURFACE9 origRenderTarget;
	if( FAILED( g_pd3dDevice->GetRenderTarget(  0, &origRenderTarget) ) )
	{
		return;
	}
	
	// get original depth buffer
	LPDIRECT3DSURFACE9 origDepthSurface;
	if( FAILED( g_pd3dDevice->GetDepthStencilSurface( &origDepthSurface) ) )
	{
		return;
	}

	{ // render impostor
		// set impostor texture to render target
		IDirect3DSurface9 *pRenderSurface;
		pLeavesRenderTarget->GetSurfaceLevel( 0, &pRenderSurface);
		if( FAILED( g_pd3dDevice->SetRenderTarget( 0, pRenderSurface) ) )
		{
			return;
		}

		if( FAILED( g_pd3dDevice->SetDepthStencilSurface( pLeavesDepthBuffer) ) )
		{
			return;
		}
		
		// Clear the back buffer
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0xff,0x50,0xbf,0x10), 1.0f, 0 );

		// set vertex and pixel shader
		g_pd3dDevice->SetVertexShader( pVShaderLeaves);
		g_pd3dDevice->SetVertexDeclaration( m_pLeavesVertexDeclaration );
		g_pd3dDevice->SetPixelShader( NULL);

		// set up impostor rendering matrix
		SetLeavesTfMatrix(tfMatrix);
		
		// render leaves	
		// set render settings
		g_pd3dDevice->SetVertexShader( pVShaderLeaves);
		g_pd3dDevice->SetVertexDeclaration( m_pLeavesVertexDeclaration );
		g_pd3dDevice->SetPixelShader( pPShaderLeaves);

		g_pd3dDevice->SetTexture( 0, NULL );
		g_pd3dDevice->SetStreamSource( 0, g_pLeavesVB, 0, sizeof(LEAVESVERTEX) );
		g_pd3dDevice->SetIndices( g_pLeavesIB);

		// Begin the scene
		if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
		{
			g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, cntpLeaves, 0, cntIndexLeaves/3 );

			// End the scene
			g_pd3dDevice->EndScene();
		}
	}

	// set original depth buffer and render target
	if( FAILED( g_pd3dDevice->SetRenderTarget( 0, origRenderTarget) ) )
	{
		return;
	}
	if( FAILED( g_pd3dDevice->SetDepthStencilSurface( origDepthSurface) ) )
	{
		return;
	}

	{ // render final image
		// Clear the backbuffer
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 0.999f, 0 );

		// Begin the scene
		if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
		{
			// render impostors
			// set vertex shader
			g_pd3dDevice->SetVertexShader( pVShaderTree);
			g_pd3dDevice->SetPixelShader( pPShaderTree);
			g_pd3dDevice->SetVertexDeclaration( m_pBlockVertexDeclaration );
			
			SetBlocksTfMatrix( tfMatrix);

			g_pd3dDevice->SetTexture( 0, pLeavesRenderTarget );
			g_pd3dDevice->SetStreamSource( 0, g_pBlockVB, 0, sizeof(BLOCKVERTEX) );

			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, cntBlock/3 );
		
			// render brench
			g_pd3dDevice->SetVertexShader( pVShaderObjects);
			g_pd3dDevice->SetPixelShader( pPShaderObjects);
			g_pd3dDevice->SetTexture( 0, NULL);
			g_pd3dDevice->SetStreamSource( 0, g_pBranchesVB, 0, sizeof(OBJVERTEX) );
			g_pd3dDevice->SetVertexDeclaration( m_pObjectsVertexDeclaration );
			g_pd3dDevice->SetIndices( g_pBranchesIB);

			SetObjectsTfMatrix( tfMatrix);

			g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, cntBranch, 0, cntIndexBranch/3 );

			// End the scene
			g_pd3dDevice->EndScene();
		}

		// Present the backbuffer contents to the display
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (doScreenshot)
	{
		IDirect3DSurface9* pBackSurface;
		g_pd3dDevice->GetRenderTarget(0, &pBackSurface);

		D3DSURFACE_DESC pDesc;
		pBackSurface->GetDesc( &pDesc);
		
		IDirect3DSurface9* pDestSurface;
		g_pd3dDevice->CreateOffscreenPlainSurface( pDesc.Width, pDesc.Height, pDesc.Format, D3DPOOL_SYSTEMMEM, &pDestSurface, NULL);
		
		g_pd3dDevice->GetRenderTargetData( pBackSurface, pDestSurface);

		D3DLOCKED_RECT LockedRect;
		pDestSurface->LockRect( &LockedRect, NULL, 0);
		
		char str[256];
		static int saveCnt = 0;
		sprintf(str, ".\\out%03d.bmp", saveCnt++);

		SaveToFile( str, pDesc.Width, pDesc.Height, (unsigned char *)LockedRect.pBits, 4, 1);
		
		pDestSurface->UnlockRect();
		pDestSurface->Release();
		pBackSurface->Release();

		doScreenshot = false;
	}
}



//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
        case WM_CHAR:
        {
			switch (wParam) {
//			case '1': 
//				if (g_FOV > (D3DX_PI / 10.f))
//					g_FOV -= D3DX_PI / 100.f;
//				break;
//			case '2': 
//				if (g_FOV < D3DX_PI / 3.f * 2.f)
//					g_FOV += D3DX_PI / 100.f;
//				break;
			case 'w': {
					D3DXVECTOR3 res = g_vLookatPt - g_vEyePt;
					res.y		= 0;
					g_vEyePt	+= res * 0.01f;
				} break;
			case 's': {
					D3DXVECTOR3 res = g_vLookatPt - g_vEyePt;
					res.y		= 0;
					g_vEyePt	-= res * 0.01f;
				} break;
			case 'r':
				g_vEyePt.y += 0.1f;
				break;
			case 'f':
				g_vEyePt.y -= 0.1f;
				break;
			case 'a': {
					D3DXMATRIXA16 matTmp;
					D3DXVECTOR3 res = g_vEyePt - g_vLookatPt;
					D3DXMatrixRotationY( &matTmp, 2 * D3DX_PI / 200);
				    D3DXVec3TransformCoord( &res, &res, &matTmp);
				    g_vEyePt = g_vLookatPt + res;
				} break;
			case 'd': {
					D3DXMATRIXA16 matTmp;
					D3DXVECTOR3 res = g_vEyePt - g_vLookatPt;
					D3DXMatrixRotationY( &matTmp, -2 * D3DX_PI / 200);
				    D3DXVec3TransformCoord( &res, &res, &matTmp);
				    g_vEyePt = g_vLookatPt + res;
				} break;
			case '`':
				doScreenshot = true;
				break;
			case '[':
				g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_FALSE );
				break;
			case ']':
				g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
				break;
			} 
        } break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE( IDC_MAINICON)), 
                      NULL, NULL, NULL, "D3D Tree Tutorial", NULL };
                      
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Tree Tutorial", "Tree",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1024+8, 1024+27,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the vertex buffer
        if( SUCCEEDED( InitVB() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else {
                    RenderTrees();
                }
            }
        }
    }

    UnregisterClass( "D3D Tree Tutorial", wc.hInstance );
    return 0;
}
