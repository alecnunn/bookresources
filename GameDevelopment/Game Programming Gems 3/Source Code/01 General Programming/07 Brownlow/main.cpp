#include "savemgr.hpp"

#define PLAY_ID 1

class PLAYER : public SAVEOBJ
{
public:
    /* constructors/members omitted */
    int     GetSaveID();
    SAVERECORD *GetSaveData();

protected:
/* This is the table returned by GetSaveData() */
    static SAVERECORD player_savedata[];

/* This is the data for the PLAYER class */
    int    mat;
    int    *targetNTT;
};

/* The data saved for class PLAYER */
SAVERECORD PLAYER::player_savedata[] =
{
    SAVEDATA(PLAYER,mat),
    SAVEPTR(PLAYER,targetNTT),
    SAVEDONE()
};

SAVERECORD *PLAYER::GetSaveData()
{   /* return the data table for PLAYER */
    return player_savedata;
}

int PLAYER::GetSaveID()
{   /* return a unique ID for this class */
    return PLAY_ID;
}

/****************************************
    This is the class factory function.
    It takes a class id and makes an
    object of the correct type
****************************************/
SAVEOBJ *SAVEMGR::MakeObject( int classID )
{
    switch( classID )
    {
    case PLAY_ID:
        return new PLAYER();
    }
    return NULL;
}

