/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#ifndef __MOVE_H__
#define __MOVE_H__
#include "types.h"

class MMove  
{
public:
    MMove():_ldown(0),_rdown(0),_mdown(0){};
    virtual ~MMove(){};
    CPoint  _pt;
	CPoint  _ptd;
    BOOL    _ldown;
    BOOL    _rdown;
    BOOL    _mdown;
    V3      _wpdown;
};

#endif // __MOVE_H__
