/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __MOVE_H__
#define __MOVE_H__

enum {M_MOVED=1,M_SCALED,M_ROTATED,M_SHIFTED};

//---------------------------------------------------------------------------------------
// mouse move class
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
};

#endif // __MOVE_H__
