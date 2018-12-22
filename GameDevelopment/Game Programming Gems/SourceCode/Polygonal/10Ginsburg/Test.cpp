/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
////////////////////////////////////////////////////////////////////////////////
//
//  Test.cpp
//
//  Description:
//
//      An obscenely simple GL App to demonstrate the Octree 
//
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "Octree.h"

///
//  Macros
//
#define WINDOW_WIDTH    512
#define WINDOW_HEIGHT   384
#define NUM_TRIS        16
#define WORLD_SIZE      1.0f

///
//  Globals
//
static OCT_NODE     *OctTable= NULL;
static int           OctCount;
static unsigned int *TriIdxTable= NULL;
static int           TriIdxCount;
static TRI           Tris[NUM_TRIS];
static int           DrawIdx= 0;
static int           ShowTree= TRUE;

////////////////////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//


///
//  AppInit()
//
void AppInit()
{
    int i,
        j,
        k;
    
    Octree Tree;

    ///
    //  Throw some random triangles together
    //
    for(i= 0; i < NUM_TRIS; i++)
        for(j= 0; j < 3; j++)
            for(k= 0; k < 3; k++)        
            {
                int   Rand= (rand() % 32767) - 16384;
                float Val= (float)Rand / 16384.0f;
                
                Tris[i].Pt[j][k]= (Val * WORLD_SIZE);
            }

    printf("Building Octree...\n");
    Tree.Create(&Tris[0], NUM_TRIS, 4);
    printf("Done!\n");
 
    Tree.GetTables(&OctTable, &OctCount, &TriIdxTable, &TriIdxCount);
}


///
//  AppTerm()
//
static
void AppTerm()
{
    if(OctTable)
    {
        free(OctTable);
        OctTable= NULL;
    }

    if(TriIdxTable)
    {
        free(TriIdxTable);
        TriIdxTable= NULL;
    }
}

///
//  DrawTri()
//
static
void DrawTri(TRI Tri)
{
    int i;

    glBegin(GL_LINES);
    for(i= 0; i < 3; i++)
    {
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(Tri.Pt[i].x, Tri.Pt[i].y, Tri.Pt[i].z);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(Tri.Pt[(i+1)%3].x, Tri.Pt[(i+1)%3].y, Tri.Pt[(i+1)%3].z);
    }
    glEnd();

}

///
//  DrawBox()
//
static
void DrawBox(BBOX BBox, float r, float g, float b)
{
    float BoxColor[3];

    BoxColor[0]= r;
    BoxColor[1]= g;
    BoxColor[2]= b;

    glBegin(GL_LINES);
        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Min.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Max.x, BBox.Max.y, BBox.Max.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Min.z);

        glColor3fv(BoxColor);
        glVertex3f(BBox.Min.x, BBox.Max.y, BBox.Max.z);

    glEnd();
}

///
//  Reshape()
//
static
void Reshape(int Width, int Height)
{
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (60.0, (GLfloat)Width/(GLfloat)Height, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef(0.0, 0.0, -3.0);  
}

///
//  Key()
//
void Key(unsigned char ch, int x, int y)
{
         
    switch(ch)
    {
        case 'U':
        case 'u':
            ShowTree= FALSE;
            if(DrawIdx < OctCount-1)
                DrawIdx++;
            break;

        case 'D':
        case 'd':
            ShowTree= FALSE;
            if(DrawIdx > 0)
                DrawIdx--;
            break;

        case 'T':
        case 't':
            ShowTree= TRUE;
            DrawIdx=  0;
            break;
    }

    glutPostRedisplay();
}

///
//  Draw()
//
static
void Draw(void)
{
    int i;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(OctTable)
    {
        int Idx= DrawIdx;
        int Start,
            End;

        ///
        //  Draw the whole tree
        //
        if(ShowTree)
        {
            int i;

            for(i= 0; i < OctCount; i++)
                DrawBox(OctTable[i].BBox, 1, 1, 1);

        }
        ///
        //  Draw one node with its neighbors and tris
        //
        else
        {

            ///
            //  Draw Neighbors
            //
            for(i= 0; i < NUM_NEIGHBORS; i++)
                if(OctTable[Idx].NeighborIdx[i] != NULL_NODE)
                    DrawBox(OctTable[OctTable[Idx].NeighborIdx[i]].BBox, 1, 0, 0);

            ///
            //  Draw Box
            //
            glDisable(GL_DEPTH_TEST);
            DrawBox(OctTable[Idx].BBox, 1, 1, 1);
            glEnable(GL_DEPTH_TEST);

        }

        
        ///
        //  Draw triangles
        //
        Start= OctTable[Idx].TriIdxStart;
        End=   OctTable[Idx].TriIdxStart + OctTable[Idx].TriIdxCount;

        for(i= Start; i < End; i++)
        {
            unsigned int TriIdx= TriIdxTable[i];
    
            DrawTri(Tris[TriIdx]);
        }
    }

    
    glutSwapBuffers();
    glFlush();
}

////////////////////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  main()
//
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(200, 250);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Simple Octree Demo");

    glutDisplayFunc(Draw);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);

    AppInit();

    glutMainLoop();

    AppTerm();

    return(TRUE);

}

