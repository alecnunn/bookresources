/*----------------------------------------------------------------------------
    ENTITY NULL
    An entity that does basically nothing!  Used for example purposes but
    also good for organizational nodes.

    This "null" entity only supports the reporting of it's class name,
    and the getting/setting of the instance name.

    In response to other messages, it simply prints some output so we can
    see what it is doing.

    This example is in C, and suffers from the drawbacks of typecasting,
    switch statements and the like.  However, our point is to show entity
    management at its most basic level and thus provide inspiration for
    your own custom implementation.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/


/*--------------------------------------------------------------------------*/
/*                              Include Files                               */
/*--------------------------------------------------------------------------*/

#include    <stdio.h>
#include    <assert.h>
#include    <malloc.h>
#include    <string.h>
#include    "SystemStub.h"          // stub code to allow examples to compile
#include    "EntityManager.h"       // entity manager
#include    "EntityUtilities.h"     // common entity utilities
#include    "Entities.h"            // entity procedure prototypes


/*--------------------------------------------------------------------------*/
/*                            Defined Constants                             */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/

typedef struct nullEntTag
    {
    char        name[ENT_NAME_LEN];
    } NULLENT;


// this macro simplifies grabbing the entity structure out of the container node
// this essentially becomes our "this" pointer for subsequent operations
#define THISENT ((NULLENT*)entity->data)

/*--------------------------------------------------------------------------*/
/*                            External Variables                            */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Local Prototypes                             */    
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/

static char* className = "NULL";

/*----------------------------------------------------------------------------
    NULL PROC
    Procedure for "NULL" entities.  This can be used for placeholder nodes.
    Often the "world" node may use this null proc.
    
*/

int NullProc(
    ENTITY*     entity,         // this entity
    int         message,        // message to process
    int         var1,           // general purpose parameter 1
    int         var2)           // general purpose parameter 2
    {
    switch(message)
        {
        case EM_CLSINIT :
            printf("NULL class initialized.\n");
            return(TRUE);

        case EM_CLSFREE :
            printf("NULL class freed.\n");
            return(TRUE);

        case EM_CLSNAME   :
            strcpy((char*)var1, className);
            return(TRUE);

        case EM_CREATE    :
            {
            NULLENT*    e;

            assert(entity != NULL);

            e = malloc(sizeof(NULLENT));
            if (e == NULL)
                return(TRUE);

            memset(e, sizeof(NULLENT), 0);

            // save data off in entity structure
            entity->data = e;

            PrintCreateMessage(className);
            return(TRUE);
            }

        case EM_START  :
            PrintStartMessage(className, THISENT->name);
            return(TRUE);

        case EM_SHUTDOWN  :
        case EM_DESTROY   :
            PrintDestroyMessage(className, THISENT->name);
            if (THISENT)
                free(THISENT);
            EntDestroyEntity(entity);
            return(TRUE);

        case EM_UPDATE :
            PrintUpdateMessage(className, THISENT->name);
            return(TRUE);

        case EM_DRAW :
            PrintDrawMessage(className, THISENT->name);
            return(TRUE);

        case EM_SETNAME   :
            strncpy(THISENT->name, (char*)var1, ENT_NAME_LEN);
            return(TRUE);

        case EM_GETNAME   :
            strcpy((char*)var1, THISENT->name);
            return(TRUE);
        }
    return(TRUE);
    }