/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved
*/
#include "Terrain.hpp"

// Every terrain has a unique ID used for debugging
unsigned short	Terrain::CurID = 0;


// Set the ID to the next available ID
Terrain::Terrain(void)
{
	ID = CurID++;
}



// Function to create a scanline buffer (X0, X1) for every Y
// which creates the frustrum's bounding polygon
void Terrain::ScanLine(Vector2D<float> *Pts, unsigned short Max)
{
	Vector2D<float>	*Left, *Right;							// Next Left and Right vertex of our bounding polygon
	float			 OldLeftX, OldRightX, LeftX, RightX;	// X value for the last and current left & right edge row
	float			 LeftSlope, RightSlope;					// Slopes for the left & right edges
	Span			*At;									// Strip array we're constructing
	short			 Y;										// Y Position in world space


	// Prepare our scanline of the first vertex
	Left	= Pts;
	Right	= &Pts[Max];
	LeftX	= Left->X;
	RightX	= Right->X;
	Y		= StartY = (short)floor(Pts->Y);
	At		= Strips;

	// Do the first (Top-Left quadrant)
	do {
		ASSERT(Y < MAX_SPANY);
		Y++;

		// Update the Left position
		OldLeftX = LeftX;
		LeftX	+= LeftSlope;
		// Are we hiting a new vertex to the left?
		while ((Y >= Left->Y) && ((Left + 1)->Y >= Left->Y)) {
			// Add the the vertex itself if it beats the old one
			if (OldLeftX > Left->X)
				OldLeftX = Left->X;

			// Recalculate the slope
			if ((Left + 1)->Y != Left->Y) {
				LeftSlope	 = ((Left + 1)->X - Left->X) / ((Left + 1)->Y - Left->Y);
				LeftX		 = Left->X + (Y - Left->Y) * LeftSlope;
			}

			Left++;
		}

		// Update the Right position
		OldRightX = RightX;
		RightX	 += RightSlope;
		// Are we hiting a new vertex to the right?
		while ((Y >= Right->Y) && (Left <= Right)) {
			// Add the the vertex itself if it beats the old one
			if (OldRightX < Right->X)
				OldRightX = Right->X;

			// Recalculate the slope
			if ((Right - 1)->Y != Right->Y) {
				RightSlope	 = ((Right - 1)->X - Right->X) / ((Right - 1)->Y - Right->Y);
				RightX		 = Right->X + (Y - Right->Y) * RightSlope;
			}

			Right--;
		}

		// Store the span
		At->Start	= (short)floor(min(LeftX, OldLeftX));
		At->End		= (short)ceil(max(RightX, OldRightX));
		At++;
	} while (Left <= Right);


	// Do this to simplify the code later on
	At->Start	= (short)floor(OldLeftX);
	At->End		= (short)ceil(OldRightX);
	At--;
	// Process our last line
	Y--;
	StopY		= Y;
	At->Start	= (short)floor(OldLeftX);
	At->End		= (short)ceil(OldRightX);
}



// Clips the Furstrum's corner to our 2 Y planes
bool Terrain::Clip3D(Vector3D<float> *Pts, unsigned short &Max)
{
	// List of paired vertex yielding segments
	unsigned short  Matches[12][2] = {{0, 1}, {2, 3}, {4, 5}, {6, 7},
									 	{0, 2}, {1, 3}, {4, 6}, {5, 7},
										{0, 4}, {1, 5}, {2, 6}, {3, 7}};
	Vector3D<float> Pt[8];			// Temporary copy of Pts for source
	unsigned short  Code[8];		// Code calculated per vertex
	Vector3D<float> *At, *Src;		// Current destination vertex and source vertex array
	unsigned short *CAt;			// Current position for the codes
	unsigned short i;

	// Make a backup of the vertex values
	memcpy(Pt, Pts, 8 * sizeof(Vector3D<float>));

	// First, add all the vertices within the 2 planes
	Max = 0;
	Src = Pt;
	At  = Pts;
	CAt = Code;
	for (i = 8 ; i ; i--) {
		// If it's within the 2 planes, add it
		if ((Src->Y >= BottomY) && (Src->Y <= TopY)) {
			*CAt  = 0;
			*At++ = *Src;
			Max++;

		// If not, simply assign it's clipping code
		} else if (Src->Y > TopY)
			*CAt = 1;
		else
			*CAt = 2;

		Src++;
		CAt++;
	}

	// If nothing is clipped, return
	if (Max == 8)
		return false;


	// Now add all the intersection vertices
	for (i = 0 ; i < 12; i++) {
		unsigned short Code1 = Matches[i][0];
		unsigned short Code2 = Matches[i][1];
		unsigned short Codes = Code[Code1] ^ Code[Code2];

		if (Codes) {
			float t;

			// Code & 1 means we're clipping at the Top Y plane
			if (Codes & 1) {
				t     = (TopY - Pt[Code1].Y) / (Pt[Code1].Y - Pt[Code2].Y);
				At->X = Pt[Code1].X + t * (Pt[Code1].X - Pt[Code2].X);
				At->Z = Pt[Code1].Z + t * (Pt[Code1].Z - Pt[Code2].Z);
				At->Y = BottomY;
				At++;
				Max++;
			}

			// Code & 2 means we're clipping at the Bottom Y plane
			if (Codes & 2) {
				t     = (BottomY - Pt[Code1].Y) / (Pt[Code1].Y - Pt[Code2].Y);
				At->X = Pt[Code1].X + t * (Pt[Code1].X - Pt[Code2].X);
				At->Z = Pt[Code1].Z + t * (Pt[Code1].Z - Pt[Code2].Z);
				At->Y = TopY;
				At++;
				Max++;
			}
		}
	}

	return !Max;
}



// Clips the bounding polygon to a 2D rectangle
bool Terrain::Clip2D(Vector2D<float> *Pts, unsigned short &Max)
{
	float			 CornerX, CornerY;				// Corner we're clipping with (if any)
	unsigned short	 Code[16], Code2[16], i;		// Code array per vertex
	unsigned short	*CAt = Code;					// Current position in the code array
	unsigned short	 NoClipped = 0;					// Number of vertices clipped
	Vector2D<float>	*At = Pts;						// Current position in the vertex array
	float			 LeftX, RightX, TopY, BottomY;	// Bounding limit in frustrum space

	// Calculate our limits relative to our frustrum
	LeftX   = this->LeftX  + (Center.X / VERTEX_PER_SAMPLE);
	RightX  = this->RightX + (Center.X / VERTEX_PER_SAMPLE);
	TopY    = this->FrontY + (Center.Y / VERTEX_PER_SAMPLE);
	BottomY = this->BackY  + (Center.Y / VERTEX_PER_SAMPLE);

	// Find the reference Corner (with its inside/outside Code)
	for (i = Max + 1 ; i ; i--, CAt++, At++) {
		*CAt = 0;

		// Are we clipped to the left?
		if (At->X <= LeftX) {
			CornerX = LeftX;
			*CAt    = 1;
			NoClipped++;

		// Are we clipped to the right?
		} else if (At->X >= RightX) {
			CornerX = RightX;
			*CAt	= 1;
			NoClipped++;
		}

		// Are we clipped to the top?
		if (At->Y <= TopY) {
			CornerY = TopY;
			*CAt   |= 2;
			NoClipped++;

		// Are we clipped to the bottom?
		} else if (At->Y >= BottomY) {
			CornerY = BottomY;
			*CAt   |= 2;
			NoClipped++;
		}
	}

	// If we don't need to clip at all, quit
	if (!NoClipped)
		return false;


	// Clip on the X plane first
	Vector2D<float>	 Pt[16];						// Destination vertex array
	Vector2D<float>	*Src, *Dst;						// Source & destination vertex array
	unsigned short	*CSrc = Code;					// Current position in the source code array
	unsigned short	*CDst = &Code2[1];				// Current position in the destination code array

	Src  = Pts;
	Dst  = Pt + 1;
	CSrc = Code;
	for (i = Max ; i ; i--) {
		Src++;
		CSrc++;

		// Outside<-->Inside; insert the intersection
		if ((*CSrc & 1) != (*(CSrc - 1) & 1)) {
			float dx = (Src - 1)->X - Src->X;
			float dy = (Src - 1)->Y - Src->Y;

			Dst->X  = CornerX;
			Dst->Y  = Src->Y + dy * (CornerX - Src->X) / dx;
			*CDst++ = (Dst->Y < TopY) || (Dst->Y > BottomY);
			Dst++;
		}

		// Next vertex is inside; insert the next vertex
		if (!(*CSrc & 1)) {
			*Dst++  = *Src;
			*CDst++ = *CSrc >> 1;
		}
	}

	// Copy the last vertex and reset the new vertex count
	*Pt    = *(Dst - 1);
	*Code2 = *(CDst - 1);
	Max    = Dst - Pt - 1;


	// Clip on the Y plane
	Vector2D<float>	Pt2[16];						// Destination vertex array

	Src  = Pt;
	Dst  = Pt2;
	CSrc = Code2;
	for (i = Max ; i ; i--) {
		Src++;
		CSrc++;

		// Outside<-->Inside; insert intersection
		if (*CSrc != *(CSrc - 1)) {
			float dx = (Src - 1)->X - Src->X;
			float dy = (Src - 1)->Y - Src->Y;

			Dst->X = Src->X + dx * (CornerY - Src->Y) / dy;
			Dst->Y = CornerY;
			Dst++;
		}

		// Next vertex is inside; insert the next vertex
		if (!*CSrc)
			*Dst++ = *Src;
	}
	
	Max  = Dst - Pt2;


	// Now re-order the vertices for a top-bottom order
	// find the top-most vertex
	unsigned short j = 0;
	At    = Pt2;
	LeftX = Pt2->X;
	TopY  = Pt2->Y;
	for (i = Max ; i  ; i--, At++)
		if ((TopY > At->Y) || ((TopY == At->Y) && (LeftX > At->X))) {
			TopY = At->Y;
			j    = Max - i;
		}

	// then, copy them in order back into the Pts array
	memcpy(Pts, &Pt2[j], (Max - j) * sizeof(Pt2[0]));
	memcpy(&Pts[Max - j], &Pt2[0], (j + 1) * sizeof(Pt2[0]));

	return !Max;
}



// Find the vertices which composes the bounding 
// polygon given a set of points.
void Terrain::GetBoundingPolygon(Vector3D<float> *Src, Vector2D<float> *Pt, unsigned short &Max)
{
	unsigned short	 i, Tot = 0;	// Total number of vertices describing the bounding polygon
	Vector3D<float>	*At, *Best;		// Current vertex and best vertex in our Src array
	float			 BestSlope;		// Current Best Slope

	// Find the vertex which has the *physically highest* value in Z
	// SIMD min/max would be nice here
	Best = Src;
	At	 = &Src[1];
	for (i = 1 ; i < Max ; i++, At++)
		if ((Best->Z > At->Z) || ((Best->Z == At->Z) && (Best->X > At->X)))
			Best = At;

	// Insert this vertex as the first vertex of our frustrum
	Pt->X = Best->X;
	Pt->Y = Best->Z;

	// Find all the vertices which compose the upper-left quadran
	do {
		BestSlope	= 0;
		Best		= NULL;
		At			= Src;

		// Parse all the remaining vertices
		for (i = 0 ; i < Max ; i++, At++) {
			float dx = At->X - Pt->X;	// X Delta
			float dy = At->Z - Pt->Y;	// Y Delta

			// Is *At in the top-left quadran?
			if ((dx <= 0) && (dy > 0)) {
				// If it's vertical and we haven't found anything better yet; take it
				if (!dx) { 
					if (!BestSlope && (!Best || (Best && (At->Y > Best->Y))))
						Best = At;

				} else {
					float Slope = dy / dx;

					// We're looking for the Greatest slope
					if (!BestSlope || (Slope > BestSlope) || ((Slope == BestSlope) && (At->X < Best->X))) {
						Best		= At;
						BestSlope	= Slope;
					}
				}
			}
		}

		// Should we insert a new vertex in the frustrum's list?
		if (Best) {
			Max--;
			Pt++;
			Tot++;
			Pt->X = Best->X;
			Pt->Y = Best->Z;
			*Best = *Src++;	// Remove the vertex from the search list
		}
	} while (BestSlope);

	// Find all the vertices which compose the lower-left quadran
	do {
		BestSlope	= 0;
		Best		= NULL;
		At			= Src;

		// Parse all the remaining vertices
		for (i = 0 ; i < Max ; i++, At++) {
			float dx = At->X - Pt->X;	// X Delta
			float dy = At->Z - Pt->Y;	// Y Delta

			// Is *At in the bottom-left quadran?
			if ((dx > 0) && (dy >= 0)) {
				// If it's vertical and we haven't found anything better yet; take it
				if (!dy) { 
					if (!BestSlope && (!Best || (Best && (At->X > Best->X))))
						Best = At;

				} else {
					float Slope = dy / dx;

					// We're looking for the Greatest slope
					if (!BestSlope || (Slope > BestSlope) || ((Slope == BestSlope) && (At->Y > Best->Y))) {
						Best		= At;
						BestSlope	= Slope;
					}
				}
			}
		}

		// Should we insert a new vertex in the frustrum's list?
		if (Best) {
			Max--;
			Pt++;
			Tot++;
			Pt->X = Best->X;
			Pt->Y = Best->Z;
			*Best = *Src++;	// Remove the vertex from the search list
		}
	} while (BestSlope);

	// Find all the vertices which compose the lower-right quadran
	do {
		BestSlope	= 0;
		Best		= NULL;
		At			= Src;

		// Parse all the remaining vertices
		for (i = 0 ; i < Max ; i++, At++) {
			float dx = At->X - Pt->X;	// X Delta
			float dy = At->Z - Pt->Y;	// Y Delta

			// Is *At in the bottom-right quadran?
			if ((dx >= 0) && (dy < 0)) {
				// If it's vertical and we haven't found anything better yet; take it
				if (!dx) {
					if (!BestSlope && (!Best || (Best && (At->Y < Best->Y))))
						Best = At;

				} else {
					float Slope = dy / dx;

					// We're looking for the Greatest slope
					if (!BestSlope || (Slope > BestSlope) || ((Slope == BestSlope) && (At->X > Best->X))) {
						Best		= At;
						BestSlope	= Slope;
					}
				}
			}
		}

		// Should we insert a new vertex in the frustrum's list?
		if (Best) {
			Max--;
			Pt++;
			Tot++;
			Pt->X = Best->X;
			Pt->Y = Best->Z;
			*Best = *Src++;	// Remove the vertex from the search list
		}
	} while (BestSlope);

	// Find all the vertices which compose the upper-right quadran
	do {
		BestSlope	= 0;
		Best		= NULL;
		At			= Src;

		// Parse all the remaining vertices
		for (i = 0 ; i < Max ; i++, At++) {
			float dx = At->X - Pt->X;	// X Delta
			float dy = At->Z - Pt->Y;	// Y Delta

			// Is *At in the top-right quadran?
			if ((dx < 0) && (dy <= 0)) {
				// If it's vertical and we haven't found anything better yet; take it
				if (!dy) {
					if (!BestSlope && (!Best || (Best && (At->X < Best->X))))
						Best = At;

				} else {
					float Slope = dy / dx;

					// We're looking for the Greatest slope
					if (!BestSlope || (Slope > BestSlope) || ((Slope == BestSlope) && (At->Y < Best->Y))) {
						Best		= At;
						BestSlope	= Slope;
					}
				}
			}
		}

		// Should we insert a new vertex in the frustrum's list?
		if (Best) {
			Max--;
			Pt++;
			Tot++;
			Pt->X = Best->X;
			Pt->Y = Best->Z;
			*Best = *Src++;	// Remove the vertex from the search list
		}
	} while (BestSlope);

	Max = Tot;
}



// Find the scanline edges by calculating the frustrum corners
// clipping in 3D, finding the bounding polygon and clipping
// in 2D.  Optionally, it draws debug information.
bool Terrain::Update(Camera &Cam)
{
	Vector2D<float>	 Pts2D[16];	// 2D bounding polygon
	Vector3D<float>	 Pts3D[16];	// 3D frustrum corner
	unsigned short	 Max = 8;	// Total points describing the polygon

	// Determine the center of the frustrum from the grid in grid coordinates
	Center.X	= (float)floor(Cam.Position.X / VERTEX_PER_SAMPLE) * VERTEX_PER_SAMPLE;
	Center.Y	= (float)floor(Cam.Position.Z / VERTEX_PER_SAMPLE) * VERTEX_PER_SAMPLE;

	// Get our bounding frustrum polygon
	Cam.Update		();
	Cam.GetFrustrumCorners(Pts3D);
	if (ClipPlanes && Clip3D(Pts3D, Max)) {
		StartY = 0;
		StopY  = -1;
		return false;
	}
	GetBoundingPolygon(Pts3D, Pts2D, Max);


	// Convert our coordinates into grid-space
	Vector2D<float>	*At = Pts2D;	// Current position in our 2D bounding polygon
	for (unsigned short i = Max + 1 ; i ; i--) {
		At->X = (At->X - (Cam.Position.X - Center.X)) / VERTEX_PER_SAMPLE;
		At->Y = (At->Y - (Cam.Position.Z - Center.Y)) / VERTEX_PER_SAMPLE;
		At++;
	}


	// Clip the polygon to our bounding rectangle & scanline it
	if (Clip2D(Pts2D, Max)) {
		StartY = 0;
		StopY  = -1;
		return false;
	}
	ScanLine(Pts2D, Max);

	// If we want to see our bounding polygon
	if ((ID == ShowID) && ShowLines) {
		unsigned short i = 0;
		float	Y	= StartY * VERTEX_PER_SAMPLE;
		Span	*At	= Strips;

		glDisable		(GL_CULL_FACE);
		glMatrixMode	(GL_MODELVIEW);
		glPushMatrix	();
		glLoadIdentity	();

		glColor3f		(1, 0, 0);
		glBegin			(GL_LINE_LOOP);

		// Show the bounding polygon on the screen
		for (i = Max ; i ; i--)
			glVertex3d	(Pts2D[i].X * VERTEX_PER_SAMPLE / SCALE, -Pts2D[i].Y * VERTEX_PER_SAMPLE / SCALE, -6);

		glEnd();
		glPopMatrix		();
		glEnable		(GL_CULL_FACE);
	}

	// If we want to see how we're choosing our polygons
	if ((ID == ShowID) && ShowFill) {
		unsigned short i = 0;
		float	Y	= StartY * VERTEX_PER_SAMPLE;
		Span	*At	= Strips;

		glDisable		(GL_CULL_FACE);
		glMatrixMode	(GL_MODELVIEW);
		glPushMatrix	();
		glLoadIdentity	();

		// Render all the scanlines
		do {
			switch (i) {
				case 0:	glColor3f	(0, 0, 1); break;
				case 1: glColor3f	(0, 1, 0); break;
			}

			glBegin		(GL_TRIANGLE_STRIP);

			glVertex3d	((At->Start * VERTEX_PER_SAMPLE) / SCALE, (-Y)						/ SCALE, -6);
			glVertex3d	((At->Start * VERTEX_PER_SAMPLE) / SCALE, (-Y - VERTEX_PER_SAMPLE)	/ SCALE, -6);
			glVertex3d	((At->End	* VERTEX_PER_SAMPLE) / SCALE, (-Y)						/ SCALE, -6);
			glVertex3d	((At->End	* VERTEX_PER_SAMPLE) / SCALE, (-Y - VERTEX_PER_SAMPLE)	/ SCALE, -6);

			glEnd		();

			At++;
			Y += VERTEX_PER_SAMPLE;
			i = (i + 1) & 1;
		} while (Y <= StopY * VERTEX_PER_SAMPLE);

		glPopMatrix		();
		glEnable		(GL_CULL_FACE);
	}

	// If we want to see where our 2 clipping planes are
	if ((ID == ShowID) && ShowPlanes) {
		glMatrixMode	(GL_MODELVIEW);
		glPushMatrix	();
		glDisable		(GL_CULL_FACE);
		glTranslatef	(Cam.Position.X, Cam.Position.Y, Cam.Position.Z);

		glColor3f		(1, 0, 0);
		glBegin			(GL_QUADS);

		// Draw the top plane
		glVertex3f		(0, TopY, 0);
		glVertex3f		(0, TopY, 735);
		glVertex3f		(735, TopY, 735);
		glVertex3f		(735, TopY, 0);

		// Draw the bottom plane
		glVertex3f		(0, BottomY, 0);
		glVertex3f		(0, BottomY, 735);
		glVertex3f		(735, BottomY, 735);
		glVertex3f		(735, BottomY, 0);

		glEnd			();

		glEnable		(GL_CULL_FACE);
		glPopMatrix		();
	}

	return true;
}
