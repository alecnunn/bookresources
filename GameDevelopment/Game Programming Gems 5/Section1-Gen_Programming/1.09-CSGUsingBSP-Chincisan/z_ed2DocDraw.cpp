/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "mainfrm.h"
#include "Z_ed2View.h"
#include "Z_ed3View.h"
#include "baseutils.h"
#include <algorithm>

//---------------------------------------------------------------------------------------
// Sets up the GL on the passwd in hDC 
void CZ_ed2Doc::InitGL(HDC hDC, HGLRC& hrc)
{
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize       = sizeof(pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 24;
    pfd.cDepthBits  = 16;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    int iFormat     = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, iFormat, &pfd);
    hrc = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hrc);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearDepth( 1 );
    glDisable(GL_FOG);
    glDisable(GL_DITHER);
    glEnable(GL_ALPHA_TEST);
    glClearColor(0.0f,0.0f,0.0f,0.f);
    glDisable(GL_LIGHTING);
}

//---------------------------------------------------------------------------------------
// Sets different colors on brushes based on theirs properties
static void S_SelectColor(Brush& rb,BOOL sel=FALSE)
{
    if(rb._flags & BRSH_SOLID){
        if(sel)
            glColor4ubv(CLR_SELECT);
        else{
            if(rb._flags & BRSH_DETAIL)
                glColor4ubv(CLR_DETAIL);
            else
                glColor4ubv(CLR_NORMAL);

        }
    }
    else{
        RGBA	rgba = {180,0,0,255};//		CLR_CUT
        if(rb._flags & BRSH_REV){
            rgba[2] = 100;
        }
        if(rb._flags & BRSH_CUTALL){
            rgba[2] = 100;
        }
        if(rb._flags & BRSH_NEW){
            rgba[2] = 255;
        }
        if(sel){
            rgba[1] = 240;
            rgba[2] = 255;
        }
        glColor4ubv(rgba);
    }
}

//---------------------------------------------------------------------------------------
// Draws in 2D view
void CZ_ed2Doc::Draw2DBrushes(CZ_ed2View *pV)
{
    BBox box;
    glColor4ubv(CLR_NORMAL);
    if(HasForms()){
        FOREACH(PBrushes, (*GetForms()), b){
            Brush& rb = (*b)->GetResult();
            if(!pV->IsBoxInViewPort(rb._box))
                continue;
            S_SelectColor(rb);
            PPoly pb = rb._polys.begin();
            PPoly pe = rb._polys.end();
            for(;pb!=pe;pb++){
                Poly& pl = *pb;
                glBegin(GL_LINE_LOOP);
                PVertex vb = pl._vtci.begin();
                PVertex ve = pl._vtci.end();
                for(;vb!=ve;vb++){
                    VTCIS& vtci = *vb;
                    glVertex3f(vtci._xyz.x,vtci._xyz.y,vtci._xyz.z);
                }
                glEnd();
            }
        }
    }

    // Draws selected brushes
    glColor4ubv(CLR_WHITE);
    int           brushes;
    Brush** pBS = GetSelBrushes(brushes);
    for(int i=0; i < brushes; i++, pBS++){
        Brush& rb = (**pBS);
        if(_alignment){
            if (i==brushes-1)
                glColor4ubv(CLR_SELECT); //last sel colir it different. (align reference)
            else
                glColor4ubv(CLR_WHITE);
        }
        else{
            S_SelectColor(rb,TRUE);
        }

        FOREACH(Polys, rb._polys, pp){
            glBegin(GL_LINE_LOOP);
            {
                FOREACH(vvector<VTCIS>, (pp)->_vtci, v)
                {
                    glVertex3f(v->_xyz.x,v->_xyz.y,v->_xyz.z);
                }
            }
            glEnd();
            glPointSize(5.f);
            glBegin(GL_POINTS);
            {
                FOREACH(vvector<VTCIS>, (pp)->_vtci, v){
                    glVertex3f(v->_xyz.x,v->_xyz.y,v->_xyz.z);
                }
            }
            glEnd();
            glPointSize(1.f);
        }
    }

    glColor4ubv(CLR_WHITE);
    int			 polys;
    Poly** pPS = GetSelPolys(polys);
    if(polys)
    {
        glPointSize(5.f);
        for(int i=0; i < polys; i++, pPS++){
            Brush* pb = (*pPS)->_pBrush;
            {
                glBegin(GL_POINTS);
                FOREACH(vvector<VTCIS>, (*pPS)->_vtci, pv){
                    VTCIS& v = *pv;
                    glVertex3f(v._xyz.x,v._xyz.y,v._xyz.z);
                }
                glEnd();
            }
        }
        glPointSize(1.f);
    }
}

//---------------------------------------------------------------------------------------

void CZ_ed2Doc::DrawCameraRay(CZ_ed2View *pV)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBegin(GL_LINES);
    glColor4ubv(CLR_CAMSTART);
    glVertex3f(_camray[0].x,_camray[0].y,_camray[0].z);
    glColor4ubv(CLR_CAMEND);
    glVertex3f(_camray[1].x,_camray[1].y,_camray[1].z);
    glEnd();
    glDisable(GL_BLEND);
}

//---------------------------------------------------------------------------------------
// called when the zooming factor is big enough. Draws a cull camera frusrum in the 2D 
// views
void CZ_ed2Doc::DrawCamFrustrum(CZ_ed2View *pV)
{
    if(pV->_zoom > 1.f){
        glColor4ubv(CLR_WHITE);
        glBegin(GL_LINES);

        for(int i=0;i<5;i++){
            glVertex3f(_frustDots[0].x,_frustDots[0].y,_frustDots[0].z);
            glVertex3f(_frustDots[i].x,_frustDots[i].y,_frustDots[i].z);
        }
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4ubv(CLR_SCRSURFACE);
        glBegin(GL_QUADS);
        for( i=1;i<5;i++){
            glVertex3f(_frustDots[i].x,_frustDots[i].y,_frustDots[i].z);
        }
        glEnd();
        glDisable(GL_BLEND);
        glPointSize(5.f);
        glColor4ubv(CLR_WHITE);
        glBegin(GL_POINTS);
        glVertex3f(_camray[2].x,_camray[2].y,_camray[2].z);
        glEnd();
        glPointSize(1.f);
    }
    else
    {
        glPushMatrix();
        {
            glColor4ubv(CLR_CAMFRUST);
            REAL sc = 16.f/pV->_zoom;
            V3  vdir;
            V3  nfd[5];
            ::memcpy(nfd,_frustDots,sizeof(_frustDots));
            for(int i=1;i<5;i++){
                vdir = nfd[i] - nfd[0];
                vdir.norm();
                nfd[i] = nfd[0] + vdir *  sc;
            }
            glBegin(GL_LINES);
            for(i=0;i<5;i++)
            {
                glVertex3f(nfd[0].x,nfd[0].y,nfd[0].z);
                glVertex3f(nfd[i].x,nfd[i].y,nfd[i].z);
            }
            glEnd();
        }
        glPopMatrix();
    }
}

//---------------------------------------------------------------------------------------
// Draws a bounding box
void    CZ_ed2Doc::GLDrawBBox(BBox& box, BOOL asWire)
{
    glBegin(GL_LINE_LOOP);
        glVertex3f(box._min.x, box._min.y, box._min.z);
        glVertex3f(box._max.x, box._min.y, box._min.z);
        glVertex3f(box._max.x, box._min.y, box._max.z);
        glVertex3f(box._min.x, box._min.y, box._max.z);
    glEnd();

    glBegin(GL_LINE_LOOP);
        glVertex3f(box._min.x, box._max.y, box._min.z);
        glVertex3f(box._max.x, box._max.y, box._min.z);
        glVertex3f(box._max.x, box._max.y, box._max.z);
        glVertex3f(box._min.x, box._max.y, box._max.z);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(box._min.x, box._max.y, box._min.z);
        glVertex3f(box._min.x, box._min.y, box._min.z);
        glVertex3f(box._max.x, box._max.y, box._min.z);
        glVertex3f(box._max.x, box._min.y, box._min.z);
        glVertex3f(box._max.x, box._max.y, box._max.z);
        glVertex3f(box._max.x, box._min.y, box._max.z);
        glVertex3f(box._min.x, box._max.y, box._max.z);
        glVertex3f(box._min.x, box._min.y, box._max.z);
    glEnd();
}

//---------------------------------------------------------------------------------------
//  Draws the 3D scene
void    CZ_ed2Doc::Draw3DScene(z_ed3View* pV)
{
    DWORD dwMode ;
    if(_vmodeback){
        // show back faces
        dwMode = GL_FRONT_AND_BACK;
        glDisable(GL_CULL_FACE);
    }
    else{
        dwMode = GL_BACK;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);
    }

    if(_vmodefire){
        // wireframe mode
        glPolygonMode(dwMode, GL_LINE);
    }
    else{
        // fill mode
        glPolygonMode(dwMode, GL_FILL);
    }
    glDisable(GL_BLEND);
    glPushMatrix();
    {
        DrawTextured(pV);
        if(_selmode  == SEL_BRUSH){
            DrawSelBrushes(pV);
        }
        DrawAxis();
    }
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DrawTextured(z_ed3View *pV)
{
    // cuts brushes are blended so draw them at the end
    if(HasForms()){
        FOREACH(PBrushes, (*GetForms()), b){
            Brush& rb = (*b)->GetResult();
            if(_cam.IsCulled(&rb._box._min,&rb._box._max))
                continue;
            if(rb._flags & BRSH_SOLID){
                DrawSolidBrush(rb);
            }
        }
    }
    if(SEL_NONE != _selmode){
        FOREACH(PBrushes, (*GetForms()), b)
        {
            Brush& rb = (*b)->GetResult();
            if(_cam.IsCulled(&rb._box._min,&rb._box._max))
                continue;
            if(!(rb._flags & BRSH_SOLID)){
                DrawCutBrush(rb);
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DrawCutBrush(Brush& rb)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_TEXTURE_2D);
    if(rb._flags & BRSH_REV)
        glColor4ubv(CLR_CUT3DBRUSH);
    else
        glColor4ubv(CLR_CUT3DBRUSH);
    PPoly pb = rb._polys.begin();
    PPoly pe = rb._polys.end();

    for(;pb!=pe;pb++){
        Poly& pl = *pb;
        glBegin(GL_POLYGON);
        PVertex vb = pl._vtci.begin();
        PVertex ve = pl._vtci.end();
        for(;vb!=ve;vb++){
            VTCIS& vtci = *vb;
            glTexCoord2f(vtci._uv[0].u,vtci._uv[0].v);
            glVertex3f(vtci._xyz.x,vtci._xyz.y,vtci._xyz.z);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DrawSolidBrush(Brush& rb)
{
    vvector<Poly*> transPolys(64);
    vvector<Poly*> mirrors(64);

    if(!_vmodefire){
        glEnable(GL_TEXTURE_2D);
    }
    else{
        glDisable(GL_TEXTURE_2D);
    }
    glColor4ubv(CLR_WHITE);
    PPoly pb = rb._polys.begin();
    PPoly pe = rb._polys.end();
    for(;pb!=pe;pb++){
        Poly& pl = *pb;
        if(!_vmodefire){
            glBindTexture(GL_TEXTURE_2D, DOC()->_dumptex);
        }
        glColor4ubv((BYTE*)pl._color);
        glBegin(GL_POLYGON);
        PVertex vb = pl._vtci.begin();
        PVertex ve = pl._vtci.end();
        for(;vb!=ve;vb++){
            VTCIS& vtci = *vb;
            glTexCoord2f(vtci._uv[0].u,vtci._uv[0].v);
            glVertex3f(vtci._xyz.x,vtci._xyz.y,vtci._xyz.z);
            if(_glLight)
                glNormal3f(pl._normal.x,pl._normal.y,pl._normal.z);
        }
        glEnd();
    }

    // draw transparent polygons
    if(transPolys.size()){
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(0);
        FOREACH(vvector<Poly*>, transPolys, ppPoly){
            Poly* pPoly = *ppPoly;
            glColor4ubv((BYTE*)pPoly->_color);
            glBindTexture(GL_TEXTURE_2D, DOC()->_dumptex);
            glBegin(GL_POLYGON);
            PVertex vb = pPoly->_vtci.begin();
            PVertex ve = pPoly->_vtci.end();
            for(;vb!=ve;vb++){
                VTCIS& vtci = *vb;
                glTexCoord2f(vtci._uv[0].u,vtci._uv[0].v);
                glVertex3f(vtci._xyz.x,vtci._xyz.y,vtci._xyz.z);
            }
            glEnd();
        }
        glDepthMask(1);
        glDisable(GL_BLEND);
    }
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DrawSelBrushes(z_ed3View *pV)
{
    int     brushes;
    Brush** pBS         = DOC()->GetSelBrushes(brushes);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    for(int i=0; i < brushes; i++, pBS++){
        Brush& rb = (**pBS);    //->GetResult();
        if(_cam.IsCulled(&rb._box._min,&rb._box._max))
            continue;
        if(_alignment){
            if(i==brushes-1){
                if (i==brushes-1){
                    glColor4ubv(CLR_SELECT);
                }
                else{
                    glColor4ubv(CLR_WHITE);

                }
            }
        }
        else{
            glColor4ubv(CLR_SELECT);
        }

        PPoly pb = rb._polys.begin();
        PPoly pe = rb._polys.end();
        for(;pb!=pe;pb++){
            Poly& pl = *pb;
            glBegin(GL_LINE_LOOP);
            PVertex vb = pl._vtci.begin();
            PVertex ve = pl._vtci.end();
            for(;vb!=ve;vb++){
                VTCIS& vtci = *vb;
                glVertex3f(vtci._xyz.x,vtci._xyz.y,vtci._xyz.z);
            }
            glEnd();
        }
    }
    glEnable(GL_DEPTH_TEST);
}

//---------------------------------------------------------------------------------------
void CZ_ed2Doc::DrawAxis()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_LINES);
    glColor4ub(255,96,96,128);
    glVertex3f(0,0,0);
    glVertex3f(32768.f,.6f,0.f);

    glColor4ub(96,255,96,128);
    glVertex3f(0,0,0);
    glVertex3f(0.f,32768.f,0.f);

    glColor4ub(96,96,255,128);
    glVertex3f(0,0,0);
    glVertex3f(0.f,.1f,32768.f);

    glColor4ub(128,0,0,128);
    glVertex3f(0,0,0);
    glVertex3f(-32768.f,.6f,0.f);

    glColor4ub(0,128,0,128);
    glVertex3f(0,0,0);
    glVertex3f(0.f,-32768.f,0.f);

    glColor4ub(0,0,128,128);
    glVertex3f(0,0,0);
    glVertex3f(0.f,0.f,-32768.f);
    glEnd();

    glColor4ub(43,43,43,64);

    REAL gs = _gridStep*10;
    glBegin(GL_LINES);
    for(REAL r = -_gridSize/2; r< _gridSize/2; r+=gs){
        glVertex3f(r,0,-_gridSize/2);
        glVertex3f(r,0,_gridSize/2);
    }

    for(r = -_gridSize/2; r< _gridSize/2; r+=gs){
        glVertex3f(-_gridSize/2,0,r);
        glVertex3f(_gridSize/2,0,r);
    }

    glEnd();

    glDepthMask(0);
    glDisable(GL_DEPTH_TEST);
    glColor4ub(255,255,0,255);

    glBegin(GL_LINES);
    glVertex3f(_ip[0].x,_ip[0].y,_ip[0].z);
    glVertex3f(_ip[1].x,_ip[1].y,_ip[1].z);

    glEnd();
    glPointSize(1.f);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
    glDisable(GL_BLEND);

}

