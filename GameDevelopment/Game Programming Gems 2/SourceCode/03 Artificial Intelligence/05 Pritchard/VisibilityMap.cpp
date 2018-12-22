/* Copyright (C) Matt Pritchard, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Matt Pritchard, 2001"
 */
// This routine "explores" the game map in an area around the specified
// unit position using a line of sight template
// The template span data is an array of structs with 3 offset elements:
//  {vertical position, horizontal start, horizontal end}

void VisibilityMap::AddLOSTemplate(int XPosition, int YPosition, LOSTemplate *template)
{
   int n, x, y, xStart, xEnd;

   for (n = 0; n < template->number_of_vertical_lines; n++)
   {
      y = Yposition + template->SpanData[n].Yoffset;
      if (y >= map_y_min_coord && y <= map_y_max_coord) {
         xStart = max(XPosition + template->lines[n].XStartOffset, map_x_min_coord);
         xEnd = min(XPosition + template->lines[n].XEndOffset, map_x_max_coord);

         for (x = xStart; x <= xEnd; x++) {
            if ((VisibleMap[y][x]++) == 0) {
               ExploreTileForFisrtTimeHandler(x, y);
               VisbleMap[y][x] = 1;
            }
         }
      }
   }
}
