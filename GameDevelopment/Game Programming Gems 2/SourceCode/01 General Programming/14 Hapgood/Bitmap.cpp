/* Copyright (C) Bryon Hapgood, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bryon Hapgood, 2001"
 */
enum Format{
	RGB_3x8=0,
	RGB_565=4,
	RGB_555=8,
	RGB_4x8=12,
	RGB_1x8=16
};

struct BITMAP{
	void *pixels;
	u32 w,h,depth;
	TRIPLET *pal;
	Format pxf;
	u32 stride;
	u32 size;
~	BITMAP();
	BITMAP(int,int,int,Format,int n=1);
	void draw(int,int,BITMAP&,int,int,int,int);
	operator bool(){
		return v!=NULL;
	}
};
