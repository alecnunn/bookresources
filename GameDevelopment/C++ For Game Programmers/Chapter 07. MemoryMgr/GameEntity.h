

#ifndef GAMENTITY_H_
#define GAMENTITY_H_


#include "MemoryMgr.h"


class GameEntity
{
public:
    int someData;
    float moreData;
    int stuff[200];

private:
    DECLARE_HEAP;
};



class GameCamera : public GameEntity
{
public:
    int someMoreStuff;
    float andSomeMore;

private:
    DECLARE_HEAP;
};



class GameActor : public GameEntity
{
public:
    char name[128];
    int  controller;
    int  state;
    char model[256];

private:
    DECLARE_HEAP;
};



#endif