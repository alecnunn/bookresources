
#include "GameEntity.h"
#include "GameCamera.h"
#include "StreamFile.h"
#include "EntityFactory.h"
#include "AddressTranslator.h"
#include <iostream>
#include <assert.h>


GameEntity * CreateGame()
{
    GameEntity * pRoot = new GameEntity("Root");

    GameCamera * pCamera = new GameCamera("Camera1");
    pCamera->m_FOV = 80.0f;
    pCamera->m_nearPlane = 10.0f;
    pCamera->m_farPlane  = 500.0f;

    GameEntity * pEntity1 = new GameEntity("Entity1");
    GameEntity * pEntity2 = new GameEntity("Entity2");

    pRoot->AddChild(pCamera);
    pRoot->AddChild(pEntity1);
    pRoot->AddChild(pEntity2);

    GameEntity * pEntity3 = new GameEntity("Entity3");
    GameEntity * pEntity4 = new GameEntity("Entity4");
    pEntity1->AddChild(pEntity3);
    pEntity1->AddChild(pEntity4);

    GameEntity * pEntity5 = new GameEntity("Entity5");
    pEntity4->AddChild(pEntity5);

    pCamera->m_pTarget = pEntity4;

    return pRoot;
}


void DeleteTree (GameEntity * pRoot)
{
    int numChildren = pRoot->GetNumChildren();
    for (int i=0; i!=numChildren; ++i)
    {
        GameEntity * pChild = pRoot->GetChild(i);
        DeleteTree (pChild);
        delete pChild;
    }
}


void PrintTree(GameEntity * pRoot, int indent = 0)
{
    //for (int col=0; col<indent; ++col)
    //    std::cout << "  ";

    //std::cout << pRoot->GetClass() << ": " << pRoot->GetName() << std::endl;
    pRoot->Print(indent);

    int numChildren = pRoot->GetNumChildren();
    for (int i=0; i<numChildren; ++i)
    {
        GameEntity * pChild = pRoot->GetChild(i);
        PrintTree (pChild, indent+1);
    }
}



void SaveTree(IStream & stream, GameEntity * pRoot)
{
    WriteString(stream, pRoot->GetClass());
    WriteInt(stream, (int)pRoot);
    pRoot->Write(stream);

    int numChildren = pRoot->GetNumChildren();
    for (int i=0; i<numChildren; ++i)
    {
        GameEntity * pChild = pRoot->GetChild(i);
        SaveTree (stream, pChild);
    }
}


void SaveGame(GameEntity * pRoot, const std::string & filename)
{
    StreamFile stream;
    stream.OpenWrite(filename);
    SaveTree(stream, pRoot);
    stream.Close();
}


GameEntity * LoadEntity(IStream & stream)
{
    std::string strClassName = ReadString(stream);
    GameEntity * pEntity = EntityFactory::Create(strClassName);
    assert (pEntity!= NULL);

    void * pOldAddress = (void *)ReadInt(stream);
    AddressTranslator::AddAddress(pOldAddress, pEntity);

    pEntity->Read(stream);
    return pEntity;
}



GameEntity * LoadTree(IStream & stream)
{
    GameEntity * pEntity = LoadEntity(stream);

    if (pEntity != NULL)
    {
        int numChildren = pEntity->GetNumChildren();
        for (int i=0; i<numChildren; ++i)
            LoadTree(stream);
    }

    return pEntity;
}


void FixupPass(GameEntity * pRoot)
{
    pRoot->Fixup();

    int numChildren = pRoot->GetNumChildren();
    for (int i=0; i<numChildren; ++i)
        FixupPass(pRoot->GetChild(i));
}



GameEntity * LoadGame(const std::string & filename)
{
    StreamFile stream;
    stream.OpenRead(filename);
    GameEntity * pRoot = LoadTree(stream);
    FixupPass(pRoot);
    stream.Close();

    return pRoot;
}



int main()
{
    GameEntity * pRoot = CreateGame();

    std::cout << "Original tree:" << std::endl;
    PrintTree(pRoot);

    std::string filename = "game.bin";
    SaveGame(pRoot, filename);
    DeleteTree(pRoot);

    pRoot = LoadGame(filename);

    std::cout << std::endl << "Loaded tree:" << std::endl;
    PrintTree(pRoot);

    return 0;
}

