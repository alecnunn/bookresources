#ifndef EW_TABLET_H
#define EW_TABLET_H

/******************************************************************************

Coral Tree ew/tablet.h
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

#include <gpl/gpl.h>

// Mutually-exclusive flags for use by derived DrawOnTablet() methods
#define EW_TABLET_RAW			0x00
#define EW_TABLET_PRETTY		0x01
#define EW_TABLET_DEFAULT		EW_TABLET_RAW

/***********************************************************************
class EW_Tablet
***********************************************************************/
class EW_Tablet
{
	public:
		EW_Tablet(void);
		virtual ~EW_Tablet(void);

		virtual GPL_Integer GetColor(void) const;
		virtual GPL_Integer GetFlags(void) const;

		virtual void SetColor(const GPL_Integer i);
		virtual void SetFlags(const GPL_Integer f);

		virtual GPL_Boolean Initialize(void) { return TRUE; }
		virtual GPL_Boolean Finalize(void) { return TRUE; }
		virtual void Clear(void) {}

		virtual void SetClipBounds(const GPL_Integer x,
									const GPL_Integer y,
									const GPL_Integer width,
									const GPL_Integer height) {}
		virtual void RectWire(const GPL_Integer x,
								const GPL_Integer y,
								const GPL_Integer width,
								const GPL_Integer height);
		virtual void RectFill(const GPL_Integer x,
								const GPL_Integer y,
								const GPL_Integer width,
								const GPL_Integer height);
		virtual void Text(const GPL_Integer x, const GPL_Integer y,
							const GPL_String & string);

	protected:
		GPL_Integer colorIndex;
		GPL_Integer flags;
};

/***********************************************************************
class EW_PostScriptTablet
***********************************************************************/
class EW_PostScriptTablet : public EW_Tablet
{
	public:
		EW_PostScriptTablet(void);
		virtual ~EW_PostScriptTablet(void);

		virtual GPL_Boolean Initialize(void);
		virtual void Clear(void);

		virtual void SetClipBounds(const GPL_Integer x,
									const GPL_Integer y,
									const GPL_Integer width,
									const GPL_Integer height);
		virtual void RectWire(const GPL_Integer x,
								const GPL_Integer y,
								const GPL_Integer width,
								const GPL_Integer height);
		virtual void RectFill(const GPL_Integer x,
								const GPL_Integer y,
								const GPL_Integer width,
								const GPL_Integer height);
		virtual void Text(const GPL_Integer x, const GPL_Integer y,
							const GPL_String & string);

		virtual GPL_Boolean Save (const GPL_String & fileName);

	protected:
		GPL_Integer clipX;
		GPL_Integer clipY;
		GPL_Integer clipWidth;
		GPL_Integer clipHeight;
		GPL_List<GPL_String> prefix;
		GPL_List<GPL_String> postfix;
		GPL_List<GPL_String> lines;

		static GPL_Boolean SaveStrings (GPL_File & file,
										const GPL_List<GPL_String> & strings);
};

#endif /* EW_TABLET_H */
