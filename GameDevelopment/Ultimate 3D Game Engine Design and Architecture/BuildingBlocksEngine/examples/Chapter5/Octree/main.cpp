/*
   OctreeNode Demo - Chapter 5
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"main.h"
#include<OpenGL.h>
#include<Input.h>
#include<Octree.h>


// Single vertex.
struct stVertex
{
   float x, y, z;
};


// Vertex descriptor for the models.
bbe::VertexDescriptor g_desc;

// Scene rotations.
int g_xRot = 0, g_oldXRot = 0;
int g_yRot = 0, g_oldYRot = 0;

// Rendering System.
bbe::OpenGLRenderer g_Render;

// OctreeNode to be rendered.
bbe::OctreeNode *g_octree = NULL;

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


   g_desc.AddElement(BB_ELEMENT_TYPE_VERTEX_3F);


   g_octree = new bbe::OctreeNode();

   if(g_octree == NULL)
      return false;

   g_octree->CreateNode(bbe::Vector3D(0, 0, 0), 5, 0, 2);

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


void RenderOctreeNode(bbe::OctreeNode* pNode)
{
   if(pNode == NULL)
      return;

   if(pNode->isLeaf())
      {
         // Get the min and max bounding box for this node.
         float bbSize = pNode->GetSize();
         bbe::Vector3D center = pNode->GetCenter();
         bbe::Vector3D min, max;

         min = center - bbSize / 2.0f;
         max = center + bbSize / 2.0f;

         stVertex verts[32];

         verts[0].x = min.x; verts[0].y = min.y; verts[0].z = min.z;
         verts[1].x = max.x; verts[1].y = min.y; verts[1].z = min.z;
         verts[2].x = max.x; verts[2].y = min.y; verts[2].z = min.z;
         verts[3].x = max.x; verts[3].y = min.y; verts[3].z = max.z;
         verts[4].x = max.x; verts[4].y = min.y; verts[4].z = max.z;
         verts[5].x = min.x; verts[5].y = min.y; verts[5].z = max.z;
         verts[6].x = min.x; verts[6].y = min.y; verts[6].z = max.z;
         verts[7].x = min.x; verts[7].y = min.y; verts[7].z = min.z;

         verts[8].x = min.x; verts[8].y = max.y; verts[8].z = min.z;
         verts[9].x = max.x; verts[9].y = max.y; verts[9].z = min.z;
         verts[10].x = max.x; verts[10].y = max.y; verts[10].z = min.z;
         verts[11].x = max.x; verts[11].y = max.y; verts[11].z = max.z;
         verts[12].x = max.x; verts[12].y = max.y; verts[12].z = max.z;
         verts[13].x = min.x; verts[13].y = max.y; verts[13].z = max.z;
         verts[14].x = min.x; verts[14].y = max.y; verts[14].z = max.z;
         verts[15].x = min.x; verts[15].y = max.y; verts[15].z = min.z;

	      verts[16].x = min.x; verts[16].y = min.y; verts[16].z = min.z;
         verts[17].x = min.x; verts[17].y = max.y; verts[17].z = min.z;
         verts[18].x = min.x; verts[18].y = max.y; verts[18].z = min.z;
         verts[19].x = max.x; verts[19].y = max.y; verts[19].z = min.z;
         verts[20].x = max.x; verts[20].y = max.y; verts[20].z = min.z;
         verts[21].x = max.x; verts[21].y = min.y; verts[21].z = min.z;
         verts[22].x = max.x; verts[22].y = min.y; verts[22].z = min.z;
         verts[23].x = min.x; verts[23].y = min.y; verts[23].z = min.z;

         verts[24].x = min.x; verts[24].y = min.y; verts[24].z = max.z;
         verts[25].x = min.x; verts[25].y = max.y; verts[25].z = max.z;
         verts[26].x = min.x; verts[26].y = max.y; verts[26].z = max.z;
         verts[27].x = max.x; verts[27].y = max.y; verts[27].z = max.z;
         verts[28].x = max.x; verts[28].y = max.y; verts[28].z = max.z;
         verts[29].x = max.x; verts[29].y = min.y; verts[29].z = max.z;
         verts[30].x = max.x; verts[30].y = min.y; verts[30].z = max.z;
         verts[31].x = min.x; verts[31].y = min.y; verts[31].z = max.z;

         g_Render.Render(BB_PRIMITIVE_LINE_LIST, &g_desc,
                         (char*)verts, NULL,
                         sizeof(verts), 0,
                         sizeof(stVertex));
      }
   else
      {
         // Else continue down the tree.
         bbe::OctreeNode **pSubNodes = pNode->GetSubNodes();
         RenderOctreeNode(pSubNodes[TOP_FRONT_LEFT]);
         RenderOctreeNode(pSubNodes[TOP_FRONT_RIGHT]);
         RenderOctreeNode(pSubNodes[TOP_BACK_LEFT]);
         RenderOctreeNode(pSubNodes[TOP_BACK_RIGHT]);

         RenderOctreeNode(pSubNodes[BOTTOM_FRONT_LEFT]);
         RenderOctreeNode(pSubNodes[BOTTOM_FRONT_RIGHT]);
         RenderOctreeNode(pSubNodes[BOTTOM_BACK_LEFT]);
         RenderOctreeNode(pSubNodes[BOTTOM_BACK_RIGHT]);
      }
}


void DemoRender()
{
	g_Render.StartRendering(1, 1, 0);
   g_Render.LoadIdentityMatrix();

   g_Render.SetView(0, 0, 10, 0, 0, 0, 0, 1, 0);
   g_Render.RotateMatrix((float)g_xRot, 0, 1, 0);
   g_Render.RotateMatrix((float)g_yRot, 1, 0, 0);

   RenderOctreeNode(g_octree);

	g_Render.EndRendering();
}


void DemoShutdown()
{
   g_Render.Shutdown();

   if(g_octree != NULL)
      delete g_octree;

   g_octree = NULL;
}