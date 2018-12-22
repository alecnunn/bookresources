/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Include
File:  UL.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
   Utility Library Header
    // forward references
    
      class Mat3; 3x3 matrix
      class Mat4; 4x4 matrix
      class MatR; matrix implemented as 3x3 rotation matrix and a position vector
      class MatQ; matrix implemented as quaternion
      
        class Vec2; // 2 element vector
        class Vec3; // 3
        class Vec4; // 4
        
          class Quat; // quaternion class
          
            class ulOrientation : orientation


******************************************************************************/


#ifndef UL_H
#define UL_H

#ifdef MULTI_THREAD
  #if _MSC_VER > 1000
    #pragma once
  #endif // _MSC_VER > 1000

  #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

  // =================================================================//
  //  Include MFC components for multithreaded (MT) builds.  Behavior
  //    of new and delete operators is different for MFC, so to use
  //    the utility library with an MFC app, we have to include these
  //    MFC headers.

  #include <afxwin.h>         // MFC core and standard components
  #include <afxext.h>         // MFC extensions
  #include <afxdisp.h>        // MFC Automation classes
  #include <afxcview.h>        // CListView, CTreeView
  #include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
  #ifndef _AFX_NO_AFXCMN_SUPPORT
     #include <afxcmn.h>			// MFC support for Windows Common Controls
  #endif // _AFX_NO_AFXCMN_SUPPORT

#endif			// ifdef MULTI_THREAD


//////////////////////////////////////////////////////////////
// Link the library with anything that includes this header

#ifdef MULTI_THREAD
	#if (FLOAT_SIZE==64)
		#pragma message( "Utility Library - utility64MT.lib" )
		#pragma comment(lib, "utility64MT.lib")
	#else
		#pragma message( "Utility Library - utilityMT.lib" )
		#pragma comment(lib, "utilityMT.lib")
	#endif
#else
	#if (FLOAT_SIZE==64)
		#pragma message( "Utility Library - utility64.lib" )
		#pragma comment(lib, "utility64.lib")
	#else
		#pragma message( "Utility Library - utility.lib" )
		#pragma comment(lib, "utility.lib")
	#endif
#endif



/////////////////////////////////////////////////////////////////
//  Common includes for all modules:

#include "..\CommonSrc\Utility\ulCommon.h"


/////////////////////////////////////////////////////////////////
//	2D Vector
//  3D Vector
//  4D Vector

#include "..\CommonSrc\Utility\ulVec2.h"
#include "..\CommonSrc\Utility\ulVec3.h"
#include "..\CommonSrc\Utility\ulVec4.h"


/////////////////////////////////////////////////////////////////
//	3 x 3 Matrix
//  4 x 4 Matrix
//	Quaternion

#include "..\CommonSrc\Utility\ulMat3.h"
#include "..\CommonSrc\Utility\ulMat4.h"
#include "..\CommonSrc\Utility\ulQuat.h"

/////////////////////////////////////////////////////////////////
//	3 x 3 Rotation + Translation vector

#include "..\CommonSrc\Utility\ulMatR.h"


/////////////////////////////////////////////////////////////////
//	Quaternion + Translation Vector

#include "..\CommonSrc\Utility\ulMatQ.h"


/****************************************************************
*                                                               *
*                        Plane                                  *
*                                                               *
****************************************************************/



class Plane
{
    
public:
    
    
    union
    {
        struct 
        {
            Vec3 vec3;
            Float D;
        };
        struct 
        {
            Float a,b,c,d;
        };
        struct 
        {
            Vec4 vec4;
        };
    };
    
    Plane(const Vec3 & _vec3, Float _d);
    Plane(const Vec4 & _vec4);
    Plane();
    
    const Vec3 &GetNormal() const {return vec3;}
    const Vec3 &normal() const {return vec3;}
    
    const Float &GetOffset() const {return D;}
    const Float &offset() const {return D;}
    
    Int FindPlanePos ( Float x0 , Float y0 , Float z0 , 
        Float x1 , Float y1 , Float z1 , 
        Float x2 , Float y2 , Float z2 ,
        Int unitize = 0);
    
    Int FindPlane ( Float x0 , Float y0 , Float z0 , 
        Float x1 , Float y1 , Float z1 , 
        Float x2 , Float y2 , Float z2, int unitize = 0 );
    
    Int FindPlane ( Vec3 &vp0 , Vec3 &vp1 , Vec3 &vp2 , int unitize = 0);
    
    Float DistanceToPoint(const Vec3 &point) const;
    
    
    void set(const Vec3 & _vec3, Float _d);
    void set(const Vec3 & v1, const Vec3 & v2);
    
    ostream& print(ostream &os) const;
    
};



class MatrixStack
{
public:
    MatrixStack();
    void PushMatrix();
    void PopMatrix();
    void InitMatrixStack();
    Mat4 & GetMatrix ();
    void LoadMatrix(Mat4 &mat);
    
    
private:
    
    Mat4 matrix_stack[256];
    int matrix_top;
    Mat4 *matrix; // top of stack
};


template <class _Type> 
class Rect2D
{
public:
    _Type x1, y1, x2, y2;
};
template <class _Type> 
class Rect3D
{
public:
    _Type x1, y1, x2, y2, z1, z2;
};





typedef union UPointer
{
    char * c;		// type 1
    byte * b;		// type 2
    
    short * s;		// type 3
    ushort * w;		// type 4
    
    Int * i;		// type 5
    DWORD * l;		// type 6
    
    Float32 * f;	// type 7
    void ** p;		// type 8
    Float64 * d;	// type 9
    void * v;		// type 10
} UPointer;





class ulColor
{
public:
    union
    {
        DWORD color;  
        struct
        {
            unsigned char b, g, r, a;
        };
    };
    ulColor() {color = 0;}
    ulColor(DWORD c) {color = c;}
    ulColor(unsigned char alpha, unsigned char red, unsigned char green, unsigned char blue) 
    {a = alpha; r = red; g = green; b = blue;}
};


/////////////////////////////////////////////////////////////////
// Vector - Matrix operator declarations
// Only partial for now.

Vec3 operator * (const Vec3& v3, const Mat4& m4);

Vec4 operator * (const Vec4& v4, const Mat4& m4);
Vec4 operator * (const Mat4& v4, const Vec4& m4);

/////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


template <class _Type> 
class ulLeaf
{
public:
    ulLeaf() { m_parent = 0; m_left = 0; m_right = 0;}
    
    int m_id;
    _Type * m_parent;
    _Type * m_left;
    _Type * m_right;
};



template <class _Type> 
class ulTree
{
public:
    ulTree() { m_top = 0;  }
    // 0 == top
    _Type * Top() { return m_top; }
    
    _Type * GetUpperRight(_Type * c)
    {
        _Type * parent;
        if (c == 0)
            return 0;
        for(;;)
        {
            parent = c->m_parent;
            if (parent == 0)
                return c; // at the top
            
            if (parent->m_left == c)
                c = parent;
            else
                return c;
        }
        
    }
    
    void InsertRightEnd(_Type * parent, _Type * child)
    {
        if (m_top == 0)
        {
            m_top = child;
            return;
        }
        if (parent == 0)
            parent = m_top;
        
        while(parent->m_right != 0)
            parent = parent->m_right;
        // find right leaf
        
        parent->m_right = child;
        
        child->m_right = 0;
        child->m_left = 0;
        child->m_parent = parent;
    }
    
    void InsertLeftEnd(_Type * parent, _Type * child)
    {
        if (m_top == 0)
        {
            m_top = child;
            return;
        }
        if (parent == 0)
            parent = m_top;
        
        while(parent->m_left != 0)
            parent = parent->m_left;
        // find right leaf
        
        parent->m_left = child;
        child->m_right = 0;
        child->m_left = 0;
        child->m_parent = parent;
    }
    
    void InsertLeft(_Type * parent, _Type * child)
    {
        if (m_top == 0)
        {
            m_top = child;
            return;
        }
        if (parent == 0)
            parent = m_top;
        
        _Type * left = parent->m_left;
        
        parent->m_left = child;
        if (left)
            left->m_parent = child;
        child->m_left = left;
        child->m_right = 0;
        child->m_parent = parent;
    }
    
    
    void InsertRight(_Type * parent, _Type * child)
    {
        if (m_top == 0)
        {
            m_top = child;
            return;
        }
        if (parent == 0)
            parent = m_top;
        
        _Type * right = parent->m_right;
        
        parent->m_right = child;
        if (right)
            right->m_parent = child;
        child->m_right = right;
        child->m_left = 0;
        child->m_parent = parent;
    }
    
    void EraseTree();
    
    void EraseKeepLeft(_Type * n)
    {
        if (n == 0)
            return;
        _Type * parent = n->m_parent;
        
        if (parent == 0)
        {
            // we are top
            m_top = n->m_left;
        }
        else
        {
            if (parent->m_left == n)
                parent->m_left = n->m_left;
            else
                parent->m_right = n->m_left;
        }
        
        delete n;
        
    }
    void EraseKeepRight(_Type * node);
    
    // deletes this and all childen,
    void EraseWithChildren(_Type * n)
    {
        if (n == 0)
            return;
        if (n->m_parent == 0)
        {
            m_top = 0;
        }
        
        // make parent a leaf
        if (n->m_parent->m_left == n)
            n->m_parent->m_left = 0;
        else if (n->m_parent->m_right == n)
            n->m_parent->m_right = 0;
        
        DeleteTree(n);
        
    }
    
    typedef void  ( *ulTreeFunc)(_Type *);
    
    void TraversePost(_Type * n, ulTreeFunc func)
    {
        if (n == 0)
            return;
        
        TraversePost(n->m_left, func);
        TraversePost(n->m_right, func);
        
        func(n);
        
    }
    
    
    void TraversePost(ulTreeFunc func)
    {
        TraversePost(m_top, func);
    }
    
    void TraversePre(_Type * n, ulTreeFunc func)
    {
        if (n == 0)
            return;
        
        func(n);
        
        TraversePre(n->m_left, func);
        TraversePre(n->m_right, func);
        
    }
    
    void TraversePre(ulTreeFunc func)
    {
        TraversePre(m_top, func);
    }
    
    
    void DeleteTree(_Type * n)
    {
        if (n == 0)
            return;
        
        DeleteTree(n->m_left);
        DeleteTree(n->m_right);
        
        delete n;
        
    }
    
    void DeleteTree()
    {
        DeleteTree(m_top);  
    }
    
    
    
    void FindById(_Type * n, int id)
    {
        if (n == 0)
            return;
        
        if (m_found)
            return;
        
        if (n->m_id == id)
        {
            m_found = 1;
            m_foundnode = n;
            return;
        }
        FindById(n->m_left, id);
        FindById(n->m_right, id);
        
    }
    
    _Type * FindById(int id)
    {
        m_found = 0;
        m_foundnode = 0;
        FindById(m_top, id);
        return m_foundnode;
    }
    
    _Type * m_top;
    bool m_found;
    _Type * m_foundnode;
    int level;
    
    
};




#define NextItem(nptr) (nptr) = (nptr)->GetNext()
#define PrevItem(nptr) (nptr) = (nptr)->GetPrev()
//#define NextItem(nptr) ++nptr


/****************************************************************
*                                                               *
*                             ComRef                            *
*                                                               *
****************************************************************/


class ulName
{
public:
    ulName() {m_name = 0;}
    
    void ReleaseName() 
    {
        SAFE_ARRAY_DELETE(m_name);
    }
    
    virtual ~ulName()
    { 
        SAFE_ARRAY_DELETE(m_name);
        
        
    }
    const char * GetName() const { return m_name;}
    char * GetName() { return m_name;}
    void ClearName() { m_name = 0;} 
    void SetName(const char * name) 
    { 
        SAFE_ARRAY_DELETE(m_name);
        
        if (name == 0)
            return;
        
        m_name = new char [strlen(name)+1];
        strcpy(m_name, name);
    }
    
    bool IsName(const char * name)
    {
        if (name == 0)
            return false;
        if (strcmpi(m_name, name) == 0)
            return true;
        
        return false;
        
    }
    
private:
    char * m_name;
};








class Dirt
{
public:
    Dirt() { m_dirty = true; }
    
    virtual bool RefreshRenderer() = 0; // flush properties to renderer
    
    virtual HRESULT AddToRenderer() = 0;   // build an object for the renderer
    virtual HRESULT RemoveFromRenderer() = 0; // kill renderer specific
    
    virtual HRESULT Apply() = 0; // Make this the current
    virtual HRESULT DeleteLists() = 0; // remove all lists and pointers
    
    void SetChanged(const bool & b) { m_dirty = b;}
    bool IsChanged() { return m_dirty;}
    
private:
    bool m_dirty;
    
};

class ComRef: public IUnknown
{
public:
    ComRef() 
    { 
        m_dwRef = 0;
    }
    
    virtual ~ComRef()
    {
        m_dwRef = 0;
    }
    
    HRESULT __stdcall QueryInterface(const IID & riid, LPVOID * ppvObj)
    {
        if (riid == IID_IUnknown)
        {
            *ppvObj = this;
            AddRef();
            return 0;
        }
        return 1;
    }  
    
    DWORD __stdcall AddRef() 
    { 
        return ++m_dwRef; 
    }
    DWORD __stdcall GetRef() 
    { 
        return m_dwRef; 
    }
    DWORD __stdcall SetRef(ULONG v) 
    { 
        m_dwRef = v; 
    }
    
    
    DWORD __stdcall Release() 
    { 
        if (--m_dwRef == 0) 
        {
            delete this; 
            return 0;
        }
        return m_dwRef;
    }
    
private:
    DWORD m_dwRef;
    
};


template <class _BaseClass, class _TypePtr> 
class InstanceList
{
    friend class Instance;
    
public:
    
    ~InstanceList() 
    {
        // in a destructor, leave the data hanging around
        EraseList(false); 
    }
    
    class Instance
    {
        friend class InstanceList<_BaseClass, _TypePtr>;
    public:
        Instance * GetNext() {return m_next;}
        Instance * GetPrev() {return m_prev;}
        void GetNext(Instance * n) {m_next = n;}
        void GetPrev(Instance * n) {m_prev = n;}
        
        Instance(Instance *p, Instance *n, _TypePtr v)
        {
            set(p,n,v);
        }
        
        void set(Instance *p, Instance *n, _TypePtr v)
        {
            m_prev = p;
            m_next = n;
            data = v;
        }    
        
        operator _TypePtr() const
        {
            return data;
        }
        
        _TypePtr operator->() const
        {
            ATLASSERT(data!=NULL);
            return data;
        }
        
        bool operator==(_TypePtr pT) const
        {
            return data == pT;
        }
        
    protected:
        
        Instance * m_next;   
        Instance * m_prev;   
        
    private:
        _TypePtr data;
    };
    
    
    typedef Instance * iterator;
    
    
    
    Instance * m_head;
    Instance * m_tail;
    _TypePtr m_current;
    int m_currentnum;
    int m_NumNodes;   
    Instance * iptr; // traversal
    
    
public:
    InstanceList() 
    {
        InitList();
    }
    
    void InitList()
    {
        m_head = 0; 
        m_tail = 0; 
        m_NumNodes  = 0; 
        m_current = 0; 
        m_currentnum = -1;
    }
    
    Instance * Head() { return m_head;}
    Instance * Tail() { return m_tail;}
    bool IsEmpty() { return (Size() == 0);}
    bool empty() { return (Size() == 0);}
    int Size() {return m_NumNodes;}
    int size() {return m_NumNodes;}
    _TypePtr SetCurrentByNumber(const int & n) 
    {
        m_current = GetEntryByNumber(n);
        m_currentnum = n;
        return m_current;
    }
    _TypePtr GetFirstEntry()
    {
        iptr = Head();
        if (iptr)
            return iptr->data;
        return 0;
    }
    _TypePtr GetNextEntry()
    {
        if (iptr)
        {
            iptr = iptr->GetNext();
            if (iptr)
                return iptr->data;
        }
        
        return 0;
    }
    _TypePtr GetLastEntry()
    {
        iptr = Tail();
        if (iptr)
            return iptr->data;
        return 0;
    }
    _TypePtr GetPrevEntry()
    {
        if (iptr)
        {
            iptr = iptr->GetPrev();
            if (iptr)
                return iptr->data;
        }
        return 0;
    }
    
    
    void SetCurrent(_TypePtr node) {m_current = node;}
    void SetCurrentAndNumber(_TypePtr node) 
    {
        FindNode(node, m_currentnum);
        m_current = node;
    }

    _TypePtr GetCurrent()
	 {
		 return m_current;
	 }
    int GetCurrentNumber() {return m_currentnum;}
    
    
    _TypePtr GetEntryByNumber(int n)
    {
        iterator p;
        int i = 0;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */ )
        {
            if (i == n)
            {
                if (p)
					 {
						 iptr = Head();
                    return p->data;
					 }
                else
					 {
						 iptr = Head();
                    return 0;
					 }
            }
            i++;
        }
		  iptr = Head();
        return 0;
    }
    
    iterator FindNode(_TypePtr f, int & n)
    {
        iterator p;
        n = 0;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */ )
        {
            if (f == p->data)
                return p;
            n++;
        }
        return 0; // not found
    }
    
    
    
    
    iterator Erase(iterator p, bool del_obj = true) 
    {
        iterator nextnode = 0;  // return next in list
        if (p)
        {
            nextnode = p->GetNext();  // return next in list
            
            if (p == m_head)
            {
                // at start
                m_head = m_head->m_next;  // inc head
                if (m_head)
                    m_head->m_prev = 0;     // terminate start
                else
                    m_tail = 0;   // no more list
                
            }
            else if (p == m_tail)
            {
                m_tail = m_tail->m_prev;
                if (m_tail)
                    m_tail->m_next = 0;     // terminate end
                else
                    m_head = 0;
                
            }
            else
            {
                // at start
                Instance * prevnode = p->m_prev;  // return prev in list
                
                prevnode->m_next = nextnode;  
                nextnode->m_prev = prevnode;  
            }
            if (*p == m_current)
            {
                m_current = 0;
                m_currentnum = -1;
            }
            
            if (del_obj)
                DeleteData(*p);
            delete p;
            p = 0;
            m_NumNodes--;
        }
        
        if (m_tail && m_tail->m_next != 0)
            m_tail->m_next = 0;
        
        if (m_head && m_head->m_prev != 0)
            m_head->m_prev = 0; 
        
        return nextnode;
    }
    
    void EraseList(bool del_obj = true) 
    {
        m_current = 0;
        
        iterator p = m_head;
        while (p)
            p = Erase(p, del_obj);
        
        InitList();
        
    }
    
    void DeleteData(_BaseClass * p)
    {
        if (p)
            delete p;
    }
    
    void DeleteData(ulComPtr<_BaseClass> p)
    {
        p->DeleteLists();
    }
    
    
    void SoilList() 
    {
        iterator p;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */)
        {
            (*p)->SetChanged(true);
        }
    }
    void RefreshList() 
    {
        iterator p;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */ )
        {
            (*p)->RefreshRenderer();
        }
    }
    
    void AddListToRenderer() 
    {
        iterator p;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */ )
        {
            (*p)->AddToRenderer();
            (*p)->SetChanged(true);
        }
    }
    void RemoveListFromRenderer() 
    {
        iterator p;
        
        for(p = Head(); p; p = p->GetNext() /*NextItem(p) */)
        {
            (*p)->RemoveFromRenderer();
        }
    }
    
    
    void ApplyFunc(ulFuncPtrv function) 
    {
        iterator p;
        
        for(p = Head(); p; p = p->GetNext() /*NextItem(p) */)
        {
            (*p)->function();
        }
    }
    
    void ApplyFunc(ulFuncPtrb function, bool b) 
    {
        iterator p;
        
        for(p = Head(); p;  p = p->GetNext() /*NextItem(p) */)
        {
            (*p)->function(b);
        }
    }
    
    
    // insert at the end of the list
    iterator Insert(_TypePtr data)
    {
        if (data == 0)
            return 0;
        Instance * p = new Instance(0,0,data);
        
        if (p == 0)
            return 0;
        if (m_head)  
        { 
            m_tail->m_next = p;
            p->m_prev = m_tail;
            
            // new tail
            m_tail = p;
            m_tail->m_next = 0;
        } 
        else // no head so insert
        { 
            m_tail = m_head = p; 
            m_head->m_next = 0;
            m_head->m_prev = 0; 
        }
        m_NumNodes++;    
        
        if (m_tail && m_tail->m_next != 0)
            m_tail->m_next = 0;
        
        if (m_head && m_head->m_prev != 0)
            m_head->m_prev = 0; 
        
        return p;
    }
    
        
};



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



/*inline Float random0 (void)
{
    return (Float (rand ()) / Float (RAND_MAX)) *2.0f - 1.0f;
} */

#define RELEASE(x) if (x != NULL) {x->Release(); x = NULL;}


#define bcopy(s,d,n)  memcpy(d,s,n)
#define rand0() ((Float)rand() / 32768.0)  // excludes 1
#define rand1() ((Float)rand() / 32767.0)  // includes 1

#define UL_FRAC(x) ((x) - (Float)(int)(x))
#define UL_DOT(a,b) ((a).x*(b).x + (a).y*(b).y + (a).z*(b).z)


#define UL_M2KTS(z) ( 1.9438  * (z) ) /* macro to convert m/s to kts    */
#define UL_KTS2M(z) ( (z) / 1.7821782 ) /* macro to convert kts to m/s  */
#define UL_M2FT(z) ( 3.28 * (z) ) /* macro to convert meters to feet    */
#define UL_FT2M(z) ( (z) / 3.28 ) /* macro to convert feet to meters    */
#define UL_NM2FT(z) ( (z) * 6075.0 ) /* macro to convert nautical miles to feet    */
#define UL_NM2M(z) ( (z) * 1852.0 )  /* macro to convert nautical miles to meters  */
#define UL_M2NM(z) ( (z) / 1852.0 )  /* macro to convert meters to nautical miles  */



#define arcsec_lon_to_meters(x)  ((Float)(x) * 21.94061103)
#define arcsec_lat_to_meters(x)  ((Float)(x) * 30.0)
#define lon_to_meters(x)  ((Float)(x) * 78986.19971)
#define lat_to_meters(x)  ((Float)(x) * 108000.0)
#define meters_to_lon(x)  ((Float)(x) / 78986.19971)
#define meters_to_lat(x)  ((Float)(x) / 108000.0)
#define feet_to_meters(x)  ((Float)(x) * 0.3048)
#define meters_to_feet(x)  ((Float)(x) / 0.3048)






#define roundf(x)  truncf((x) + 0.5)




// r = 1/p
#define ulFpInvert(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

///////////////////////////////////////////////////////
// Thanks to Vincent Van Eeckhout for this

const float two = 2.0f;

#define FP_INV2(r,p)                     \
{                                        \
    __asm { mov     eax,0x7F000000    }; \
    __asm { sub     eax,dword ptr [p] }; \
    __asm { mov     dword ptr [r],eax }; \
    __asm { fld     dword ptr [p]     }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fsubr   [two]             }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fstp    dword ptr [r]     }; \
}

///////////////////////////////////////////////////////

#define ulFpExponent(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + FP_ONE_BITS;                                                \
    e = *(float *)&_i;                                                       \
}

/*float f;
long l;

  
    inline long ulFpNormToByte(float f)
    {
    f += (1L << (23 - 8));
    l = 0xff & (*(DWORD *)&f);
    }
*/

#define ulFpNormToByte(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    i = *(int *)&_n;                                                         \
    if (i >= 0x40000000)     i = 0xFF;                                       \
    else if (i <= FP_ONE_BITS) i = 0;                                          \
    else i = ((i) >> 15) & 0xFF;                                             \
}


/*
inline DWORD FP_NORM_TO_BYTE2(float p)                                                 
{                                                                            
float fpTmp = p + 1.0f;                                                      
return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
}

  
    inline DWORD FP_NORM_TO_BYTE3(float p)     
    {
    float ftmp = p + 12582912.0f;                                                      
    return ((*(DWORD *)&ftmp) & 0xFF);
    }
*/







__forceinline void ulFloatToInt(int *int_pointer, float f) 
{
    __asm  fld  f
    __asm  mov  edx,int_pointer
    __asm  FRNDINT
    __asm  fistp dword ptr [edx];
    
}



typedef enum PlaneRelationship
{
    OFF_PLANE  ,
        ON_PLANE ,
        ON_POLYGON,           
} PlaneRelationship;


//////////////////////////////////////////////////////////////////////
// These are defined in each of the class's header files:

//inline ostream &operator<<(ostream &os, const Vec3 &v)  {return v.print(os);}
//inline ostream &operator<<(ostream &os, const Mat3 &M)  {return M.print(os);}
//inline ostream &operator<<(ostream &os, const MatR &X)  {return X.print(os);}
//inline ostream &operator<<(ostream &os, const Quat &q)  {return q.print(os);}
//inline ostream &operator<<(ostream &os, const MatQ &T)  {return T.print(os);}

//////////////////////////////////////////////////////////////////////
// combined operators

#include "..\CommonSrc\Utility\ulMatrixVec.h"



//////////////////////////////////////////////////////////////////////
// Graphics file stuff:

// ### Graphics file routines moved to nvImageLib library
//  What was ulGraphicsFile.h is now NVI_ImageLib.h


///////////////////////////////////////////////////////////
//  class ulOrientation

#include "..\CommonSrc\Utility\ulOrientation.h"


///////////////////////////////////////////////////////////

void ulComputeYDDerivative(double *Y, int len, double *YD);
void ulTriDiagonal(double *D, int len);
int ulComputeYDDerivativeIrregular(double *X, double *Y, double *YD, int len);
int ulTriDiagonalIrregular(double *A, double *B, double *C, double *D, int len);


          

#endif  // UL_H




