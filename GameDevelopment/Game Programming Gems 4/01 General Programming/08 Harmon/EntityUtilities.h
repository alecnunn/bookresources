/*----------------------------------------------------------------------------
    ENTITY UTILITIES.H
    Common utilities for manipulating game entities.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifndef _ENTITY_UTILITIES_H     // include guard
#define _ENTITY_UTILITIES_H

#ifdef __cplusplus              // make C++ friendly
extern "C" {
#endif


/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                            Defined Constants                             */
/*--------------------------------------------------------------------------*/

// some helpful message printing macros
#define PrintCreateMessage(c)       printf("%s entity created.\n", c, e)
#define PrintStartMessage(c,e)      printf("%s:\"%s\" started.\n", c, e)
#define PrintDestroyMessage(c,e)    printf("%s:\"%s\" destroyed.\n", c, e)
#define PrintUpdateMessage(c,e)     printf("%s:\"%s\" update.\n", c, e)
#define PrintDrawMessage(c,e)       printf("%s:\"%s\" draw.\n", c, e)
#define PrintShutdownMessage(c,e)   printf("%s:\"%s\" shutting down.\n", c, e)



/*--------------------------------------------------------------------------*/
/*                            Public Prototypes                             */    
/*--------------------------------------------------------------------------*/

ENTITY* EntFireProjectile  (char*       className,  // type of projectile to create
                            ENTITY*     shooter,    // shooter/owner of projectile
                            ENTITY*     parent);    // scene graph parent

void    DamageHandler      (ENTITY*     entMe,      // this entity
                            ENTITY*     entSender,  // var1 = entity doing the damage
                            int         hitVal,     // var2 = hitpoints of damage to do
                            int*        hitPoints,  // entMe's current hitpoint variable
                            int         pointValue);// entMe's point value when killed


/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus          // make C++ friendly
}
#endif


#endif  // _ENTITY_UTILITIES_H
