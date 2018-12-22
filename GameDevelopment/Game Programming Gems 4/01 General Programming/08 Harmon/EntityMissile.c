/*----------------------------------------------------------------------------
    ENTITY MISSILE
    Movement, drawing and dynamics code for a simple missile entity.

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

typedef struct missileTag           // missile class structure
    {
    char        name[ENT_NAME_LEN]; // a name we can use to display/referr to
    VECTOR3     velocity;           // velocity vector
    VECTOR3     position;           // world-space position
    VECTOR3     forceAccum;         // force accumulator
    MATRIX4     matModel;           // orientation matrix
    int         engineRunning;      // TRUE if engine is on
    // and lots of other cool stuff you need!
    } MISSILE;  

// this macro simplifies grabbing the entity structure out of the container node
// this essentially becomes our "this" pointer for subsequent operations
#define THISENT ((MISSILE*)entity->data)

/*--------------------------------------------------------------------------*/
/*                            External Variables                            */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Local Prototypes                             */    
/*--------------------------------------------------------------------------*/

static int  ClsInit            (char*       path);
static int  ClsFree            (void);
static int  Create             (ENTITY*     entity);
static int  Shutdown           (MISSILE*    e);
static int  Destroy            (ENTITY*     entity,
                                MISSILE*    e);
static int  Start              (MISSILE*    e);
static int  Update             (MISSILE*    e,
                                float       elapsedSec);
static int  Draw               (MISSILE*    e);
static int  SetDirection       (MISSILE*    e,
                                VECTOR3*    dir);


/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/

static char*        className = "Missile";
static int          initialized = FALSE;
static MODEL        mdlMissile;
static SOUND        launchSound;
static float        thrust;


/*----------------------------------------------------------------------------
    MISSILE PROC
*/

int MissileProc(                // message processor
    ENTITY*     entity,         // this entity's container
    EM          message,        // message to process
    int         var1,           // general purpose parameter 1
    int         var2)           // general purpose parameter 2
    {
    // process any messages that concern a Missile entity
    switch(message)
        {
        // class operations
        case EM_CLSNAME : strcpy((char*)var1, className); return(TRUE);
        case EM_CLSINIT : return( ClsInit((char*)var1) );
        case EM_CLSFREE : return( ClsFree() );

        // creation and destruction
        case EM_CREATE  : return( Create(entity) );
        case EM_SHUTDOWN: return( Shutdown(THISENT) );
        case EM_DESTROY : return( Destroy(entity, THISENT) );
        case EM_START   : return( Start(THISENT) );

        // standard actions
        case EM_UPDATE  : return( Update(THISENT, INT2FLOAT(var1)) );
        case EM_DRAW    : return( Draw(THISENT) );

        // data access
        case EM_SETNAME : strncpy(THISENT->name, (char*)var1, ENT_NAME_LEN); return(TRUE);
        case EM_GETNAME : strcpy((char*)var1, THISENT->name); return(TRUE);
        case EM_SETPOS  : V3Copy(&THISENT->position, (VECTOR3*)var1); return(TRUE);
        case EM_SETVEL  : V3Copy(&THISENT->velocity, (VECTOR3*)var1); return(TRUE);
        case EM_SETDIR  : return( SetDirection(THISENT, (VECTOR3*)var1) );
        default         : return( DefEntityProc(entity, message, var1, var2) );
        }
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    CLS INIT
    Class initialization.  Load resources, setup static values.
*/

static int ClsInit(
    char*   path)           // path to missile's data (textures, sounds, etc.)
    {
    assert(initialized == FALSE);

    // load resources that all missile instances use
    // textures, sounds, models, etc.

    // initialize values static to the missile class
    // better yet, read them out of a config file so users can change
    // the behavior of this entity class
    thrust = 1000.0f;

    // flag that this class is initialized and ready for use
    initialized = TRUE;

    printf("MISSILE class initialized.\n");
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    CLS FREE
*/

static int ClsFree()
    {
    assert(initialized);

    // free up resources

    initialized = FALSE;

    printf("MISSILE class freed.\n");
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    CREATE
    Handles the creation of a missile instance.  
    Entities receive the EM_CREATE message after somebody calls EntCreateEntity()
*/

static int Create(
    ENTITY*     entity)            // generic "container" or handle
    {
    MISSILE*    e;

    assert(entity != NULL);
    assert(initialized);

    // allocate a missile object
    e = (MISSILE*)malloc(sizeof(MISSILE));
    if (e == NULL)
        return(FAIL);

    // now, save our pointer in the entity container
    entity->data = e;

    // set initial values
    e->engineRunning = FALSE;
    V3Clear(&e->forceAccum);

    // create child objects if any - perhaps a smoke trail

    PrintCreateMessage(className);
    return(SUCCESS);
    }


/*----------------------------------------------------------------------------
    START
    By convention, entities receive a EM_START message after they have been
    created, and all their values set. 

    For example, the missile needs to play a launch sound, but it can't do this
    in the EM_CREATE handler, because it hasn't been positioned in the world yet,
    and we (of course) want a 3D positional sound.
*/

static int Start(
    MISSILE*     e)
    {
    assert(initialized);
    assert(e);

    e->engineRunning = TRUE;

    // play the missile launch sound effect

    // start the smoke trail child object

    PrintStartMessage(className, e->name);
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    SHUTDOWN
    Initiaites the graceful shutdown of a missile.  In this case, we'll turn 
    off the engine and let it crash into the ground.
*/

static int Shutdown(
    MISSILE*    e)
    {
    assert(initialized);
    assert(e);

    e->engineRunning = FALSE;

    PrintShutdownMessage(className, e->name);
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    DESTROY
*/

static int Destroy(
    ENTITY*     entity,
    MISSILE*    e)
    {
    assert(entity);
    assert(e);

    PrintDestroyMessage(className, e->name);
    // shut down any children that may still be alive

    // free this entity's memory that we allocated in Create()
    if (e)
        free(e);

    // Call the entity manager's destroy function to 
    // unlink the entity container and free it up
    EntDestroyEntity(entity);

    return(TRUE);
    }


/*----------------------------------------------------------------------------
    DETONATE
*/

static int Detonate(
    ENTITY*     entity,
    MISSILE*    e)
    {
    assert(initialized);
    assert(entity);
    assert(e);

    // send a message to ourselves!
    EntSendMessage(entity, EM_DESTROY, 0, 0);

    return(TRUE);
    }


/*----------------------------------------------------------------------------
    SET DIRECTION
    Takes an incoming direction vector and orients the missile to match it.
*/

static int SetDirection(
    MISSILE*    e,
    VECTOR3*    dir)
    {
    assert(initialized);
    assert(e);
    assert(dir);

    // perhaps build matrix based on direction vector and "up" vector.
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    UPDATE
    Update the missile's position in space.  Run dynamics code, apply forces, etc.
*/

static int Update(
    MISSILE*    e,
    float       elapsedSec)
    {
    assert(initialized);
    assert(e);

    if (e->engineRunning)
        {
        // apply thrust
        }

    // calculate and accumulate forces due to gravity, drag, etc.

    // update the missile's position

    // clear out the force accumulator so external influences that come in
    // via messages can affect the missile
    V3Clear(&e->forceAccum);

    PrintUpdateMessage(className, e->name);
    return(TRUE);
    }


/*----------------------------------------------------------------------------
    DRAW
*/

static int Draw(
    MISSILE*    e)
    {
    PrintDrawMessage(className, e->name);
    return(TRUE);
    }
