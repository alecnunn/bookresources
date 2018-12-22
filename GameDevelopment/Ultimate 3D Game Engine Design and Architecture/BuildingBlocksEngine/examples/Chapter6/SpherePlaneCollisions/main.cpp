/*
   Sphere and Plane Collisions with Point Masses Demo - Chapter 6
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
#include<PointMass.h>
#include<CollisionResponse.h>
#include<Plane.h>
#include<Timer.h>


// Models to be displayed.
bbe::DynamicModel g_object1, g_object2, g_room;

// Object's Physics informaiton.
bbe::PointMass g_obj1Mass, g_obj2Mass;

// Room bounds.
std::vector<bbe::Plane> g_roomPlanes;

// Time values for real time simulation.
float g_lastTime = 0, g_currentTime = 0;

// Resource managers.
BB_MODEL_RESOURCE_MANAGER g_modelManager(32);
BB_EFFECT_RESOURCE_MANAGER g_effectManager(32);

// Scene graph.
bbe::SceneGraph g_sceneGraph;

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
   g_Render.Enable(BB_BACK_CCW_CULLING);


   // Create models.
   bbe::ModelData *object1Mesh = new bbe::ModelData();
   bbe::ModelData *object2Mesh = new bbe::ModelData();
   bbe::ModelData *roomMesh = new bbe::ModelData();

   if(bbe::CreateSphereMesh(5, 15, 15, object1Mesh) == false)
      return false;

   if(bbe::CreateSphereMesh(5, 15, 15, object2Mesh) == false)
      return false;

   if(bbe::CreateCubeMesh(50, roomMesh, &g_roomPlanes) == false)
      return false;

   // Resource manage.
   bbe::Handle<bbe::ModelResourceTag> modelHandle;

   g_modelManager.Create(new bbe::ModelResource(object1Mesh),
      "OBJECT 1", &modelHandle);

   g_object1.SetModelHandle(modelHandle);

   g_modelManager.Create(new bbe::ModelResource(object2Mesh),
      "OBJECT 2", &modelHandle);

   g_object2.SetModelHandle(modelHandle);

   g_modelManager.Create(new bbe::ModelResource(roomMesh),
      "ROOM", &modelHandle);

   g_room.SetModelHandle(modelHandle);


   // Load textures and effects.
   std::vector<BB_FILTER_TYPE> filters;
   filters.push_back(BB_MIN_LINEAR_FILTER);
   filters.push_back(BB_MAG_LINEAR_FILTER);

   bbe::Effect *effect1 = NULL;
   effect1 = new bbe::TextureShader("resources/vs.glsl",
      "resources/ps.glsl", "resources/decal.tga", &filters,
      static_cast<bbe::RenderInterface*>(&g_Render));

   if(effect1 == NULL)
      return false;

   bbe::Effect *effect2 = NULL;
   effect2 = new bbe::TextureShader("resources/vs.glsl",
      "resources/ps.glsl", "resources/decal2.tga", &filters,
      static_cast<bbe::RenderInterface*>(&g_Render));

   if(effect2 == NULL)
      return false;

   // Resource manage.
   bbe::Handle<bbe::EffectResourceTag> effectHandle;

   g_effectManager.Create(new bbe::EffectResource(effect1),
      "effect 1", &effectHandle);

   g_object1.SetEffectHandle(effectHandle);

   g_effectManager.Create(new bbe::EffectResource(effect2),
      "effect 2", &effectHandle);

   g_object2.SetEffectHandle(effectHandle);

   // Copy handle to the same resource for the room.
   g_room.SetEffectHandle(effectHandle);


   // Scene graph.
   bbe::GeometryNode *object1Node = NULL, *object2Node = NULL,
                     *roomNode = NULL;

   object1Node = new bbe::GeometryNode(&g_object1,
      &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   object2Node = new bbe::GeometryNode(&g_object2,
      &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   roomNode = new bbe::GeometryNode(&g_room,
      &g_modelManager, &g_effectManager,
      static_cast<bbe::RenderInterface*>(&g_Render));

   g_sceneGraph.AddNode(object1Node);
   g_sceneGraph.AddNode(object2Node);
   g_sceneGraph.AddNode(roomNode);


   // Physics init.
   g_obj1Mass.SetPosition(bbe::Vector3D(-40, 30, 0));
   g_obj1Mass.SetVelocity(bbe::Vector3D(15, -25, 0));
   g_obj1Mass.SetMass(1);

   g_obj2Mass.SetPosition(bbe::Vector3D(-40, -30, 0));
   g_obj2Mass.SetVelocity(bbe::Vector3D(15, 25, 0));
   g_obj2Mass.SetMass(1);

   g_lastTime = bbe::GetTimeSeconds();

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


   // Update time.
   g_currentTime = bbe::GetTimeSeconds();
   float dt = g_currentTime - g_lastTime;


   // Update object 1.
   g_obj1Mass.Update(dt);
   g_object1.SetTransformation(g_obj1Mass.GetTransformation());

   // Update object 2;
   g_obj2Mass.Update(dt);
   g_object2.SetTransformation(g_obj2Mass.GetTransformation());


   // Respond to any collisions between the balls.
   bbe::CollisionResponse collisionSolver;

   collisionSolver.ResponseToCollision(&g_obj1Mass, 5,
      &g_obj2Mass, 5, 0.001f);

   // Respond to any collisions against the room.
   for(int i = 0; i < (int)g_roomPlanes.size(); i++)
      {
         collisionSolver.ResponseToCollision(&g_obj1Mass, 5,
            g_roomPlanes[i], 5.5f);

         collisionSolver.ResponseToCollision(&g_obj2Mass, 5,
            g_roomPlanes[i], 5.5f);
      }

   g_lastTime = bbe::GetTimeSeconds();
}


void DemoRender()
{
	g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(0, 0, 200, 0, 0, 0, 0, 1, 0);
   g_Render.RotateMatrix((float)g_xRot, 0, 1, 0);
   g_Render.RotateMatrix((float)g_yRot, 1, 0, 0);

   g_sceneGraph.Process();

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();

   g_effectManager.Release(g_object1.GetEffectHandle());
   g_modelManager.Release(g_object1.GetModelHandle());

   g_effectManager.Release(g_object2.GetEffectHandle());
   g_modelManager.Release(g_object2.GetModelHandle());

   g_effectManager.Release(g_room.GetEffectHandle());
   g_modelManager.Release(g_room.GetModelHandle());

   g_sceneGraph.Release();
}