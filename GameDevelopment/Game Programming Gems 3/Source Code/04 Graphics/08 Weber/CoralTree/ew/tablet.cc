/******************************************************************************

Coral Tree ew/tablet.cc
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

#include <ew/ew.h>

/***********************************************************************
class EW_Tablet
***********************************************************************/
EW_Tablet::EW_Tablet(void) : colorIndex(0), flags(EW_TABLET_DEFAULT)
{
}

EW_Tablet::~EW_Tablet(void)
{
}

GPL_Integer EW_Tablet::GetColor(void) const
{
	return colorIndex;
}

GPL_Integer EW_Tablet::GetFlags(void) const
{
	return flags;
}

void EW_Tablet::SetColor(const GPL_Integer i)
{
	colorIndex = i;
}

void EW_Tablet::SetFlags(const GPL_Integer f)
{
	flags = f;
}

void EW_Tablet::RectWire(const GPL_Integer x, const GPL_Integer y,
							const GPL_Integer width, const GPL_Integer height)
{
	EW_PRINT(EW_EW, EW_LOG, "%d,%d %dx%d", x, y, width, height);
}

void EW_Tablet::RectFill(const GPL_Integer x, const GPL_Integer y,
							const GPL_Integer width, const GPL_Integer height)
{
	EW_PRINT(EW_EW, EW_LOG, "%d,%d %dx%d", x, y, width, height);
}

void EW_Tablet::Text(const GPL_Integer x, const GPL_Integer y,
						const GPL_String & string)
{
	EW_PRINT(EW_EW, EW_LOG, "%d,%d %s", x, y, string.GetData());
}

/***********************************************************************
class EW_PostScriptTablet
***********************************************************************/
EW_PostScriptTablet::EW_PostScriptTablet(void)
{
}

EW_PostScriptTablet::~EW_PostScriptTablet(void)
{
}

GPL_Boolean EW_PostScriptTablet::Initialize(void)
{
	clipX = -1;
	clipY = -1;
	clipWidth = -1;
	clipHeight = -1;
	prefix.Append("%! PostScript");
	prefix.Append("/Helvetica findfont 6 scalefont setfont");
	prefix.Append("/VerticalMargin 72 def");
	prefix.Append("/HorizontalMargin 72 def");
	prefix.Append("% x y w h SetClipBounds");
	prefix.Append("/SetClipBounds");
	prefix.Append("{");
	prefix.Append("/Height exch def");
	prefix.Append("/Width exch def");
	prefix.Append("newpath");
	prefix.Append("VerticalMargin add exch");
	prefix.Append("HorizontalMargin add exch");
	prefix.Append("moveto");
	prefix.Append("0 Height rlineto");
	prefix.Append("Width 0 rlineto");
	prefix.Append("0 -1 Height mul rlineto");
	prefix.Append("closepath");
	prefix.Append("clip");
	prefix.Append("} def");
	prefix.Append("% x y w h RectWire");
	prefix.Append("/RectWire");
	prefix.Append("{");
	prefix.Append("/Height exch def");
	prefix.Append("/Width exch def");
	prefix.Append("newpath");
	prefix.Append("VerticalMargin add exch");
	prefix.Append("HorizontalMargin add exch");
	prefix.Append("moveto");
	prefix.Append("0 Height rlineto");
	prefix.Append("Width 0 rlineto");
	prefix.Append("0 -1 Height mul rlineto");
	prefix.Append("closepath");
	prefix.Append("stroke");
	prefix.Append("} def");
	prefix.Append("% x y w h grey RectFill");
	prefix.Append("/RectFill");
	prefix.Append("{");
	prefix.Append("/FillGrey exch def");
	prefix.Append("/Height exch def");
	prefix.Append("/Width exch def");
	prefix.Append("newpath");
	prefix.Append("VerticalMargin add exch");
	prefix.Append("HorizontalMargin add exch");
	prefix.Append("moveto");
	prefix.Append("0 Height rlineto");
	prefix.Append("Width 0 rlineto");
	prefix.Append("0 -1 Height mul rlineto");
	prefix.Append("closepath");
	prefix.Append("gsave");
	prefix.Append("FillGrey setgray");
	prefix.Append("fill");
	prefix.Append("grestore");
	prefix.Append("stroke");
	prefix.Append("} def");
	prefix.Append("% str x y Text");
	prefix.Append("/Text");
	prefix.Append("{");
	prefix.Append("VerticalMargin add exch");
	prefix.Append("HorizontalMargin add exch");
	prefix.Append("moveto");
	prefix.Append("show");
	prefix.Append("} def");
	prefix.Append("-90 rotate");
	prefix.Append("-792 0 translate");
	postfix.Append("showpage");
	return TRUE;
}

void EW_PostScriptTablet::Clear(void)
{
	clipX = -1;
	clipY = -1;
	clipWidth = -1;
	clipHeight = -1;
	lines.Clear();
}

void EW_PostScriptTablet::SetClipBounds(const GPL_Integer x,
										const GPL_Integer y,
										const GPL_Integer width,
										const GPL_Integer height)
{
	// If the clipping bounds have changed...
	if (x != clipX || y != clipY || width != clipWidth || height != clipHeight)
	{

		// Save the clipping bounds.
		clipX = x;
		clipY = y;
		clipWidth = width;
		clipHeight = height;

		GPL_String image;
		image.Printf("%% %d %d %d %d SetClipBounds", clipX, clipY, clipWidth, clipHeight);
		lines.Append(image);
	}
}

void EW_PostScriptTablet::RectWire(const GPL_Integer x,
									const GPL_Integer y,
									const GPL_Integer width,
									const GPL_Integer height)
{
	GPL_String image;
	image.Printf("%d %d %d %d RectWire", x, y, width, height);
	lines.Append(image);
}

void EW_PostScriptTablet::RectFill(const GPL_Integer x,
									const GPL_Integer y,
									const GPL_Integer width,
									const GPL_Integer height)
{
	// NOTE: colorIndex is currently ignored.
	GPL_String image;
	image.Printf("%d %d %d %d 1 RectFill", x, y, width, height);
	lines.Append(image);
}

void EW_PostScriptTablet::Text(const GPL_Integer x, const GPL_Integer y,
								const GPL_String & string)
{
	GPL_String image;
	image.Printf("(%s) %d %d Text", string.GetData(), x, y);
	lines.Append(image);
}

GPL_Boolean EW_PostScriptTablet::Save (const GPL_String & fileName)
{
	GPL_File file;
	if (!file.Open (fileName, OSD_FILE_WRITE | OSD_FILE_ERASE))
	{
		EW_PRINT(EW_OS, EW_ERROR, "Unable to open file \"%s\"",
					fileName.GetData());
		return FALSE;
	}

	if (!SaveStrings(file, prefix))
	{
		return FALSE;
	}

	if (!SaveStrings(file, lines))
	{
		return FALSE;
	}

	if (!SaveStrings(file, postfix))
	{
		return FALSE;
	}

	if (!file.Close ())
	{
		EW_PRINT(EW_OS, EW_WARN, "Unable to close file \"%s\"",
					fileName.GetData());
		return FALSE;
	}

	return TRUE;
}

GPL_Boolean EW_PostScriptTablet::SaveStrings (GPL_File & file,
									const GPL_List<GPL_String> & strings)
{
	for (unsigned long i = 0; strings.GetCount () > i; ++i)
	{
		if (!file.Write(strings[i].GetData(), strings[i].GetLength()))
		{
			EW_PRINT(EW_OS, EW_ERROR, "Unable to write to file \"%s\"",
						file.GetName());
			return FALSE;
		}
		if (!file.Write("\n", 1))
		{
			EW_PRINT(EW_OS, EW_ERROR, "Unable to write to file \"%s\"",
						file.GetName());
			return FALSE;
		}
	}
	return TRUE;
}
