/*
   Cloth Simulation Demo - Chapter 6
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<BBShapes.h>
#include<Timer.h>
#include<PointMass.h>
#include<Spring.h>
#include<PhysicsSpace.h>
#include<CollisionResponse.h>
#include"Cloth.h"


// Models to be displayed.
bbe::ModelData g_object;

// Main sphere object's position and radius.
// The sphere is user controlled.
float g_spherePos[3] = { 0, -2, 0 };
float g_sphereRadius = 3.0f;

// Dynamic cloth.
Cloth g_cloth;

// Shader ID.
bbe::ShaderHandle g_shader;

// Shader parameters.
bbe::ParameterHandle g_lightPos;
bbe::ParameterHandle g_lightCol;

// Physical space in which the object lives.
bbe::PhysicsSpace g_3dSpace;

// Time values for real time simulation.
float g_lastTime = 0, g_currentTime = 0;

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


   // Create meshes.
   result = bbe::CreateSphereMesh(g_sphereRadius, 15, 15, &g_object);

   if(result == false)
      return false;


   // Load shader.
   result = g_Render.CreateShaderFromFile("resources/vs.glsl",
                                          "resources/ps.glsl",
                                          &g_shader);

   if(result == false)
      return false;

   g_Render.SetupShaderParameter("lightPos", g_shader, &g_lightPos);
   g_Render.SetupShaderParameter("lightColor", g_shader, &g_lightCol);


   // Create the cloth.
   int gridSize = 10;

   if(!g_cloth.Initialize(gridSize, 0.01f, 15.0f, 1.0f, 0.85f))
      return false;

   // Set the cloth grid to a default location.
   int index = 0;
   bbe::Vector3D pos;
   bbe::SpringMass *ptr = g_cloth.GetMasses();
	
   // Create flat grid.
	for(int i = 0; i < gridSize; ++i)
	   {
		   for(int j = 0; j < gridSize; ++j)
		      {
               index = i * gridSize + j;

               pos.x = (float)j - (gridSize - 1) / 2;
			      pos.y = 5.0f;
			      pos.z = (float)i - (gridSize - 1) / 2;

			      ptr[index].SetIsFixed(false);
			      ptr[index].SetPosition(pos);
		      }
	   }

   // Fix location for two of the corners.
	ptr[0].SetIsFixed(true);
	ptr[gridSize - 1].SetIsFixed(true);


   // Physics initialize.
   g_3dSpace.SetIsAirAtmosphere(true);
   g_3dSpace.SetGravity(bbe::Vector3D(0.0f, -0.98f, 0.0f));

   g_lastTime = bbe::GetTimeSeconds();

   return true;
}


void DemoUpdate()
{
   int mouseX = 0, mouseY = 0;

   bbe::GetMousePosition(&mouseX, &mouseY);

   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      g_quitDemo = true;

   if(bbe::isButtonDown(BB_BUTTON_ARROW_UP))
      g_spherePos[2] -= 0.05f;

   if(bbe::isButtonDown(BB_BUTTON_ARROW_DOWN))
      g_spherePos[2] += 0.05f;

   if(bbe::isButtonDown(BB_BUTTON_MOUSE_LEFT))
		{
         g_xRot -= (mouseX - g_oldXRot);
			g_yRot -= (mouseY - g_oldYRot);
		}

	g_oldXRot = mouseX;
	g_oldYRot = mouseY;


   // Update physics.
   bbe::CollisionResponse collisionSolver;
   bbe::PointMass sphere;
   sphere.SetMass(1);

   // Update time.
   g_currentTime = bbe::GetTimeSeconds();
   float dt = g_currentTime - g_lastTime;

   // Update all spring forces.
   g_cloth.UpdateSprings();

   // Update all point masses of the soft body.
   bbe::SpringMass *ptr = g_cloth.GetMasses();

   for(int i = 0; i < g_cloth.GetTotalMasses(); ++i)
	   {
	      // Allow for some points not to move.
         if(ptr[i].isFixed())
			   {
				   ptr[i].SetVelocity(bbe::Vector3D());
			   }
			else
			   {
			      // Apply gravity.
               g_3dSpace.InfluenceObject(&ptr[i], 1);

               // Update all forces that are on the point mass.
               ptr[i].Update(dt * 0.8f);
            }
	   }

   // Apply collisions with the cloth and the main sphere.
   for(int i = 0; i < g_cloth.GetTotalMasses(); ++i)
	   {
         collisionSolver.ResponseToCollision(&sphere, g_sphereRadius,
            &ptr[i], 0.1f, 0.06f);

         // Keep sphere un-moveable.
         sphere.SetPosition(bbe::Vector3D(g_spherePos[0],
                            g_spherePos[1], g_spherePos[2]));
         sphere.SetVelocity(bbe::Vector3D(0, 0, 0));
	   }

   // Update mesh and normals.
   g_cloth.RecalculateMesh();

   g_lastTime = bbe::GetTimeSeconds();
}


void DemoRender()
{
	g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(0, 5, 30, 0, 0, 0, 0, 1, 0);
   g_Render.RotateMatrix((float)g_xRot, 0, 1, 0);
   g_Render.RotateMatrix((float)g_yRot, 1, 0, 0);

   g_Render.ApplyShader(g_shader);

   g_Render.SetShaderParameter4f(g_lightPos, 0, 10, 10, 0);
   g_Render.SetShaderParameter4f(g_lightCol, 0.8f, 0.8f, 0.8f, 0);


   // Render cloth.
   g_Render.Render(BB_PRIMITIVE_TRI_LIST, &g_cloth.GetDescriptor(),
                   (char*)g_cloth.GetVertices(), NULL,
                   g_cloth.GetVertexSizeInBytes(), 0,
                   g_cloth.GetVertexStride());


   // Render sphere object.
   g_Render.PushMatrix();

      g_Render.TranslateMatrix(g_spherePos[0],
                               g_spherePos[1],
                               g_spherePos[2]);

      g_Render.Render(BB_RENDER_MODEL_DATA_PARAMS(g_object));

   g_Render.PopMatrix();

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();
   g_cloth.Shutdown();
}