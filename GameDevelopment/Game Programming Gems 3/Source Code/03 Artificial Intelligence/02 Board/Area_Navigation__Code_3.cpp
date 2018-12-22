/* Copyright (C) Dogfish Entertainment, 2002.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dogfish Entertainment, 2002"
 */

// The size of the map
#define MAP_WIDTH_CELLS		(128)
#define MAP_HEIGHT_CELLS	(MAP_WIDTH_CELLS)

/*
	This is the class for the map, containing all the information about the world's cells and their types
*/
class CMap
{
	public:
		RectangulariseWorld ( MapCellTypes type, int maxAreaSize );
		AddMapArea (CMapArea *area ) { areas.push_back(area); }

	private:
		CMapCell mCells[MAP_WIDTH_CELLS][MAP_HEIGHT_CELLS];
		list<CMapAreas> areas;
};

enum MapCellTypes
{
	MCF_PATH				= 0,
	MCF_GRASS,
	MCF_NUMBER_OF_CELL_TYPES,
};

/*
	This is the class for the individual map cells, containing their cells type
*/
class CMapCell
{
	public:
		MapCellTypes	GetCellType( void )					{ return mType; }
		void			SetCellType( MapCellTypes type )	{ mType = type; }
		bool			GetAreaFree( void )					{ return mAreaFree; }
		void			SetAreaFree( bool free )			{ mAreaFree = free; }

	private:
		MapCellTypes	mType;
		bool			mAreaFree;
		
};

/*
	This is the class for areas of map cells, defined by a bounding rectangle
*/
class CMapArea
{
	public:
		SetBoundaries( int south, int west, int north, int east ) { mNorth = north; mSouth = south; mEast = east, mWest = west; }
		SetType( MapCellTypes type ) { mType = type; }

	private:
		int				mNorth,	mSouth, mEast, mWest;
		MapCellTypes	mType;
};

/*
	This function takes the map and produces a set of areas of the parameter type
	The larger the initial square used to test the world, defined by maxAreaSize, the slower
	the algorithm but the better the rectangularisation. maxAreaSize = 1 is the fastest value
	but it is likely to produce many long thin rectangles. maxAreaSize = MAX_WIDTH_CELLS is
	the slowest value, but it will produce the optimal path traversal rectangle set.
*/
void CMap::RectangulariseWorld( MapCellTypes type, int maxAreaSize )
{
	int cellCount = 0;	// Used for counting the number of cells of a particular type
	int x,y;			// Used as counters for x and y rectangle start positions on the 2D grid
	int checkX, checkY;	// Used for checking cells inside the test rectangle

	// Count up the total number of map cells of the type we're about to rectangularise
	for(x=0;x<MAP_WIDTH_CELLS;x++)
	{
		for(y=0;y<MAP_HEIGHT_CELLS;y++)
		{
			// If this is the right type and it is not already used in an area
			if(mCells[x][y].GetCellType()==type && mCells[x][y].GetAreaFree()==true)
			{
				cellCount++;
			}
		}
	}

	// If there are no cells of this type in the world, then early out
	if(cellCount==0)
	{
		return;
	}
	
	// If the cells were in one large square how big would it be, never look for size larger than this
	if(cellCount > 2 && cellCount - 1 < maxAreaSize * maxAreaSize)
	{
		maxAreaSize = (int) sqrt(cellCount - 1);
	}

	// For all the possible sizes of squares, starting with the largest and working down
    for(int size = maxAreaSize;size>0;size--)
    {
		// This is a fail test, when this is set to true, a rectangle does not contain all free cells of the required type
		bool fail;
        // For the whole map, ensuring the rectangle stays on the map
        for(x=0;x+size <= MAP_WIDTH_CELLS;x++)
        {
			for(y=0;y+size <= MAP_HEIGHT_CELLS;y++)
            {
				// Initialise the fail test
				fail = false;
				// For the entire rectangle being checked
                for(checkX = x;checkX<x+size;checkX++)
                {
					for(checkY = y;checkY<y+size;checkY++)
                    {
						// Grab the cell
						CMapCell *cell = &mCells[checkX][checkY];
                        // If the cell is not of the required type or it is in use by another bounded area
                        if(cell->GetCellType()!=type || cell->GetAreaFree()==false)
						{
						    // The fail test is set
	                        fail = true;
							// Break out of the inner for statements
							break;
						}
					}
					// If we've failed
					if(fail==true)
					{
						// Do no continue with this rectangle
						break;
	                }
                }
	            // If we managed to make it out without finding a fail
				if(fail==false)
				{
					// We're now going to try to expand the succesful rectangle
					// These are used to define the rectangle of the new untested area produced by 
					// the expansion of the rectangle
					int tx1, tx2, ty1, ty2;
					// These are the guaranteed tested area boundaries, the size of the original rectangle
					int mx1 = x,my1 = y,mx2 = x+size, my2 = y+size;
					// This is a success test
					bool success;
					int failCount = 0;
					// These are the current test area boundaries
					int cx1, cy1, cx2, cy2;
					// Until we've failed to expand in all four directions
					while(failCount<4)
					{    
						// For each direction of expansion
						for(int i=0;i<4;i++)
						{
							// This is working so far
							success = true;
							// Set up the test values to the actual values
							cx1 = mx1;cy1 = my1;cx2 = mx2; cy2 = my2;
							// Expand in one direction
							switch(i)
							{
								case 0:
									// Expand to the west
									cx1--;
									// Calculate the extra strip produced by this expansion
									tx1 = cx1; tx2 = cx1 + 1; ty1 = cy1; ty2 = cy2;
									break;
								case 1:
									// Expand to the south
									cy1--;
									// Calculate the extra strip produced by this expansion
									tx1 = cx1; tx2 = cx2; ty1 = cy1; ty2 = cy1 + 1;
									break;
								case 2:
									// Expand to the east
									cx2++;
									// Calculate the extra strip produced by this expansion
									tx1 = cx2 - 1; tx2 = cx2; ty1 = cy1; ty2 = cy2;
									break;
								case 3:
									// Expand to the north
									cy2++;
									// Calculate the extra strip produced by this expansion
									tx1 = cx1; tx2 = cx2; ty1 = cy2 - 1; ty2 = cy2;
									break;
							}
							// If we've expanded off the map
							if(cx1 < 0 || cy1 < 0 || cx2 > MAP_WIDTH_CELLS || cy2 > MAP_HEIGHT_CELLS)
							{
								success = false;
							}
							else
							{
								// For the rectangle defined by the new strip from the expansion of the main rectangle
								for(checkX = cx1;checkX<cx2;checkX++)
								{
									for(checkY = cy1;checkY<cy2;checkY++)
									{
										// Grab the cell
										CMapCell *cell = &mCells[checkX][checkY];
				                        // If the cell is not of the required type or it is in use by another bounded area
										if(cell->GetCellType()!=type || cell->GetAreaFree()==false){
											// The expansion has failed
											success = false;
											// Break out of the inner for statements
											break;
										}
									}
									// If we've failed
									if(success==false)
									{
										// Do not continue with checking the strip
										break;
									}
								}

							}
							// If we've failed
							if(success==false)
							{
								// Keep a count
								failCount++;
							}
							else
							{
								// We have succeeded, let's try another 4 expansions
								failCount = 0;
								// The actual values are set equal to the succesful test values
								mx1 = cx1; my1 = cy1; mx2 = cx2; my2 = cy2;
							}
						}
					}
					// Go through every cell inside this rectangle
					for(checkX=mx1;checkX<mx2;checkX++)
					{
						for(checkY=my1;checkY<my2;checkY++)
						{
							// Set the cell to "taken"
							mCells[checkX][checkY].SetAreaFree(false);
						}
					}
					// The number of cells left free of this type is decreased
					cellCount -= (mx2 - mx1) * (my2 - my1);
					// Map a new area
					CMapArea *area = new CMapArea();
					// Set areas's boundaries s,w,n,e
					area->SetBoundaries(mx1,my1,mx2,my2);
					// Set its type
					area->SetType(type);
					// Add it to the map's area list
					AddMapArea(area);
					// If we've no cells left of this type
					if(cellCount==0)
					{
						// Set up values so we quit out
						finish = true;
					}
					// Ensure that the number of cells hasn't fallen below zero
					ASSERT(cellCount >= 0);
				}
			}
		}
	}
}
