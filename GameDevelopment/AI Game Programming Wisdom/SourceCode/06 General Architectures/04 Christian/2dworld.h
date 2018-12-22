

#ifndef _World2D_H_
#define _World2D_H_

#include <string>

class World2D;
class WorldPos;

class WorldObj
{

public:

    WorldObj ( World2D * world, char * name );           

    void   setPos   ( int x, int y );             // set the world pos center point
    void   setSize  ( int width, int height );    // set the size of the object
    void   setSpeed ( float speed );              // set the speed of the object
    void   move     ( int x, int y );             // move the object
    char * getName  ();                           // get the object name

private:
    
    std::string m_name;                           // object name
    WorldPos *  m_pos;          
    World2D  *  m_world;
};

class WorldPos
{
    friend World2D;

public:

    void       setObject ( WorldObj * obj );     // register an obj on this pos
    WorldObj * getObj    ();                     // get the object on this ps

private:
    int        m_id;    // world position id
    WorldObj * m_obj;   // object in this position
    int        m_type;  // pos type 

};

class World2D
{

public:

    World2D ( int length, int width );

    void setTileDistance ( float dist );

private:

    int   m_length;
    int   m_width;

    float m_tileDist;

    WorldPos * m_posArray;
};

#endif