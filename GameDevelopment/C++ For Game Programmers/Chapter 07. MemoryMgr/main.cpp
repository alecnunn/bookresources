
#include "MemoryMgr.h"
#include "HeapFactory.h"
#include "GameEntity.h"
#include "PhysicsInfo.h"


int main()
{
    int bookmark = HeapFactory::GetMemoryBookmark ();

    // This is just a normal allocation. Goes in the root heap
    char * pText = new char[1024];

    // This allocation should automatically go in the physics info heap
    PhysicsInfo * pPhysics = new PhysicsInfo;    

    // This allocation should automatically go into their hierarchical heaps
    GameEntity * pEntity = new GameEntity;    
    GameEntity * pCamera = new GameCamera;
    GameEntity * pCamera2 = new GameCamera;
    const int numActors = 20;
    GameEntity * pActor[numActors];
    for (int i=0; i<numActors; ++i)
        pActor[i] = new GameActor;

    // This is a custom allocation in a specific heap
    Heap * pHeap = HeapFactory::CreateHeap("CustomHeap");
    int * pBuffer = new (pHeap) int[512];

    HeapFactory::PrintInfo ();


    // Delete it all
    delete pBuffer;
    for (int j=0; j<numActors; ++j)
        delete pActor[j];
    delete pCamera;
    // We leave this one without freeing on purpose to see the memory leak
    //delete pCamera2;
    delete pEntity;
    delete pPhysics;
    delete pText;

    HeapFactory::PrintInfo ();

    HeapFactory::ReportMemoryLeaks (bookmark);

    
    return 0;
}

