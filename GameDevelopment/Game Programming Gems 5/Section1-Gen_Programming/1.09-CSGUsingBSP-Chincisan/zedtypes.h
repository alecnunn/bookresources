#ifndef _ZT___
#define _ZT___

// point structure
struct FPOINT
{
    REAL x;
    REAL y;
};

// rectangle structure
struct FRECT
{
    REAL left;
    REAL top;
    REAL right;
    REAL bottom;
};

// selection modes
typedef enum _BR_MODE{BR_VIEW, BR_MOVE, BR_SCALE, BR_ROTATE, BR_RISE}BR_MODE;
typedef enum _EL_SEL{SEL_NONE, SEL_BRUSH, SEL_FACE, SEL_VERTEX}EL_SEL;

// cursor id's
enum
{
    CUR_NONE=0,
    CUR_SCROLL,
    CUR_MOVE,
    CUR_SCALE,
    CUR_ROTATE,
    CUR_NS,
    CUR_EW,
    CUR_NWSE,
    CUR_NESW,
};


#endif
