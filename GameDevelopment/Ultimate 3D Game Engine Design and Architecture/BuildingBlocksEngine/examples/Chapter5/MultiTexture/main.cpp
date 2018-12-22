/*
   Multi-Texture Demo - Chapter 5
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<OpenGL.h>
#include<Input.h>


// Single vertex.
struct stVertex
{
   float x, y, z;
   float tu1, tv1;
};


// Square Model.
stVertex g_squareData[] =
{
    1, -1, -1,  1, 0,
   -1, -1, -1,  0, 0,
   -1,  1, -1,  0, 1,
    1,  1, -1,  1, 1
};

// Square Model's indices.
unsigned int g_squareIndices[] = { 0, 1, 2, 2, 3, 0 };
int g_numSquareIndices = 6;


// Vertex descriptor for the models.
bbe::VertexDescriptor g_desc;

// Textures.
bbe::BB_TEXTURE g_tex;
bbe::BB_TEXTURE g_tex2;

// Shader ID.
bbe::ShaderHandle g_shader;

// Shader parameters.
bbe::ParameterHandle g_decal1Param;
bbe::ParameterHandle g_decal2Param;

// Scene rotations.
int g_xRot = 0, g_oldXRot = 0;
int g_yRot = 0, g_oldYRot = 0;

// Rendering System.
bbe::OpenGLRenderer g_Render;

// Flag used to quit.
bool g_quitDemo = false;


void DemoResize(int width, int height)
{
   g_Render.SetViewPort(0, 0, width, height);
   g_Render.ResizeWindow(45.0f, 0.1f, 1000, width, height);
}


bool DemoInitialize(bbe::RenderParams &params)
{
   params.m_colorBits = 24;
   params.m_depthBits = 16;
   params.m_stencilBits = 8;
   params.m_fullscreen = WINDOW_FULLSCREEN;
   params.m_height = WINDOW_HEIGHT;
   params.m_width = WINDOW_WIDTH;
   params.m_maxCacheBytes = 2000;
   params.m_maxCacheIndices = 2000;

   if(g_Render.Initialize(&params) != BB_SUCCESS)
      return false;

   DemoResize(WINDOW_WIDTH, WINDOW_HEIGHT);


   g_Render.SetClearColor(0, 0, 0, 255);
   g_Render.Enable(BB_DEPTH_TESTING);
   g_Render.Enable(BB_SMOOTH_SHADING);
   g_Render.Enable(BB_TEXTURE_2D);


   std::vector<BB_FILTER_TYPE> filters;
   filters.push_back(BB_MIN_LINEAR_FILTER);
   filters.push_back(BB_MAG_LINEAR_FILTER);


   g_desc.AddElement(BB_ELEMENT_TYPE_VERTEX_3F);
   g_desc.AddElement(BB_ELEMENT_TYPE_TEX1_2F);


   if(!g_Render.LoadTexFromFile("resources/image.tga", BB_TEX2D_TYPE,
      &g_tex))
      {
         return false;
      }

   g_Render.ApplyFilters(g_tex, &filters);


   if(!g_Render.LoadTexFromFile("resources/image2.tga", BB_TEX2D_TYPE,
      &g_tex2))
      {
         return false;
      }

   g_Render.ApplyFilters(g_tex2, &filters);


   if(!g_Render.CreateShaderFromFile("resources/vs.glsl",
      "resources/ps.glsl", &g_shader))
      return false;

   g_Render.SetupShaderParameter("decal", g_shader, &g_decal1Param);
   g_Render.SetupShaderParameter("decal2", g_shader, &g_decal2Param);

   return true;
}


void DemoUpdate()
{
   int mouseX = 0, mouseY = 0;

   bbe::GetMousePosition(&mouseX, &mouseY);

   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      g_quitDemo = true;

   if(bbe::isButtonDown(BB_BUTTON_MOUSE_LEFT))
		{
         g_xRot -= (mouseX - g_oldXRot);
			g_yRot -= (mouseY - g_oldYRot);
		}

	g_oldXRot = mouseX;
	g_oldYRot = mouseY;
}


void DemoRender()
{
	g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(0, 0, 5, 0, 0, 0, 0, 1, 0);
   g_Render.RotateMatrix((float)g_xRot, 0, 1, 0);
   g_Render.RotateMatrix((float)g_yRot, 1, 0, 0);

   g_Render.ApplyShader(g_shader);

   g_Render.SetShaderParameter1i(g_decal1Param, 0);
   g_Render.SetShaderParameter1i(g_decal2Param, 1);

   g_Render.ApplyTexture(0, g_tex);
   g_Render.ApplyTexture(1, g_tex2);

   g_Render.Render(BB_PRIMITIVE_TRI_LIST, &g_desc,
                   (char*)g_squareData, g_squareIndices,
                   sizeof(g_squareData), g_numSquareIndices,
                   sizeof(stVertex));

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();
}