/*----------------------------------------------------------------------------
    ENTITY MATH.H
    Generic math routines provided to allow example code to compile.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                            Compilation Control                           */
/*--------------------------------------------------------------------------*/

#ifndef _ENTITYMATH_H       // include guard
#define _ENTITYMATH_H
    
#ifdef  __cplusplus         // make C++ friendly
extern "C" {
#endif


/*--------------------------------------------------------------------------*/
/*                             Defined Constants                            */
/*--------------------------------------------------------------------------*/

#define TRUE    1
#define FALSE   0

#define SUCCESS 0
#define FAIL   -1

/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/

typedef struct point3Tag            // three dimensional points and vectors
    {
    float   x;
    float   y;
    float   z;
    } POINT3, VECTOR3;

typedef struct matrix4Tag           // 4 by 4 matrix
    {
    float   e[4][4];
    } MATRIX4;


typedef void*   MODEL;              // 3d model
typedef void*   SOUND;              // sound effect


/*--------------------------------------------------------------------------*/
/*                              Function Macros                             */    
/*--------------------------------------------------------------------------*/

#define V3Clear(v)          memset((v), 0, sizeof(VECTOR3))
#define V3Copy(vd, vs)      memcpy((vd), (vs), sizeof(VECTOR3))

#define  GetElapsedMs()     (1000/60)


/*--------------------------------------------------------------------------*/
/*                             Public Prototypes                            */    
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                            Compilation Control                           */
/*--------------------------------------------------------------------------*/

#ifdef  __cplusplus         // make C++ friendly
}
#endif

#endif  // _ENTITYMATH_H
