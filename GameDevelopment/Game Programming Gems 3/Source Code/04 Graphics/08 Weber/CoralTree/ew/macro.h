/******************************************************************************

Coral Tree ew/macro.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/



// result = 'a' with bits in bitmask 'b' turned off
#define EW_MASK_EXCLUDING(a,b)			( (a) ^ ((a)&(b)) )

#define EW_MIN(a,b)						( ((a)<(b))? (a): (b) )
#define EW_MAX(a,b)						( ((a)>(b))? (a): (b) )

#define EW_BOUND(a,b,c)					( EW_MIN( c, EW_MAX(a,b) ) )

#define EW_INBOUNDS(a,b,c)				( (a)<=(b) && (b)<=(c) )

// TRUE if 'mx,my' is inside 'ix,iy,sx,sy'
#define EW_WITHIN(mx,my,ix,iy,sx,sy)    (   ((mx) >= (ix))      &&  \
                                            ((mx) <  (ix)+(sx)) &&  \
                                            ((my) >= (iy))      &&  \
                                            ((my) <  (iy)+(sy)) )


#define EW_CAUSE_SEGMENTION_FAULT	*((long *)0)=0
