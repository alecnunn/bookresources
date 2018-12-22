/*----------------------------------------------------------------------------
    ENTITY MANAGER.C
    Package to manage game entities.

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
#include    <string.h>
#include    <malloc.h>
#include    <assert.h>
#include    "SystemStub.h"          // stub code to allow examples to compile
#include    "EntityManager.h"       // Game entity manager


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
/*                             External Variables                           */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             Local Prototypes                             */    
/*--------------------------------------------------------------------------*/

ENTITY*     EntGetLastSibling  (ENTITY*     ent);
static void EntHookEntity      (ENTITY*     entity,
                                ENTITY*     parent);
static void EntUnhookEntity    (ENTITY*     ent);


/*--------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*--------------------------------------------------------------------------*/

static int          entCleanup = 0;
static ENTCLASS*    classList  = NULL;


/*----------------------------------------------------------------------------
    ENT CREATE CLASS
    Creates an entity class by inserting the new class into the linked list.
*/

int EntCreateClass(
    ENT_PROC*   Proc,
    int         var1,
    int         var2) 
    {
    ENTCLASS*   cls;
    ENTCLASS*   oldHead;
    char        className[64];

    assert(Proc != NULL);

    // get the class name from this proc by sending a message
    className[0] = 0;
    Proc(NULL, EM_CLSNAME, (int)className, 0);
    if (className[0] == 0)
        return(FAIL);

    // If name already exists in database, abort.  No duplicates allowed!
    cls = EntFindClassByName(className);
    if (cls != NULL)
        {
        assert(0);
        return(FAIL);
        }

    // allocate space for a new entity class object
    cls = malloc(sizeof(ENTCLASS));
    if (cls == NULL)
        return(FAIL);

    // copy the name and the pointer to the class proc
    strncpy(cls->name, className, 64);
    cls->Proc = Proc;

    // link the new class to the head of the list
    oldHead     = classList;
    classList   = cls;
    cls->next   = oldHead;

    // now, send the class initialization message to allow the class to load
    // media and perform other setup
    cls->Proc(NULL, EM_CLSINIT, var1, var2);

    return(SUCCESS);
    }


/*----------------------------------------------------------------------------
    ENT DESTROY CLASS
    Removes a registered class from the list of classes.
*/

int EntDestroyClass(
    char*       className)
    {
    ENTCLASS*   cls;
    ENTCLASS*   prev;

    assert(className != NULL);

    // find the class in the list
    prev = NULL;
    cls = classList;
    while (cls != NULL)
        {
        if (strcmp(cls->name, className) == 0)
            {
            // unlink the class from the list
            if (prev != NULL)
                prev->next = cls->next;
            else
                classList = cls->next;

            // send the class destroy message and free the class structure
            cls->Proc(NULL, EM_CLSFREE, 0, 0);
            free(cls);
            return(SUCCESS);
            }
        prev = cls;
        cls = cls->next;
        }

    return(FAIL);
    }


/*----------------------------------------------------------------------------
    ENT DESTROY ALL CLASSES
    Destroys all the registered entity classes.
*/

void EntDestroyAllClasses()
    {
    ENTCLASS*   cls;
    ENTCLASS*   dest;

    cls = classList;
    while (cls != NULL)
        {
        dest = cls;
        cls = dest->next;
        // send the class destroy message
        dest->Proc(NULL, EM_CLSFREE, 0, 0);
        free(dest);
        }
    }


/*----------------------------------------------------------------------------
    ENT FIND CLASS BY NAME
*/

ENTCLASS* EntFindClassByName(
    char*       className)
    {
    ENTCLASS*   cls;

    cls = classList;
    while (cls != NULL)
        {
        if (strcmpi(cls->name, className) == 0)
            return(cls);
        cls = cls->next;
        }
    return(NULL);
    }


/*----------------------------------------------------------------------------
    ENT CREATE ENTITY
    Creates a new entity.
    Links the entity to the parent as the last child.
*/

ENTITY* EntCreateEntity(
    char*       className,
    ENTITY*     parent,
    int         var1,
    int         var2)
    {
    ENTITY*     entity;
    ENTCLASS*   cls;
    int         ret;

    assert(className);
    assert(className[0]);

    // find class in database and abort if it doesn't exist
    cls = EntFindClassByName(className);
    if (cls == NULL)
        return(NULL);

    // allocate space for a new entity container node
    entity = malloc(sizeof(ENTITY));
    if (entity == NULL)
        return(NULL);

    // initialize the entity container node
    entity->destroy       = FALSE;
    entity->data          = NULL;
    entity->Proc          = cls->Proc;

    // hook this entity node in to the list
    EntHookEntity(entity, parent);

    // send the create message (basically allocs the object)
    ret = EntSendMessage(entity, EM_CREATE, var1, var2);
    // if the create process faild, then something went very wrong.
    if (ret == FAIL)
        {
        EntDestroyEntity(entity);
        return(NULL);
        }

    return(entity);
    }


/*----------------------------------------------------------------------------
    ENT GET LAST SIBLING
    Returns the last sibling of of the given entity.
*/

ENTITY* EntGetLastSibling(
    ENTITY*     ent)
    {
    ENTITY*     test;
    ENTITY*     prev;

    assert(ent);

    prev = ent;
    test = ent->nextSibling;
    while (test != NULL)
        {
        prev = test;
        test = test->nextSibling;
        }
    return(prev);
    }


/*----------------------------------------------------------------------------
    ENT GET ROOT
    Gets the topmost parent node.  Traverses the tree all the way up to the
    top.  Very useful for finding the root of the game world.
*/

ENTITY* EntGetRoot(
    ENTITY*     ent)
    {
    ENTITY*     test;
    ENTITY*     prev;

    assert(ent != NULL);

    prev = ent;
    test = ent->parent;
    while (test != NULL)
        {
        prev = test;
        test = test->parent;
        }
    return(prev);
    }


/*----------------------------------------------------------------------------
    ENT DESTROY ENTITY
    Flags an entity to be removed from the list upon next cleanup.
*/

int EntDestroyEntity(
    ENTITY*     ent)
    {
    if (ent == NULL)
        return(FAIL);

    // flag this entity to be destroyed
    ent->destroy++;

    // let the system know to clean out the destroyed entities
    entCleanup++;

    return(SUCCESS);
    }


/*----------------------------------------------------------------------------
    ENT HOOK ENTITY
    Hooks a given entity into the entity tree.
    This simplified system puts the new entity as the last child of his parent.
*/

void EntHookEntity(
    ENTITY*     entity,
    ENTITY*     parent)
    {
    ENTITY*     lastChild;

    assert(entity);

    entity->parent = parent;

    if (parent != NULL)
        {
        // if the parent has no children, I'm his first!
        if (parent->child == NULL)
            {
            parent->child = entity;
            entity->prevSibling = NULL;
            }
        else
            {
            // otherwise, make me his last child
            lastChild = EntGetLastSibling(entity->parent->child);
            lastChild->nextSibling = entity;
            entity->prevSibling = lastChild;
            }
        }
    else
        entity->prevSibling = NULL;

    entity->nextSibling = NULL;
    entity->child = NULL;
    }


/*----------------------------------------------------------------------------
    ENT UNHOOK ENTITY
    Unhooks a given entity from the list.
    Unhooking a parent may result in lost nodes!
*/

static void EntUnhookEntity(
    ENTITY*     ent)
    {
    assert(ent != NULL);

    // unhook from hierarchical linked list
    if (ent->parent != NULL)
        {
        // if I am my parent's first child, then point my parent's child
        // to my first sibling (which may be null).
        if (ent->prevSibling == NULL)
            {
            // if I have a sibling, then his prev sibling must now be my parent
            if (ent->nextSibling != NULL)
                ent->nextSibling->prevSibling = NULL;

            ent->parent->child = ent->nextSibling;
            }
        else
            {
            // otherwise, link my prev and next siblings up.
            ent->prevSibling->nextSibling = ent->nextSibling;
            if (ent->nextSibling != NULL)
                ent->nextSibling->prevSibling = ent->prevSibling;
            }
        }

    }


/*----------------------------------------------------------------------------
    ENT SEND MESSAGE
    Sends a message to an entity.  This is the heart of the whole thing!
*/

int EntSendMessage(
    ENTITY*     ent,
    EM          message,
    int         var1,
    int         var2)
    {
    assert(ent);
    assert(ent->Proc);

    return( ent->Proc(ent, message, var1,  var2) );
    }


/*----------------------------------------------------------------------------
    ENT SEND MESSAGE PRE
    Sends a message to the entity tree in pre-order traversal.  Thus, messages
    are passed onto children before onto siblings.
    Any command returning FALSE will stop depth traversal.

    Note that nodes marked for deletion are not processed, however their children
    are.  This simplifies issues regarding entity deletion during a recursive
    message session.
*/

void EntSendMessagePre(
    ENTITY*     ent,
    EM          message,
    int         var1,
    int         var2)
    {
    ENTITY*     child;

    assert(ent != NULL);

    // send this entity the message if he is really alive
    if (!ent->destroy)
        {
        ent->Proc(ent, message, var1,  var2);
        }

    child = ent->child;
    while(child)
        {
        EntSendMessagePre(child, message, var1, var2);
        child = child->nextSibling;
        }
    }


/*----------------------------------------------------------------------------
    ENT CLEANUP
    Cleans up the entity list by removing "destroyed" entities.
    This technique is used to avoid complex problems with destroying entities
    during a recursive message sending session.
    Typically, EntCleanup() should be called once per frame on the root of the
    game world.
*/

void EntCleanup(
    ENTITY*     ent)
    {
    ENTITY*     child;
    ENTITY*     nextSibling;

    assert(ent != NULL);

    // abort if there are no more entities to be cleaned up
    // entCleanup serves as a sort of short-circuit to traversing the entire list
    if (entCleanup == 0)
        return;

    // do children first
    child = ent->child;
    while(child)
        {
        nextSibling = child->nextSibling;
        EntCleanup(child);
        child = nextSibling;
        }

    // now, if I am to be removed, remove me
    if (ent->destroy)
        {
        EntUnhookEntity(ent);
        free(ent);
        entCleanup--;
        }

    }


/*----------------------------------------------------------------------------
    DEF ENTITY PROC
    Stub for a poitential default entity procedure
*/

int DefEntityProc(
    ENTITY*     ent,            // this entity
    int         message,        // message to process
    int         var1,           // general purpose parameter 1
    int         var2)           // general purpose parameter 2
    {
    switch(message)
        {
        }
    return(TRUE);
    }


