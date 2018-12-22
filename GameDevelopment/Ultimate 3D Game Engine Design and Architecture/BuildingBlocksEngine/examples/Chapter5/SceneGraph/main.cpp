/*
   Scene Graph Demo - Chapter 5
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<OpenGL.h>
#include<Input.h>
#include<BBShapes.h>
#include<DynamicModel.h>
#include<ResourceManager.h>
#include<TransformationNode.h>
#include<GeometryNode.h>
#include<SceneGraph.h>
#include<TextureMapEffect.h>


// Rendering System.
bbe::OpenGLRenderer g_Render;

// Models to be displayed.
bbe::DynamicModel g_gameModel[4];

// Mesh handle and effect handle.
bbe::Handle<bbe::ModelResourceTag> g_boxDataHandle;
bbe::Handle<bbe::EffectResourceTag> g_tex1EffectHandle;

// Resource managers.
BB_MODEL_RESOURCE_MANAGER g_modelManager;
BB_EFFECT_RESOURCE_MANAGER g_effectManager;

// Scene graph.
bbe::SceneGraph g_sceneGraph;

// Scene rotations.
int g_xRot = 0, g_oldXRot = 0;
int g_yRot = 0, g_oldYRot = 0;

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

   bbe::ModelData *modelData = new bbe::ModelData();

   if(bbe::CreateUnitCubeMesh(modelData) == false)
      return false;

   std::vector<BB_FILTER_TYPE> filters;
   filters.push_back(BB_MIN_LINEAR_FILTER);
   filters.push_back(BB_MAG_LINEAR_FILTER);

   bbe::Effect *effect = new bbe::TextureShader(
      "resources/vs.glsl", "resources/ps.glsl",
      "resources/decal.tga", &filters,
      static_cast<bbe::RenderInterface*>(&g_Render));

   g_modelManager.Create(new bbe::ModelResource(modelData),
                         "Box 1", &g_boxDataHandle);

   g_effectManager.Create(new bbe::EffectResource(effect),
                          "Box 1 Tex Map", &g_tex1EffectHandle);

   // All models use the same data so can just copy.
   g_gameModel[0].SetModelHandle(g_boxDataHandle);
   g_gameModel[1].SetModelHandle(g_boxDataHandle);
   g_gameModel[2].SetModelHandle(g_boxDataHandle);
   g_gameModel[3].SetModelHandle(g_boxDataHandle);

   // All models use the same data so can just copy.
   g_gameModel[0].SetEffectHandle(g_tex1EffectHandle);
   g_gameModel[1].SetEffectHandle(g_tex1EffectHandle);
   g_gameModel[2].SetEffectHandle(g_tex1EffectHandle);
   g_gameModel[3].SetEffectHandle(g_tex1EffectHandle);

   bbe::TransformationNode *group1 =
      new bbe::TransformationNode(bbe::Vector3D(-5, 0, 0),
      bbe::Quaternion(), static_cast<bbe::RenderInterface*>(&g_Render));

   bbe::TransformationNode *group2 =
      new bbe::TransformationNode(bbe::Vector3D(5, 0, 0), bbe::Quaternion(),
      static_cast<bbe::RenderInterface*>(&g_Render));

   bbe::Matrix4x4 boxPos;

   boxPos.Translate(0, 0, -5);
   g_gameModel[0].SetTransformation(boxPos);

   boxPos.Translate(0, 0, 5);
   g_gameModel[1].SetTransformation(boxPos);

   boxPos.Translate(0, 0, -5);
   g_gameModel[2].SetTransformation(boxPos);

   boxPos.Translate(0, 0, 5);
   g_gameModel[3].SetTransformation(boxPos);

   bbe::GeometryNode *box1Node = new bbe::GeometryNode(
      &g_gameModel[0], &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   bbe::GeometryNode *box2Node = new bbe::GeometryNode(
      &g_gameModel[1], &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   bbe::GeometryNode *box3Node = new bbe::GeometryNode(
      &g_gameModel[2], &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   bbe::GeometryNode *box4Node = new bbe::GeometryNode(
      &g_gameModel[3], &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   group1->AddChild(box1Node);
   group1->AddChild(box2Node);

   group2->AddChild(box3Node);
   group2->AddChild(box4Node);

   g_sceneGraph.AddNode(group1);
   g_sceneGraph.AddNode(group2);

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

   g_Render.SetView(0, 5, 20, 0, 0, 0, 0, 1, 0);
   g_Render.RotateMatrix((float)g_xRot, 0, 1, 0);
   g_Render.RotateMatrix((float)g_yRot, 1, 0, 0);

   g_sceneGraph.Process();

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();

   for(int i = 0; i < 4; i++)
      {
         g_effectManager.Release(g_gameModel[i].GetEffectHandle());
         g_modelManager.Release(g_gameModel[i].GetModelHandle());
      }

   g_effectManager.Release(g_tex1EffectHandle);
   g_modelManager.Release(g_boxDataHandle);

   g_sceneGraph.Release();
}