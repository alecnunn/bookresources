/*----------------------------------------------------------------------------
    ENTITY MANAGER.H
    Package to manage game entities.

    This example is in C, and suffers from the drawbacks of typecasting,
    switch statements and the like.  However, our point is to show entity
    management at its most basic level and thus provide inspiration for
    your own custom implementation.

    Contact: Matthew Harmon - matt@matthewharmon.com
*/

/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifndef _ENTITYMANAGER_H    // include guard
#define _ENTITYMANAGER_H

#ifdef  __cplusplus         // make C++ friendly
extern "C" {
#endif


/*--------------------------------------------------------------------------*/
/*                              Function Macros                             */    
/*--------------------------------------------------------------------------*/

// useful typecasting macros to convert entity message parameters

#define INT2FLOAT(i)  (*(float*)&(i))       // gets a float out of an int
#define FLOAT2INT(i)  (*(int*)&(i))         // stuffs a float into an int


/*--------------------------------------------------------------------------*/
/*                             Defined Constants                            */
/*--------------------------------------------------------------------------*/

#define ENT_CLASS_NAME_LEN  64
#define ENT_NAME_LEN        64

typedef enum entCmdTag      // some basic entity messages - expand as needed
    {
                            // CLASS OPERATIONS
    EM_CLSINIT,             // initialize class
    EM_CLSFREE,             // free up class
    EM_CLSGUID,             // gets the globally unique ID of this entity class
    EM_CLSNAME,             // gets the textual class name
                            // CREATION AND DESTRUCTION
    EM_CREATE,              // create entity (object)
    EM_START,               // turn the entity 'on'
    EM_SHUTDOWN,            // destroy entity gracefully
    EM_DESTROY,             // destroy entity immediately

                            // BASIC OPERATIONS
    EM_DRAW,                // render the entity
    EM_DRAWEDIT,            // render the entity in "editing mode"
    EM_DRAWDEBUG,           // render the entity in "debug mode"
    EM_PREUPDATE,           // run AI, etc.
    EM_UPDATE,              // update position
    EM_POSTUPDATE,          // check collision, etc.
    EM_SLEVENT,             // process a game input event
    EM_DRAWSHADOW,          // draw any shadow effects, sent after DRAW
    EM_DRAWOVERLAY,         // draw "hud" overlays, send after DRAWSHADOW

                            // DATA ACCESS
    EM_SETNAME,             // sets the name of this entity
    EM_GETNAME,             // gets the name of this entity
    EM_SETPOS,              // set position (var1 = VECTOR3*)
    EM_GETPOS,              // get postion (var1 = VECTOR3*)
    EM_SETVEL,              // set velocity vector (var1 = VECTOR3*)
    EM_GETVEL,              // get velocity vector (var1 = VECTOR3*)
    EM_SETDIR,              // set direction vector (var1 = VECTOR3*)
    EM_GETDIR,              // get direction vector (var1 = VECTOR3*)
    EM_SETOWNER,            // tells an entity who "owns" him var1=ENTITY*
    EM_GETOWNER,            // ask who owns an entity var1=ENTITY*

    EM_GIVEPOINTS,          // gives and entity points
    EM_IMDEAD,              // pronouncement that an entity has died

    EM_USER0                
    } EM;


/*--------------------------------------------------------------------------*/
/*                             Data Structures                              */
/*--------------------------------------------------------------------------*/

typedef int  (ENT_PROC)(struct entityTag* ent, int command, int var1, int var2);

typedef struct entityClass
    {
    char                name[ENT_CLASS_NAME_LEN];       // eneity name - must be unique
    ENT_PROC*           Proc;           // code for this entity
    struct entityClass* next;           // next entity class in list
    } ENTCLASS;

typedef struct entityTag
    {
    ENT_PROC*           Proc;           // code for this entity
    void*               data;           // pointer to entity's data
    int                 guid;           // unique ID for networking
    int                 destroy;        // flag to remove entity
    struct entityTag*   parent;         // parent of this entity or NULL
    struct entityTag*   prevSibling;    // pointer to previous sibling (if any)
    struct entityTag*   nextSibling;    // pointer to next sibling
    struct entityTag*   child;          // pointer to first child
    } ENTITY;


/*--------------------------------------------------------------------------*/
/*                             Public Prototypes                            */    
/*--------------------------------------------------------------------------*/

// system setup
int         EntStartup     (void);
void        EntShutdown    (void);

// class management
int         EntCreateClass (ENT_PROC*   Proc,           // message handler
                            int         var1,           // var1 for EM_CLSINIT message
                            int         var2);          // var2 for EM_CLSINIT message
int         EntDestroyClass(char*       className);
void        EntDestroyAllClasses(void);
ENTCLASS*   EntFindClassByName(
                            char*       className);

// entity management
ENTITY*     EntCreateEntity(char*       className,      // class for new entity
                            ENTITY*     parent,         // parent in hierarchy
                            int         var1,           // var1 for EM_CREATE message
                            int         var2);          // var2 for EM_CREATE message
int         EntDestroyEntity(
                            ENTITY*     ent);
int         EntSendMessage (ENTITY*     ent,            // recipient
                            EM          message,        // messageID
                            int         var1,               
                            int         var2);

void        EntSendMessagePre(
                            ENTITY*     ent,
                            EM          message,
                            int         var1,
                            int         var2);
void        EntCleanup     (ENTITY*     ent);
ENTITY*     EntGetRoot     (ENTITY*     ent);           // Return topmost parent

int         DefEntityProc  (ENTITY*     ent,
                            EM          message,
                            int         var1,
                            int         var2);



/*--------------------------------------------------------------------------*/
/*                           Compilation Control                            */
/*--------------------------------------------------------------------------*/

#ifdef  __cplusplus
}
#endif

#endif  // _ENTITYMANAGER_H
