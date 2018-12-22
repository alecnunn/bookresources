/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "priv_precompiled.h"

#include "resourcemanager.h"
#include "imagelibrary/csbitmap.h"

#include <map>
#include <string>
#include <sstream>
using namespace std;

typedef map<string, CSBitmap*> BitmapList;

class BitmapManager
{
public:
	~BitmapManager()
	{
		for (BitmapList::iterator p = m_bitmapList.begin(); p != m_bitmapList.end(); ++p)
		{
			delete p->second;
		}
	}
	const CSBitmap* LoadBitmapImage(const char* filename, bool allowFailure /*= false*/)
	{
		const char* imageDirectories[] =
		{
			"bitmaps\\",
			"..\\bitmaps\\",
		};
		CSBitmap* pBitmap = new CSBitmap;
		for (int i = 0; i < NUMELEMENTS(imageDirectories); ++i)
		{
			string pathName = string(imageDirectories[i]) + string(filename);
			if (LoadImageFile(pathName.c_str(), pBitmap))
				break;
		}
		// Even if we fail to load the bitmap we need to add it to the
		// map - otherwise we'll keep retrying every time.
		m_bitmapList[filename] = pBitmap;
		if (!pBitmap->HasBitmap())	// If the loads failed.
		{
			stringstream errorMessage;
			errorMessage << "Failed to load '" << filename << "' from ";
			for (int i = 0; i < NUMELEMENTS(imageDirectories); ++i)
			{
				errorMessage << "'" << imageDirectories[i] << "'";
				if (i < NUMELEMENTS(imageDirectories) - 2)
				{
					errorMessage << ", ";
				}
				else if (i == NUMELEMENTS(imageDirectories) - 2)
				{
					errorMessage << " and ";
				}
			}
			dprintf("%s.\n", errorMessage.str().c_str());
			if (allowFailure)
			{
				delete pBitmap;
				pBitmap = 0;
			}
			else
			{
				MarkBitmapAsBlank(pBitmap);
			}
		}
		return pBitmap;
	}

private:
	void MarkBitmapAsBlank(CSBitmap* pBitmap)
	{
		pBitmap->SetSize(32, 32, 1);
		const uint8_t dark = 90;
		const uint8_t light = 150;
		RGBQUAD palette[2] =
		{
			{dark, dark, dark},
			{light, light, light},
		};
		pBitmap->SetPalette(0, NUMELEMENTS(palette), palette);
		for (int y = 0; y < pBitmap->GetHeight(); ++y)
		{
			uint8_t* pLine = pBitmap->GetLinePtr(y);
			for (int x = 0; x < pBitmap->GetWidth(); ++x)
			{
				pLine[x] = ((x ^ y) >> 3) & 1;
			}
		}
	}
	BitmapList m_bitmapList;
};

const CSBitmap* LoadBitmapImage(const char* filename, bool allowFailure /*= false*/)
{
	static BitmapManager s_bitmapManager;
	return s_bitmapManager.LoadBitmapImage(filename, allowFailure);
}
