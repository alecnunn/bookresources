
#ifndef GAMEENTITIES_H_
#define GAMEENTITIES_H_


class GameEntity
{
public:
    virtual ~GameEntity() {}
    virtual const char * GetClassName() { return "GameEntity"; }

};


class GameCamera : public GameEntity
{
public:
    virtual const char * GetClassName() { return "GameCamera"; }

};




#endif