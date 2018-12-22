/*----------------------------------------------------------------------------
    ENTITIES.H
    Simply publicises all the entity procedures.  Only needed by the module
    that registers the entity classes.

    This example is in C, and suffers from the drawbacks of typecasting,
    switch statements and the like.  However, our point is to show entity
    management at its most basic level and thus provide inspiration for
    your own custom implementation.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifndef _ENTITIES_H         // include guard
#define _ENTITIES_H

#ifdef  __cplusplus         // make C++ friendly
extern "C" {
#endif


/*--------------------------------------------------------------------------*/
/*                             Public Prototypes                            */    
/*--------------------------------------------------------------------------*/

ENT_PROC NullProc;
ENT_PROC MissileProc;


/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifdef  __cplusplus
}
#endif

#endif  // _ENTITIES_H
