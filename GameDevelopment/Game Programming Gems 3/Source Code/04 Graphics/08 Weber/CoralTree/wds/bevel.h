/******************************************************************************

Coral Tree wds/bevel.h
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


/* BevelBox states (enumeration) */
#define WDS_BEVELBOX_BORDERLESS		0x00000000
#define WDS_BEVELBOX_SEAM			0x00000001
#define WDS_BEVELBOX_RIDGE			0x00000002

#define WDS_BEVELBOX_IN				0x00000004
#define WDS_BEVELBOX_OUT			0x00000008

#define WDS_BEVELBOX_GROOVE			( WDS_BEVELBOX_SEAM | WDS_BEVELBOX_IN )
#define WDS_BEVELBOX_BORDER			( WDS_BEVELBOX_SEAM | WDS_BEVELBOX_OUT )
#define WDS_BEVELBOX_RIDGED			( WDS_BEVELBOX_BORDER | WDS_BEVELBOX_RIDGE )
#define WDS_BEVELBOX_MOAT			( WDS_BEVELBOX_GROOVE | WDS_BEVELBOX_RIDGE )

#define WDS_BEVELBOX_SMOOTH			0x00000010	// use HALF_LIT and HALF_SHADOW
#define WDS_BEVELBOX_ROUND			0x00000020	// round corners

#define WDS_BEVELBOX_OMIT_RIGHT		0x00000040
#define WDS_BEVELBOX_OMIT_LEFT		0x00000080
#define WDS_BEVELBOX_OMIT_TOP		0x00000100
#define WDS_BEVELBOX_OMIT_BOTTOM	0x00000200

#define WDS_BEVELBOX_POINT			0x00000400
#define WDS_BEVELBOX_POINT_VERTICAL	0x00000800
#define WDS_BEVELBOX_POINT_BACK		0x00001000
#define WDS_BEVELBOX_POINT_RIGHT	( WDS_BEVELBOX_POINT )
#define WDS_BEVELBOX_POINT_LEFT		( WDS_BEVELBOX_POINT | WDS_BEVELBOX_POINT_BACK )
#define WDS_BEVELBOX_POINT_UP		( WDS_BEVELBOX_POINT | WDS_BEVELBOX_POINT_VERTICAL )
#define WDS_BEVELBOX_POINT_DOWN		( WDS_BEVELBOX_POINT | WDS_BEVELBOX_POINT_VERTICAL | WDS_BEVELBOX_POINT_BACK )
#define WDS_BEVELBOX_ANGLE_45		0x00002000 // constrain to perfect angles
#define WDS_BEVELBOX_POINT_45		(WDS_BEVELBOX_POINT|WDS_BEVELBOX_ANGLE_45)

#define WDS_BEVELBOX_LABEL_LEFT		0x00004000	// left justify

#define WDS_BEVELBOX_ELLIPTICAL		0x00008000

#define WDS_BEVELBOX_MARK_MASK		0x00030000	// for figures
#define WDS_BEVELBOX_CHECKMARK		0x00010000
#define WDS_BEVELBOX_PLUSMARK		0x00020000
#define WDS_BEVELBOX_MINUSMARK		0x00030000

/* next space:
#define WDS_BEVELBOX_NEXT			0x00040000
*/
