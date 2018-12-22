/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include "baseos.h"
#include "basemath.h"
#include "basegeom.h"
#include <gl/gl.h>			
#include <gl/glu.h>			
#include <gl/glaux.h>	
#include "poly.h"
#include "brush.h"

//---------------------------------------------------------------------------------------
// Scene class. Holds all brushes.
class Scene
{
public:
    Scene(){};
    virtual ~Scene(){
        ClearPrimitives();
        ClearBrushes();
    }
    void Clear(){
        ClearPrimitives();
        ClearBrushes();
    }
    void ClearPrimitives()
    {
        FOREACH(PBrushes,_primitives, ppb)
        delete (*ppb);
        _primitives.clear();
    }
    void ClearBrushes()
    {
        FOREACH(PBrushes,_brushes, ppb)
        {
            if(!((*ppb)->_flags & BRSH_REF))
                delete (*ppb);
        }
        _brushes.clear();
    }
    int	GetPolyCnt();
    PBrushes* GetPrimitives(){
        return &_primitives;
    };
    void AddBrush(Brush* pB){
        _brushes.push_back(pB);
    }
    PBrushes* GetBrushes(){
        return &_brushes;
    };
    void      AddPrimitive(Brush* pB)
    {
        _primitives.push_back(pB);
    }
    Brush*  GetPrimitive(int brSel){
        if(brSel < _primitives.size())
            return _primitives[brSel];
        return 0;
    }
    Brush*  GetBrush(int brSel){
        if(brSel < _brushes.size())
            return _brushes[brSel];
        return 0;
    }

private:
    PBrushes _primitives;
    PBrushes _brushes;
};

#endif // !__SCENE_H__

