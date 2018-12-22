/*----------------------------------------------------------------------------
    ENTITY APP
    A simple skeleton application showing the use of the entity manager.

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

#define DATA_PATH   "."

/*--------------------------------------------------------------------------*/
/*                              Data Structures                             */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                            External Variables                            */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Local Prototypes                             */    
/*--------------------------------------------------------------------------*/

static void InitEntityClasses  (char*       dataPath);

static void CreateEntities     (void);
static void DestroyAllEntities (ENTITY*     entRoot);

static void GameFrame          (void);
static void GameProcessInput   (void);
static void GameUpdateWorld    (void);
static void GameDraw           (void);


/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/

static int      done            = FALSE;
static int      debugMode       = FALSE;
static int      editMode        = FALSE;
static ENTITY*  entWorld        = NULL;
static ENTITY*  entPlayer       = NULL;
static ENTITY*  entBeingEdited  = NULL;


/*----------------------------------------------------------------------------
    MAIN
*/

int main()
    {
    InitEntityClasses(DATA_PATH);

    CreateEntities();

    // run 2 frames of the game
    GameFrame();
    GameFrame();

    DestroyAllEntities(entWorld);

    EntDestroyAllClasses();

    return(0);
    }


/*----------------------------------------------------------------------------
    GAME FRAME
*/

static void GameFrame()
    {
    printf("FRAME-----------------\n");
    GameProcessInput();
    GameUpdateWorld();
    GameDraw();
    }


/*----------------------------------------------------------------------------
    GAME PROCESS INPUT
*/

static void GameProcessInput()
   {
   // map input events, then send appropriate events to the entity(ies)
   // currently under human control

   //GetInputEvents(&inputEvent);
   //MapInputEventsToGameEvents(&inputEvent, &gameEvent);
   //EntSendMessage(entPlayer, EM_USERINPUT, (int)&gameEvent, 0);
   }

   
/*----------------------------------------------------------------------------
    GAME UPDATE
*/

static void GameUpdateWorld()
    {
    EntSendMessagePre(entWorld, EM_UPDATE, GetElapsedMs(), 0);
    EntSendMessagePre(entWorld, EM_POSTUPDATE, 0, 0);
    EntCleanup(entWorld);
    }


/*----------------------------------------------------------------------------
    GAME DRAW
*/

static void GameDraw()
    {
    // let all entities render themselves, and then overlays
    EntSendMessagePre(entWorld, EM_DRAW,        0, 0);
    EntSendMessagePre(entWorld, EM_DRAWSHADOW,  0, 0);
    EntSendMessagePre(entWorld, EM_DRAWOVERLAY, 0, 0);    

    // if in debug mode, draw extra info for entities
    if (debugMode)
        EntSendMessagePre(entWorld, EM_DRAWDEBUG, 0, 0);

    // draw editing visualization for currently selected entity
    if (editMode)
        EntSendMessage(entBeingEdited, EM_DRAWEDIT, 0, 0);        
    }


/*----------------------------------------------------------------------------
    INIT ENTITY CLASSES
    Sets up all the entity classes to be used by the game.  
    EntCreateClass() sends the EM_CLSINIT message to the newly created entity
    class's proc.  In this sample, the convention is to pass the path to the
    game's data directory as var1 so classes can load assets such as sounds, 
    models and textures.
*/

static void InitEntityClasses(
    char*   dataPath)
    {
    EntCreateClass(NullProc,    (int)dataPath, 0);
    EntCreateClass(MissileProc, (int)dataPath, 0);
    }


/*----------------------------------------------------------------------------
    CREATE ENTITIES
    Creates a world and some entities.
*/

static void CreateEntities()
    {
    // create the world/root node
    entWorld = EntCreateEntity("Null", NULL, 0, 0);
    EntSendMessage(entWorld, EM_SETNAME, (int)"World", 0);

    // create the player as a child of the world
    entPlayer = EntCreateEntity("Null", entWorld, 0, 0);
    EntSendMessage(entPlayer, EM_SETNAME, (int)"Player", 0);

    // start everybody up
    EntSendMessagePre(entWorld, EM_START, 0, 0);

    // fire off a few missiles
    EntFireProjectile("Missile", entPlayer, entWorld);
    EntFireProjectile("Missile", entPlayer, entWorld);
    }


/*----------------------------------------------------------------------------
    DESTROY ALL ENTITIES
    Destroying all entities in the world is as simple as sending the 
    EM_DESTROY message, and then calling EntCleanup();
*/

static void DestroyAllEntities(
    ENTITY*     entRoot)
    {
    assert(entRoot);

    EntSendMessagePre(entRoot, EM_DESTROY, 0, 0);
    EntCleanup(entRoot);
    }