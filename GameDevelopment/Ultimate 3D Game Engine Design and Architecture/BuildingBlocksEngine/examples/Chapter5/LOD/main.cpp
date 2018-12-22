/*
   LOD Demo - Chapter 5
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<BBShapes.h>
#include<DynamicModel.h>
#include<ResourceManager.h>
#include<GeometryNode.h>
#include<SceneGraph.h>
#include<TextureMapEffect.h>


// Models to be displayed.
bbe::DynamicModel g_gameModel;

// Mesh handle and effect handle.
bbe::Handle<bbe::ModelResourceTag> g_boxDataHandle;

// Resource managers.
BB_MODEL_RESOURCE_MANAGER g_modelManager;

// Scene graph.
bbe::SceneGraph g_sceneGraph;

// Camera position.
bbe::Vector3D g_cameraPos(0, 0, -5), g_spherePos;

// Rendering System.
bbe::OpenGLRenderer g_Render;

// Quit flag.
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
   filters.push_back(BB_MIP_LINEAR_FILTER);


   bbe::ModelData *modelDataLOD0 = new bbe::ModelData();
   bbe::ModelData *modelDataLOD1 = new bbe::ModelData();
   bbe::ModelData *modelDataLOD2 = new bbe::ModelData();

   if(bbe::CreateSphereMesh(1, 20, 20, modelDataLOD0) == false)
      return false;

   if(bbe::CreateSphereMesh(1, 15, 15, modelDataLOD1) == false)
      return false;

   if(bbe::CreateSphereMesh(1, 8, 8, modelDataLOD2) == false)
      return false;

   g_gameModel.SetCurrentLOD(0);
   g_modelManager.Create(new bbe::ModelResource(modelDataLOD0), NULL,
      &g_boxDataHandle);
   g_gameModel.SetModelHandle(g_boxDataHandle);

   g_gameModel.SetCurrentLOD(1);
   g_modelManager.Create(new bbe::ModelResource(modelDataLOD1),
                         NULL, &g_boxDataHandle);
   g_gameModel.SetModelHandle(g_boxDataHandle);

   g_gameModel.SetCurrentLOD(2);
   g_modelManager.Create(new bbe::ModelResource(modelDataLOD2),
                         NULL, &g_boxDataHandle);
   g_gameModel.SetModelHandle(g_boxDataHandle);

   g_gameModel.SetCurrentLOD(0);

   bbe::GeometryNode *box1Node = new bbe::GeometryNode(&g_gameModel,
      &g_modelManager, NULL,
      static_cast<bbe::RenderInterface*>(&g_Render));

   g_sceneGraph.AddNode(box1Node);

   return true;
}


void DemoUpdate()
{
   int mouseX = 0, mouseY = 0;

   bbe::GetMousePosition(&mouseX, &mouseY);

   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      g_quitDemo = true;


   if(bbe::isButtonDown(BB_BUTTON_ARROW_UP))
      g_cameraPos.z += 0.25f;

   if(bbe::isButtonDown(BB_BUTTON_ARROW_DOWN))
      g_cameraPos.z -= 0.25f;


   if(g_cameraPos.z > -5)
      g_cameraPos.z = -5;

   float dist = g_cameraPos.Dot3(g_spherePos);

   if(dist < 0)
      dist *= -1;

   dist = sqrt(dist);

   if(dist > 40)
      g_gameModel.SetCurrentLOD(2);
   else if(dist > 20)
      g_gameModel.SetCurrentLOD(1);
   else
      g_gameModel.SetCurrentLOD(0);
}


void DemoRender()
{
	g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(g_cameraPos.x, g_cameraPos.y, g_cameraPos.z,
                    0, 0, 0, 0, 1, 0);

   g_sceneGraph.Process();

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();

   g_gameModel.SetCurrentLOD(0);
   g_modelManager.Release(g_gameModel.GetModelHandle());

   g_gameModel.SetCurrentLOD(1);
   g_modelManager.Release(g_gameModel.GetModelHandle());

   g_gameModel.SetCurrentLOD(2);
   g_modelManager.Release(g_gameModel.GetModelHandle());

   g_sceneGraph.Release();
}