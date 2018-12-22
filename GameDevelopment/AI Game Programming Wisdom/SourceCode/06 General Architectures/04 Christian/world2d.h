

#ifndef _World2D_H_
#define _World2D_H_

#include <string>

class World2D;
class WorldPos;
class WorldObj;

class WorldObj
{

public:

    WorldObj();

    void  setWorld ( World2D * world, char * name );           

    void  setPos   ( int x, int y );             // set the world pos center point
    void  getPos   ( int & x, int & y );         // get the world pos coords

    WorldPos * getWPos () { return m_pos; }
    World2D  * getWorld() { return m_world; }

    const char * getWOName  ();                  // get the object name

    void  move ();

    void  setMotionSpeed ( float speed ) 
        { m_speed = speed; }

    bool  inNewPos ();

    void setDirection ( int xdir, int ydir )
        { m_xdir = xdir; m_ydir = ydir; }

    void getDirection ( int &xdir, int &ydir )
        { xdir = m_xdir; ydir = m_ydir; }

private:
    
    std::string m_name;     // object name
    WorldPos *  m_pos;          
    World2D  *  m_world;

    float m_lastx;          // last world x
    float m_lasty;          // last world y;

    float m_speed;          // speed of movement in tile distance per second
    float m_xdist;          // x distance moved across tile
    float m_ydist;          // y distance

    int   m_xdir;           // x direction
    int   m_ydir;           // y direction
};

class WorldPos
{
    friend World2D;
    friend WorldObj;

public:

    WorldPos();

    void       setObject ( WorldObj * obj );     // register an obj on this pos
    WorldObj * getObj    ();                     // get the object on this ps

private:
    int        m_id;    // world position id
    WorldObj * m_obj;   // object in this position

    int m_x;    // world x
    int m_y;    // world y
};

class World2D
{

    friend WorldObj;

public:

    World2D  ( int width, int length, float tileDist );
    ~World2D ();

    WorldPos * setPos ( int x, int y, WorldObj * obj );
    WorldPos * getPos ( int x, int y );

    int getWidth ()  { return m_width; }
    int getLength () { return m_length; }

private:

    int   m_width;
    int   m_length;

    float m_tileDist;

    WorldPos ** m_posArray;
};

#endif