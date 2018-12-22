/* Copyright (C) Dan Higgins, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Higgins, 2001"
 */
// This is used to modify the pathfinder,
// making it slightly slower, but saves memory.
class PathModifierSubmarine : public PathModifierGoalBase
{
public:
    virtual long GetTileCost( U2Dpoint<long>& inFrom,
                              U2Dpoint<long>& inTo);

    virtual bool TileIsOpen( Tile* inTile, U2Dpoint<long>& inPoint);
};
