/*----------------------------------------------------------------------------
    ENTITY UTILITIES.C
    Common utilities for manipulating game entities.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/


/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/

#include    <stdio.h>
#include    "SystemStub.h"
#include    "EntityManager.h"
#include    "EntityUtilities.h"


/*--------------------------------------------------------------------------*/
/*                            Defined Constants                             */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              Function Macros                             */    
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                            External Variables                            */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Local Prototypes                             */    
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
    ENT FIRE PROJECTILE
    This example shows how a projectile entity can be "launched" from another
    entity in a very generic manner.

    This routine can become more and more robust, handling special features
    required by more advanced projectiles.  Simpler or older projectiles will
    simply ignore any advanced messages.
*/

ENTITY* EntFireProjectile(
    char*       className,      // type of projectile to create
    ENTITY*     entShooter,     // shooter/owner of projectile
    ENTITY*     entParent)      // scene graph parent
    {
    static int  id = 0;         // used for uniquely naming projectiles
    ENTITY*	    entProj;        // projectile entity
    VECTOR3     pos;            // position of shooter/projectile
    VECTOR3     dir;            // direction vector of shooter
    VECTOR3     vel;            // velocity vector of shooter
    char        shooterName[ENT_NAME_LEN];
    char        projName[ENT_NAME_LEN];

    // create the projectile as a scene-graph child of 'parent'
    entProj = EntCreateEntity(className, entParent, 0, 0);
    if (entProj)
        {
        // get position, direction and velocity and name from the shooter
        EntSendMessage(entShooter, EM_GETPOS,  (int)&pos, 0);
        EntSendMessage(entShooter, EM_GETDIR,  (int)&dir, 0);
        EntSendMessage(entShooter, EM_GETVEL,  (int)&vel, 0);
        EntSendMessage(entShooter, EM_GETNAME, (int)shooterName, 0);

        // setup the new projectile
        sprintf(projName, "Missile %i fired by %s", id, shooterName);
        EntSendMessage(entProj, EM_SETNAME,  (int)projName, 0);
        EntSendMessage(entProj, EM_SETPOS,   (int)&pos, 0);
        EntSendMessage(entProj, EM_SETDIR,   (int)&dir, 0);
        EntSendMessage(entProj, EM_SETVEL,   (int)&vel, 0);
        EntSendMessage(entProj, EM_SETOWNER, (int)entShooter, 0);
        EntSendMessage(entProj, EM_START,    0, 0);
        }

    id++;

    return(entProj);
    }


/*----------------------------------------------------------------------------
    ENT HANDLE DAMAGE
    This example shows some generic gamage handling and point awarding.
    An entity can call this routine when he has been struck by a projectile.
*/

void DamageHandler(             // processes a EM_DAMAGE message
    ENTITY* entMe,              // this entity
    ENTITY* entSender,          // var1 = entity doing the damage
    int     hitVal,             // var2 = hitpoints of damage to do
    int*    hitPoints,          // entMe's current hitpoint variable
    int     pointValue)         // entMe's point value when killed
    {
    ENTITY* entOwner=NULL;      // owner of sender (if a projectile)

    // reduce my hitpoints and determine if I'm dead
    *hitPoints -= hitVal;
    if (*hitPoints < 0)
        {
        // bummer, I'm dead! get the sender's owner (if any)
        EntSendMessage(entSender, EM_GETOWNER, (int)&entOwner, 0);
        // If he is owned, he must be a projectile, so give his
        // owner the points.  Otherwise, give sender the points.
        if (entOwner)
            EntSendMessage(entOwner, EM_GIVEPOINTS, pointValue, 0);
        else
            EntSendMessage(entSender, EM_GIVEPOINTS, pointValue, 0);

        // find the entity tree's root, and tell the world I'm dead
        EntSendMessagePre(EntGetRoot(entMe), EM_IMDEAD, 0, 0);
        // destroy myself
        EntSendMessage(entMe, EM_SHUTDOWN, 0, 0);
        }
    }
