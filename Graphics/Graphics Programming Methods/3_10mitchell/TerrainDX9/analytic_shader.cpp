// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "terrain.h"
#include "analytic_shader.h"

Analytic_Shader::Analytic_Shader()
: Base_Texture_Projection(INV_PATCH_WORLD_WIDTH,INV_PATCH_WORLD_WIDTH,0.0f,0.0f),
  Detail_Texture_Projection(1.0f/16.0f,1.0f/16.0f,0.0f,0.0f),
  Cloud_Map_Projection(INV_PATCH_WORLD_WIDTH,INV_PATCH_WORLD_WIDTH,0.5f,0.0f),
  Fog_Parameters(1.0f/860.0f, 900.0f/860.0f, 0.0f,0.0f),
  Average_Sky_Light(0.6f,0.6f,1.0f,1.0f),
  Sun_Light_Direction(0.0f,0.0f,0.0f,1.0f),
  Sun_Light_Color(1.0f,0.95f,0.8f,1.0f),
  Sky_Light_Factor(1.0f),
  Sun_Light_Factor(1.0f),
  Shadows_Enabled(true),
  Active_Light(true),
  Enable_Terrain_Texture(true),
  Enable_NH_Texture(true),
  Enable_Detail_Texture(true),
  Enable_Clouds_Texture(true),
  Terrain_Texture(NULL),
  Positive_Normal_Horizon_Map_Texture(NULL),
  Negative_Normal_Horizon_Map_Texture(NULL),
  White_Texture(NULL),
  Clouds_Texture(NULL)
{
	// Create textures
	HRESULT result=D3DXCreateTextureFromFile
	(
		DEV,
		"terrain7.dds",
		&Terrain_Texture
	);
	FAILMSG("Failed to create terrain texture");

	// Create textures
	result=D3DXCreateTextureFromFile
	(
		DEV,
		"terrain_detail1.dds",
		&Detail_Texture
	);
	FAILMSG("Failed to create terrain detail texture");

	result=D3DXCreateTextureFromFile
	(
		DEV,
		"clouds.bmp",
		&Clouds_Texture
	);
	FAILMSG("Failed to create clouds texture");

	result=D3DXCreateTextureFromFile
	(
		DEV,
		"pnh7.dds",
		&Positive_Normal_Horizon_Map_Texture
	);
	FAILMSG("Failed to create positive normal horizon map texture");

	result=D3DXCreateTextureFromFile
	(
		DEV,
		"nnh7.dds",
		&Negative_Normal_Horizon_Map_Texture
	);
	FAILMSG("Failed to create negative normal horizon map texture");

	result=D3DXCreateTextureFromFile
	(
		DEV,
		"white.bmp",
		&White_Texture
	);
	FAILMSG("Failed to create white texture");

	// Create vertex shader
	Create_Analytic_Vertex_Shader();

	// Create pixel shader
	Create_Pixel_Shader("analytic.psh");

	OutputDebugString("Compiled pixel shader\n");
}

Analytic_Shader::~Analytic_Shader()
{
	Safe_Release(Terrain_Texture);
	Safe_Release(Detail_Texture);
	Safe_Release(Clouds_Texture);
	Safe_Release(Positive_Normal_Horizon_Map_Texture);
	Safe_Release(Negative_Normal_Horizon_Map_Texture);
	Safe_Release(White_Texture);
}

void Analytic_Shader::Apply()
{
static float fTime=D3DX_PI/4.0f;

	Input_Class* input=Application::Get_Input();


	// show shader stats
	char string[255];
	CD3DFont* font=Application::Get_Font();

	sprintf(string,"T/G - Sun Angle %f degrees", D3DXToDegree(fmodf(fTime+D3DX_PI/2.0f,D3DX_PI*2.0f)));
	font->DrawText(10,100,string);

	// control sky light contribution
	if (input->Get_Key('Z') && Sun_Light_Factor<1.99f) Sun_Light_Factor+=0.01f;
	if (input->Get_Key('X') && Sun_Light_Factor>0.01f) Sun_Light_Factor-=0.01f;

	if (input->Get_Key('C') && Sky_Light_Factor<1.99f) Sky_Light_Factor+=0.01f;
	if (input->Get_Key('V') && Sky_Light_Factor>0.01f) Sky_Light_Factor-=0.01f;

	sprintf(string,"Z/X - Sun Light Factor %1.3f", Sun_Light_Factor);
	font->DrawText(10,120,string);

	sprintf(string,"C/V - Sky Light Factor %1.3f", Sky_Light_Factor);
	font->DrawText(10,140,string);

	// control texture stages
	if (input->Get_Key('1')) Enable_Terrain_Texture=true;
	if (input->Get_Key('2')) Enable_Terrain_Texture=false;
	if (input->Get_Key('3')) Enable_Detail_Texture=true;
	if (input->Get_Key('4')) Enable_Detail_Texture=false;
	if (input->Get_Key('5')) Enable_NH_Texture=true;
	if (input->Get_Key('6')) Enable_NH_Texture=false;
	if (input->Get_Key('7')) Enable_Clouds_Texture=true;
	if (input->Get_Key('8')) Enable_Clouds_Texture=false;

	if (Enable_Terrain_Texture)
	{
		font->DrawText(10,160,"1/2 - Toggle Terrain & Sky Radiance Texture (on)");
	}
	else
	{
		font->DrawText(10,160,"1/2 - Toggle Terrain & Sky Radiance Texture (off)");
	}
	
	if (Enable_Detail_Texture)
	{
		font->DrawText(10,180,"3/4 - Toggle Detail Color & Detail Horizon Map Texture (on)");
	}
	else
	{
		font->DrawText(10,180,"3/4 - Toggle Detail Color & Detail Horizon Map Texture (off)");
	}
	
	if (Enable_NH_Texture)
	{
		font->DrawText(10,200,"5/6 - Toggle Normal Map & Horizon Map Texture (on)");
	}
	else
	{
		font->DrawText(10,200,"5/6 - Toggle Normal Map & Horizon Map Texture (off)");
	}
	
	if (Enable_Clouds_Texture)
	{
		font->DrawText(10,220,"7/8 - Toggle Clouds Texture (on)");
	}
	else
	{
		font->DrawText(10,220,"7/8 - Toggle Clouds Texture (off)");
	}
	

	// update time
	if (input->Get_Key('T')) fTime+=0.01f;
	if (input->Get_Key('G')) fTime-=0.01f;

	if (fTime>2.0f*D3DX_PI) fTime=0.0f;
	else if (fTime<0.0f) fTime=2.0f*D3DX_PI;

	bool positive_horizon=fTime>0.0f && fTime<D3DX_PI;

	// set sun light direction
	D3DXVECTOR3 dir(0.5f,sinf(fTime),cosf(fTime));

	D3DXVec3Normalize(&dir,&dir);

	Sun_Light_Direction.x=dir.x;
	Sun_Light_Direction.y=dir.y;
	Sun_Light_Direction.z=dir.z;

	Sun_Light_Direction.w=positive_horizon ? Sun_Light_Direction.y : -Sun_Light_Direction.y;

	// set textures
	DEV->SetTexture(0, Enable_Terrain_Texture ? Terrain_Texture : White_Texture);
	DEV->SetTexture(1, Enable_Detail_Texture ? Detail_Texture : White_Texture);
	DEV->SetTexture(2, Enable_NH_Texture ? (positive_horizon ? Positive_Normal_Horizon_Map_Texture : Negative_Normal_Horizon_Map_Texture) : White_Texture);
	DEV->SetTexture(3, Enable_Clouds_Texture ? Clouds_Texture : White_Texture);

	// normal/horizon map clamp
	DEV->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	DEV->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	DEV->SetTransform(D3DTS_VIEW, Application::Get_Camera()->Get_View_Matrix());


    DEV->SetVertexDeclaration(Vertex_Declaration);
	DEV->SetVertexShader(Vertex_Shader);

	// set vertex shader constants

	/////////////////////////////////////////////////////////////////////////////////////
	// set up texture projection

	DEV->SetVertexShaderConstantF(CV_BASE_TEX_PROJECTION_SLOT, Base_Texture_Projection, 1);
	DEV->SetVertexShaderConstantF(CV_DETAIL_TEX_PROJECTION_SLOT, Detail_Texture_Projection, 1);

	// animate cloud map offset
	Cloud_Map_Projection.z+=0.0025f;

	// wrap to avoid possible texture coordinate range problems
	if (Cloud_Map_Projection.z>TEX_COORD_RANGE) Cloud_Map_Projection.z-=TEX_COORD_RANGE;

	DEV->SetVertexShaderConstantF(CV_CLOUD_TEX_PROJECTION_SLOT, Cloud_Map_Projection, 1);
	/////////////////////////////////////////////////////////////////////////////////////

	// set fog constants
	DEV->SetVertexShaderConstantF(CV_FOG_PARAMETERS_SLOT, Fog_Parameters, 1);

	// set constants
	DEV->SetVertexShaderConstantF(CV_CONST_SLOT, D3DXVECTOR4(0.0f, 1.0f, 0.5f, 2.0f), 1);

	// set pixel shader constants
	D3DXVECTOR4 average_sky_light(Average_Sky_Light);
	average_sky_light.x*=Sky_Light_Factor;
	average_sky_light.y*=Sky_Light_Factor;
	average_sky_light.z*=Sky_Light_Factor;
	DEV->SetPixelShaderConstantF(CP_SKYLIGHT_SLOT,average_sky_light,1);


	DEV->SetPixelShaderConstantF(CP_SUNLIGHT_SLOT,Sun_Light_Direction,1);

	D3DXVECTOR4 sun_light_color(Sun_Light_Color);
	sun_light_color.x*=Sun_Light_Factor;
	sun_light_color.y*=Sun_Light_Factor;
	sun_light_color.z*=Sun_Light_Factor;
	DEV->SetPixelShaderConstantF(CP_SUNCOLOR_SLOT,sun_light_color,1);

	// set constant
	DEV->SetPixelShaderConstantF(CP_CONST_SLOT, D3DXVECTOR4(-4.0f/3.1495f, 1.0f, 0.5f, 2.0f), 1);

	// set pixel shader
	DEV->SetPixelShader(Pixel_Shader);
}

void Analytic_Shader::Update_Instance()
{
	// set up camera
	D3DXMATRIX* view_proj_matrix=Application::Get_Camera()->Get_View_Projection_Matrix();
	D3DXMATRIX world_view_proj_matrix;
	D3DXMATRIX world_matrix;

	DEV->GetTransform(D3DTS_WORLD,&world_matrix);
	
	D3DXMatrixMultiply
	(
		&world_view_proj_matrix,
		&world_matrix, 
		view_proj_matrix
	);

	D3DXMatrixTranspose(&world_view_proj_matrix, &world_view_proj_matrix);
	DEV->SetVertexShaderConstantF(CV_VIEW_PROJECTION_SLOT, world_view_proj_matrix, 4);
}

void Analytic_Shader::Create_Analytic_Vertex_Shader()
{
	// Create the shader declaration.
	D3DVERTEXELEMENT9 decl[] = 
	{
	  { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	  D3DDECL_END()
	};

	HRESULT result=DEV->CreateVertexDeclaration( decl, &Vertex_Declaration );
	FAILMSG("Failed to create vertex shader declaration");

	Create_Vertex_Shader("analytic.vsh");

	OutputDebugString("Compiled vertex shader\n");
}

