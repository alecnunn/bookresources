/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __BRUSH_H__
#define __BRUSH_H__

#include "poly.h"
#include "minibsp.h"

//--------------------------------------------------------------------------------------
// Texture mapping enumaration
typedef enum _TEX_MAP
{
    TM_LIN,         // Linear textur emapping
    TM_CYL,         // Cylindrical texture mapping
    TM_SPH,         // Spherical texture mapping
    TM_ENV,         // Environmanetal texture mapping
    TM_CUBE,        // Cube texture mapping
    TM_GST,         // Generate S and T texture mapping
}TEX_MAP;

//--------------------------------------------------------------------------------------
// Creation flags of a brush
struct  CreaFlags
{
    REAL    _thick;         // hallow thikness
    DWORD   _flags;         // brush flags
    int     _vstrips;       // vertical strips
    int     _hstrips;       // horizontal strips
    TEX_MAP _tm;            // texture mapping
};

//--------------------------------------------------------------------------------------
class Brush;
class Scene;
typedef vvector<Brush*>            PBrushes;
typedef vvector<Brush*>::iterator  PPBrush;

//--------------------------------------------------------------------------------------
// The Brush class
class Brush
{
public:
    static int GID;
    Brush*    _pResult;
    V3		  _t;             // translation (! used)
    V3		  _r;             // rotation (! used)
    V3		  _s;             // scalation (! used)
    V3		  _f;	          // shift (! used)
    M4        _mt;            // (! used)
    int		  _unicID;
    Polys	  _polys;
    BBox      _box;      
    BOOL      _selected; 
    BOOL	  _copyed;
    DWORD     _flags;    
    PBrushes  _pCutBrshes;    // cut brushes which cuts here  
    PBrushes  _pSldBrushes;   // solid brushes to cut (if this is a cut brush)  
    int		  _polySec;       // second polygon group stating index (for hollows)  
    REAL	  _thikness;      // hollow thikness  
    TEX_MAP   _texmap;        // texture mapping applyed to this brush  
    CreaFlags _cf;            // cached creation flags

public:
    Brush();
    Brush(const Brush& b);
    virtual ~Brush();
    // make them
    void    MakeStairs(V3& dims, int radstrips, int vstrips, REAL thikness, TEX_MAP tm = TM_LIN);
    void    MakeCone(REAL rad, REAL height, int radstrips, int vstrips, TEX_MAP tm = TM_LIN);
    void    MakeCylinder(REAL rad, REAL height, int radstrips, int vstrips, TEX_MAP tm = TM_LIN);
    void    MakeCube(V3& dims, TEX_MAP tm = TM_LIN);
    void    MakeSheet(V3& dims, int vstrips, int hstrips, TEX_MAP tm = TM_LIN);
    void	StartSecGrp(REAL t){_polySec = _polys.size();_thikness=t;}
    Brush&  operator<<(const Brush& b);
    Brush&  operator=(const Brush& b);
    Brush&  operator<<(Poly& p);
    Brush&  operator<<(vvector<Poly>& p);
    void    RotateTCOneStep();
    Brush&  GetWorldPos();
    Brush&	GetResult();
    Brush&	GetPrimitive();
    void    GlTransform();
    void    Snap2grid(REAL gs,BOOL m);
    void	SBoxTr(BBox& newBox);
    void    Reverse();
    M4&     GetTrMatrix();
    void    DeSelect(){_selected=FALSE;}
    void    Select(){_selected=TRUE;}
    BOOL    IsSelected(){return _selected;}
    void	MakeTm();
    void    Recalc();
    void    Replan();
    void	GetTxPolysCopy(Polys& pl);
    void	GetTxReversedPolysCopy(Polys& pl);
    void    Clear();
    BOOL    IsSealed();
    void    Seal();
    void    AddCutter(Brush* pCutter);
    void    RemoveCutter(Brush* pCutter);
    void    AddBrush(Brush* pBrush);
    void    RemoveBrush(Brush* pBrush);
    void    ApplyCuts();
    void    Equal(const Brush& b);
    void	Move(V3& v);
    void	Scale(V3& v, V3& t,REAL zoom);
    void	Raise(V3& v, REAL zoom);
    void    LimitScaleVector(V3& v,REAL);
    void	Rotate(V3& v);
    void    FlagFaces(DWORD f);
    BOOL    IsSolid(){return _flags & BRSH_SOLID;}
    BOOL	IsDirty(){return _flags&BRSH_DIRTY;};
    void	Dirty(BOOL b){
        if(b)
            _flags|=BRSH_DIRTY;
        else
            _flags&=~BRSH_DIRTY;
    };

private:
    void    RescalePolys(int s, int e, V3& sc, V3& tr, BBox& b,BBox& b1);
    void    UpdateLinks();
    void    AlocateResultPtr();
    void	RecalcTM();
    void	Shift(V3& v);
};


#endif //!__BRUSH_H__
