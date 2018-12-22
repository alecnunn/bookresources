/* Copyright (C) Guy W. Lecky-Thompson, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Guy W. Lecky-Thompson, 2000"
 */
// Universe Generation and Display Engine
// (c) 2000 Guy W. Lecky-Thompson

// Standard C libraries to use
#include <stdlib.h>
#include <string.h>

// Windows-specific libraries
#include <windows.h>

// Engine-specific includes
#include "unigen.h"

void GenerateRandomUniverse(int universe[UNIVERSE_SIDE][UNIVERSE_SIDE],
									 long universe_serial_number)
{
	int x, y;
	long grid_reference;
	int random_value;

	for (y = 0; y < UNIVERSE_SIDE; y++)
	{
		for (x = 0; x < UNIVERSE_SIDE; x++)
		{
			// Calculate the grid reference for this point
			grid_reference = x + (y * UNIVERSE_SIDE);
			// Seed the random number generator
			srand((unsigned)universe_serial_number + (unsigned)grid_reference);
			// Choose a value for this point
			random_value = rand() % 100;
			if (random_value > STAR_THRESHOLD)
			{
				universe[x][y] = 1;
			}
			else
			{
				universe[x][y] = 0;
			}
		}
	}
}

int FeatureAt(long universe_serial_number, int grid_x, int grid_y)
{
	int random_value, position_counter;
	srand((unsigned)universe_serial_number + (unsigned)grid_y);
	position_counter = grid_x;
	while (position_counter > -1)
	{
		random_value = rand() % 100;
		position_counter--;
	} 
	if (random_value > STAR_THRESHOLD) return 1;
	return 0;
}

void DisplayUniverse(long universe_serial_number,
							RECT * rcTarget,
							HDC hdcTarget)
{
	HPEN hBlackPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN hWhitePen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0,0,0));
	HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255,255,255));
	HPEN hOldPen;
	HFONT hOldFont;
	HBRUSH hOldBrush;
	char name[10];
	int name_length;
	RECT rcNameRect;
   TEXTMETRIC tm;

	int nSquareXSide = (rcTarget->right  - rcTarget->left) / UNIVERSE_SIDE;
	int nSquareYSide = (rcTarget->bottom - rcTarget->top)  / UNIVERSE_SIDE;

	int x,y;
	HFONT hfnt;
	LOGFONT lf;

	SetTextColor(hdcTarget,RGB(255,255,255));
	SetBkColor(hdcTarget,RGB(0,0,0));

	hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(hfnt, sizeof(LOGFONT), &lf);
	lf.lfWeight = FW_THIN;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	lf.lfPitchAndFamily = VARIABLE_PITCH|FF_SWISS;
	lf.lfHeight = -8;
	lf.lfWidth = 0;
	hfnt = CreateFontIndirect(&lf);
	hOldFont = (HFONT)SelectObject(hdcTarget,hfnt);

	hOldBrush = (HBRUSH)SelectObject(hdcTarget,hBlackBrush);
	hOldPen = (HPEN)SelectObject(hdcTarget,hBlackPen);
	Rectangle(hdcTarget,rcTarget->left,rcTarget->top,
							  rcTarget->left + (nSquareXSide * UNIVERSE_SIDE),
							  rcTarget->top  + (nSquareYSide * UNIVERSE_SIDE));

	for (y = 0; y < UNIVERSE_SIDE; y++)
	{
		for (x = 0; x < UNIVERSE_SIDE; x++)
		{
			if (FeatureAt(universe_serial_number,x,y) == 1)
			{
				hOldPen = (HPEN)SelectObject(hdcTarget,hWhitePen);
				hOldBrush = (HBRUSH)SelectObject(hdcTarget,hWhiteBrush);
				Ellipse(hdcTarget,x * nSquareXSide, y * nSquareYSide,
									  (x * nSquareXSide) + 2,
									  (y * nSquareYSide) + 2);
				name_length = GenerateName(universe_serial_number,x,y,name);
				rcNameRect.left = (x * nSquareXSide) + 3;
				rcNameRect.top  = (y * nSquareYSide) + 3;
				// Use the font size to figure if we're too close to the edge or over it
				GetTextMetrics(hdcTarget,&tm);
				if (rcNameRect.left + (tm.tmMaxCharWidth * name_length) > rcTarget->right)
				{
					rcNameRect.left = rcNameRect.left -
						((rcNameRect.left + (tm.tmMaxCharWidth * name_length)) -
						  rcTarget->right);
				}
				TextOut(hdcTarget,rcNameRect.left,rcNameRect.top,
								name,name_length);
				SelectObject(hdcTarget,hOldPen);
            SelectObject(hdcTarget,hOldBrush);
			}
		}
	}

	// Clean up to prevent memory leaks
	SelectObject(hdcTarget,hOldFont);
	SelectObject(hdcTarget,hOldBrush);
	SelectObject(hdcTarget,hOldPen);

	DeleteObject(hfnt);
	DeleteObject(hBlackPen);
	DeleteObject(hWhitePen);
	DeleteObject(hBlackBrush);
	DeleteObject(hWhiteBrush);
}

int GenerateName(long universe_serial_number,
						int x, int y,
						char name[10])
{
	int word_length;
	int char_pos;
	srand((unsigned)universe_serial_number + (unsigned)(UNIVERSE_SIDE * y) + (unsigned)x);
	word_length = (rand() % 6) + 4;
	for (char_pos = 0; char_pos <= word_length; char_pos++)
	{
		name[char_pos] = 'A' + (rand() % 26);
		name[char_pos+1] = '\0';
	}
	return word_length;
}

void GetNumberOfPlanets(long universe_serial_number,
						int x, int y,
						char number_of_planets[12])
{
	srand((unsigned)universe_serial_number + (unsigned)(UNIVERSE_SIDE * y) + (unsigned)x);
	wsprintf(number_of_planets,"Planets : %2d",rand() % 12);
}

void DisplayFeatureInformation(long universe_serial_number,
										 RECT * rcMapWindow,
										 int mouse_x, int mouse_y,
										 RECT * rcMapInfo,HDC hdcTarget)
{
	int map_x, map_y,name_length;
	int nSquareXSide = (rcMapWindow->right  - rcMapWindow->left) / UNIVERSE_SIDE;
	int nSquareYSide = (rcMapWindow->bottom - rcMapWindow->top)  / UNIVERSE_SIDE;
	char name[10];
	char number_of_planets[12];
	HFONT hfnt, hOldFont;
	LOGFONT lf;
	RECT rcTextRect;
	HPEN hGrayPen, hOldPen;
	HBRUSH hGrayBrush, hOldBrush;

	hGrayPen = CreatePen(PS_SOLID,1,PALETTERGB(200,200,200));
	hGrayBrush = CreateSolidBrush(PALETTERGB(200,200,200));
	hOldPen = (HPEN)SelectObject(hdcTarget,hGrayPen);
	hOldBrush = (HBRUSH)SelectObject(hdcTarget,hGrayBrush);

	hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(hfnt, sizeof(LOGFONT), &lf);

	map_x = mouse_x / nSquareXSide;
	map_y = mouse_y / nSquareYSide;

	if (FeatureAt(universe_serial_number,map_x,map_y) == 1)
	{
		name_length = GenerateName(universe_serial_number,map_x,map_y,name);

		lf.lfWeight = FW_THIN;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		lf.lfPitchAndFamily = VARIABLE_PITCH|FF_SWISS;
		SetTextColor(hdcTarget,RGB(0,0,0));
		SetBkColor(hdcTarget,PALETTERGB(200,200,200));

		rcTextRect.left = rcMapInfo->left + 1;
		rcTextRect.right = rcMapInfo->right - 1;

		rcTextRect.top    = rcMapInfo->top + 1;
		rcTextRect.bottom = ((rcMapInfo->bottom - rcMapInfo->top) -2) / 4;

		Rectangle(hdcTarget,rcTextRect.left,rcTextRect.top,
								  rcTextRect.right,rcTextRect.bottom);

		lf.lfHeight = (rcTextRect.bottom - rcTextRect.top);
		lf.lfWidth = 0;
		hfnt = CreateFontIndirect(&lf);
		hOldFont = (HFONT)SelectObject(hdcTarget,hfnt);

		DrawText(hdcTarget,name,name_length,&rcTextRect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

		SelectObject(hdcTarget,hOldFont);
		DeleteObject(hfnt);

		// Show the number of planets
		GetNumberOfPlanets(universe_serial_number,map_x,map_y,number_of_planets);
		rcTextRect.top = rcTextRect.bottom;
		rcTextRect.bottom = rcTextRect.bottom + (((rcMapInfo->bottom - rcMapInfo->top) -2) / 4);

  		Rectangle(hdcTarget,rcTextRect.left,rcTextRect.top,
								  rcTextRect.right,rcTextRect.bottom);


		lf.lfHeight = (rcTextRect.bottom - rcTextRect.top);
		lf.lfWidth = 0;
		hfnt = CreateFontIndirect(&lf);
		hOldFont = (HFONT)SelectObject(hdcTarget,hfnt);

		DrawText(hdcTarget,number_of_planets,12,&rcTextRect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

		SelectObject(hdcTarget,hOldFont);
		DeleteObject(hfnt);
	}

	SelectObject(hdcTarget,hOldFont);
	SelectObject(hdcTarget,hOldBrush);
	SelectObject(hdcTarget,hOldPen);

	DeleteObject(hfnt);
	DeleteObject(hGrayPen);
	DeleteObject(hGrayBrush);

}

void BlankRect(HDC hdcTarget, RECT * rcTarget, COLORREF fg, COLORREF bk)
{
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hPen = CreatePen(PS_SOLID,1,fg);
	hBrush = CreateSolidBrush(bk);
	hOldPen = (HPEN)SelectObject(hdcTarget,hPen);
	hOldBrush = (HBRUSH)SelectObject(hdcTarget,hBrush);

	Rectangle(hdcTarget,
		rcTarget->left,rcTarget->top,rcTarget->right,rcTarget->bottom);

	SelectObject(hdcTarget,hOldBrush);
	SelectObject(hdcTarget,hOldPen);

	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void ShowText(HDC hdcTarget,RECT * rcTextRect,char * text,int text_length,
					COLORREF fg, COLORREF bk)
{
	HFONT hfnt, hOldFont;
	LOGFONT lf;

	hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
	GetObject(hfnt, sizeof(LOGFONT), &lf);

	lf.lfWeight = FW_THIN;
	lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	lf.lfPitchAndFamily = VARIABLE_PITCH|FF_SWISS;

	lf.lfHeight = (rcTextRect->bottom - rcTextRect->top);
	lf.lfWidth = 0;

	SetTextColor(hdcTarget,fg);
	SetBkColor(hdcTarget,bk);

	hfnt = CreateFontIndirect(&lf);
	hOldFont = (HFONT)SelectObject(hdcTarget,hfnt);

	DrawText(hdcTarget,text,text_length,rcTextRect,
				DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	SelectObject(hdcTarget,hOldFont);
	DeleteObject(hfnt);
}

void	DisplayStar(long universe_serial_number,
						int map_x,int map_y,int number_of_planets,
						HDC hdcTarget,RECT * rcStarRect)
{
	// Choose color
	srand((unsigned)universe_serial_number + (unsigned)(UNIVERSE_SIDE * map_y) + (unsigned)map_x);
	int nColor = rand() % 3;
	COLORREF crStar;
	int size;

	switch (nColor)
	{
		case 0 : crStar = PALETTERGB(255,0,0); break;
		case 1 : crStar = PALETTERGB(255,255,0); break;
		case 2 : crStar = PALETTERGB(255,255,255); break;
	}
   number_of_planets = number_of_planets + 2;
	// The size depends on the number of planets
	size = (((100 * number_of_planets) / 14) * (rcStarRect->right - rcStarRect->left)) / 100;

	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hPen = CreatePen(PS_SOLID,1,crStar);
	hBrush = CreateSolidBrush(crStar);

	hOldPen = (HPEN)SelectObject(hdcTarget,hPen);
	hOldBrush = (HBRUSH)SelectObject(hdcTarget,hBrush);

	Ellipse(hdcTarget,
	rcStarRect->left + (((rcStarRect->right - rcStarRect->left) / 2) - (size / 2)),
	rcStarRect->top + (((rcStarRect->bottom - rcStarRect->top) / 2) - (size / 2)),
	rcStarRect->left + (((rcStarRect->right - rcStarRect->left) / 2) + (size / 2)),
	rcStarRect->top + (((rcStarRect->bottom - rcStarRect->top) / 2) + (size / 2)));

	SelectObject(hdcTarget,hOldPen);
	SelectObject(hdcTarget,hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

}

void	DisplayPlanet(long universe_serial_number,
						int map_x,int map_y,int planet_number,
						HDC hdcTarget,RECT * rcStarRect)
{
	// Choose color
	srand((unsigned)universe_serial_number + (unsigned)(UNIVERSE_SIDE * map_y) + (unsigned)map_x + (unsigned)planet_number);
	int nColor = rand() % 4;
	COLORREF crPlanet;
	int size;

	switch (nColor)
	{
		case 0 : crPlanet = PALETTERGB(0,0,255); break;
		case 1 : crPlanet = PALETTERGB(0,255,0); break;
		case 2 : crPlanet = PALETTERGB(128,128,128); break;
		case 3 : crPlanet = PALETTERGB(0,128,128); break;
	}
	size = (rand() % 80) + 20;
	size = (int)((size / 100.0) * (rcStarRect->bottom - rcStarRect->top));

	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	hPen = CreatePen(PS_SOLID,1,crPlanet);
	hBrush = CreateSolidBrush(crPlanet);

	hOldPen = (HPEN)SelectObject(hdcTarget,hPen);
	hOldBrush = (HBRUSH)SelectObject(hdcTarget,hBrush);

	Ellipse(hdcTarget,
	rcStarRect->left + (((rcStarRect->right - rcStarRect->left) / 2) - (size / 2)),
	rcStarRect->top + (((rcStarRect->bottom - rcStarRect->top) / 2) - (size / 2)),
	rcStarRect->left + (((rcStarRect->right - rcStarRect->left) / 2) + (size / 2)),
	rcStarRect->top + (((rcStarRect->bottom - rcStarRect->top) / 2) + (size / 2)));

	SelectObject(hdcTarget,hOldPen);
	SelectObject(hdcTarget,hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

}


void	DisplayPlanets(long universe_serial_number,
						int map_x,int map_y,int number_of_planets,
						HDC hdcTarget,RECT * rcStarRect)
{
	int nPlanet;
	RECT rcPlanet;

	rcPlanet.top = rcStarRect->top;
	rcPlanet.left = rcStarRect->left;
	rcPlanet.bottom = rcStarRect->bottom;
	rcPlanet.right = rcStarRect->right;
	for (nPlanet = 0; nPlanet < number_of_planets; nPlanet++)
	{
		rcPlanet.top = rcStarRect->top + ((rcStarRect->bottom - rcStarRect->top) * nPlanet);
		rcPlanet.bottom = rcPlanet.top + (rcStarRect->bottom - rcStarRect->top);
		rcPlanet.left = (rcStarRect->right - rcStarRect->left) / 2;
		rcPlanet.left = rcPlanet.left - ((rcPlanet.bottom - rcPlanet.top) / 2);
		rcPlanet.left = rcPlanet.left + rcStarRect->left;
		rcPlanet.right = rcPlanet.left + (rcPlanet.bottom - rcPlanet.top);
		DisplayPlanet(universe_serial_number,map_x,map_y,nPlanet,hdcTarget,&rcPlanet);
	}
}

void	DisplaySolarSystem(long universe_serial_number,RECT * rcMapWindow,
												int mouse_x, int mouse_y,
											  RECT * rcSolarSystemWindow,
											  HDC hdcTarget)
{
	int nSquareXSide = (rcMapWindow->right  - rcMapWindow->left) / UNIVERSE_SIDE;
	int nSquareYSide = (rcMapWindow->bottom - rcMapWindow->top)  / UNIVERSE_SIDE;
	char name[10];
	char coords[20];
	int coords_length;
	char number_of_planets[12];
	int map_x, map_y, name_length;
	RECT rcNameRect, rcSolarSystemRect, rcStarRect;

	map_x = mouse_x / nSquareXSide;
	map_y = mouse_y / nSquareYSide;

	// Name, co-ordinates
	BlankRect(hdcTarget,rcSolarSystemWindow,RGB(0,0,0),RGB(0,0,0));

	if (FeatureAt(universe_serial_number,map_x,map_y) == 1)
	{
		name_length = GenerateName(universe_serial_number,map_x,map_y,name);
		rcNameRect.left = rcSolarSystemWindow->left + 1;
		rcNameRect.right = rcSolarSystemWindow->right - 1;
		rcNameRect.top = rcSolarSystemWindow->top;
		rcNameRect.bottom =
				  ((rcSolarSystemWindow->bottom - rcSolarSystemWindow->top)) / 16;
		rcNameRect.bottom = rcNameRect.bottom + rcNameRect.top;
		ShowText(hdcTarget,&rcNameRect,name,name_length,RGB(0,0,0),RGB(255,255,255));
		rcNameRect.top = rcNameRect.bottom;
		rcNameRect.bottom = rcNameRect.top +
				  ((rcSolarSystemWindow->bottom - rcSolarSystemWindow->top)) / 24;
		coords_length = wsprintf(coords,"%d,%d",map_x,map_y);
		ShowText(hdcTarget,&rcNameRect,coords,coords_length,RGB(0,0,0),RGB(255,255,255));
		GetNumberOfPlanets(universe_serial_number,map_x,map_y,number_of_planets);
		rcSolarSystemRect.top = rcNameRect.bottom;
		rcSolarSystemRect.left = rcNameRect.left;
		rcSolarSystemRect.right = rcNameRect.right;
		rcSolarSystemRect.bottom = rcSolarSystemWindow->bottom;
		rcStarRect.top = rcSolarSystemRect.top;
		rcStarRect.bottom = rcSolarSystemRect.top + ((rcSolarSystemRect.bottom - rcSolarSystemRect.top) / 5);
		rcStarRect.left = (rcSolarSystemRect.right - rcSolarSystemRect.left) / 2;
		rcStarRect.left = rcStarRect.left - ((rcStarRect.bottom - rcStarRect.top) / 2);
		rcStarRect.left = rcStarRect.left + rcSolarSystemRect.left;
		rcStarRect.right = rcStarRect.left + (rcStarRect.bottom - rcStarRect.top);
		DisplayStar(universe_serial_number,map_x,map_y,atoi(number_of_planets+9),hdcTarget,&rcStarRect);
		rcSolarSystemRect.top = rcStarRect.bottom;
		rcSolarSystemRect.bottom = (rcSolarSystemRect.bottom - rcSolarSystemRect.top) / 12;
		rcSolarSystemRect.bottom = rcSolarSystemRect.bottom + rcSolarSystemRect.top;
		DisplayPlanets(universe_serial_number,map_x,map_y,atoi(number_of_planets+9),hdcTarget,&rcSolarSystemRect);
	}
}
