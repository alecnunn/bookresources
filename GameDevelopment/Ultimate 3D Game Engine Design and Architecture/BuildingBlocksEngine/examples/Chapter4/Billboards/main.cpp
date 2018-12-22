/*
   Billboards - Chapter 4
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<Vector3D.h>

// Vertex structure.
struct stVertex
{
   bbe::Vector3D pos;
   float tu, tv;
};

// Rendering System.
bbe::OpenGLRenderer g_Render;

// Decal.
bbe::BB_TEXTURE g_tex;

// Vertex Descriptor.
bbe::VertexDescriptor g_descriptor;

// Flag used to quit.
bool g_quitDemo = false;


void DemoResize(int width, int height)
{
   g_Render.SetViewPort(0, 0, width, height);
   g_Render.ResizeWindow(45.0f, 0.1f, 1000, width, height);
}


bool DemoInitialize(bbe::RenderParams &params)
{
   bool result = false;

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

   result = g_Render.LoadTexFromFile("resources/textures/decal.tga",
                                     BB_TEX2D_TYPE, &g_tex);

   if(result == false)
      return false;

   g_Render.ApplyFilters(g_tex, &filters);

   g_descriptor.AddElement(BB_ELEMENT_TYPE_VERTEX_3F);
   g_descriptor.AddElement(BB_ELEMENT_TYPE_TEX1_2F);

   return true;
}


void DemoUpdate()
{
   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      g_quitDemo = true;
}


void DemoRender()
{
   g_Render.SetClearColor(0, 0, 0, 255);

   g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(10, 0, 4, 0, 0, 0, 0, 1, 0);
   g_Render.ApplyTexture(0, g_tex);

   float matrix[16], size = 1;

   g_Render.GetMatrix(BB_MODELVIEW_MATRIX, matrix);

   bbe::Vector3D right(matrix[0], matrix[4], matrix[8]),
                 top(matrix[1], matrix[5], matrix[9]);

   stVertex billBoardVerts[6] =
   {
      { (right - top) * size, 1, 0 },
      { (right + top) * -size, 0, 0 },
      { (top - right) * size, 0, 1 },
      { (top - right) * size, 0, 1 },
      { (right + top) * size, 1, 1 },
      { (right - top) * size, 1, 0 }
   };

   g_Render.Render(BB_PRIMITIVE_TRI_LIST, &g_descriptor,
                   (char*)&billBoardVerts, NULL,
                   g_descriptor.GetStride() * 6, 0,
                   g_descriptor.GetStride());

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();
}