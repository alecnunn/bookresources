/*
	Csg by Bsp.
	Author:			Marius Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __CCOMPILER_H__
#define __CCOMPILER_H__

#include "collections.h"
#include "Brush.h"
#include "BspTree.h"
#include "basethread.h"

//---------------------------------------------------------------------------------------
class Scene;
class Compiler  : public CThread
{
public:
    Compiler();
    virtual ~Compiler(){};
    BOOL     PerformHSR(Scene& scene);
    static   BOOL Union(PBrushes& pPrimitives, Brush& brush, BOOL sgow=0);
public:
    static Compiler* PCompiler;
};

//---------------------------------------------------------------------------------------
// Shortcut
#define COMP()          Compiler::PCompiler

#endif // !__CCOMPILER_H__
