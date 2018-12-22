/* Copyright (C) Steve Rabin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2001"
 */
void RDC( Pairs& pairs, Group& group, 
          Axis axis1, Axis axis2, Axis axis3 )
{
  //"pairs" holds all final pairs that are in collision
  //"group" is the current group of objects to analyze
  //"axis1" is the axis to analyze within this function
  //"axis2", "a3" will be analyzed in recursive calls

  if( Size( group ) < 10 || axis1 == INVALID_AXIS )
  { //end recursion and test for collisions
    BruteForceComparison( pairs, subGroup );
  }
  else { 
    //for this group, find the boundaries and sort them
    OpenCloseBoundaryList boundaries;
    FindOpenCloseBoundaries( axis1, group, boundaries );
    SortOpenCloseBoundaries( boundaries );  //O(nlogn)

    Group subGroup;
    unsigned int count = 0;
    Axis newAxis1 = axis2;
    Axis newAxis2 = axis3;
    Axis newAxis3 = INVALID_AXIS;
    bool groupSubdivided = false;

    //subdivide the group if possible and call recursively
    for( every curBoundary in boundaries list )
    {
      if( curBoundary is "open bracket" )
      { //this entity lies within a cluster group
        count++;
        AddToGroup( subGroup, curBoundary->entity );
      }
      else
      {
        count--;
        if( count == 0 )
        { //found the end of a cluster group - take subgroup
          //and call recursively on remaining axis'

          if( curBoundary != GetLastBoundary( boundaries ) )
          { //this group is being subdivided - remember
            groupSubdivided = true;
          }

          if( groupSubdivided )
          { //reconsider all other axis'
            if( axis1 == X_AXIS ) {
              newAxis1 = Y_AXIS;
              newAxis2 = Z_AXIS;
            }
            else if( axis1 == Y_AXIS ) {
              newAxis1 = X_AXIS;
              newAxis2 = Z_AXIS;
            }
            else if( axis1 == Z_AXIS ) {
              newAxis1 = X_AXIS;
              newAxis2 = Y_AXIS;
            }
          }
        }

        //recursive call
        RecursiveClustering( pairs, subGroup,
                             newAxis1, newAxis2, INVALID_AXIS );
        Clear( subGroup ); //clear the subGroup for the next group
      }
    }
  }
}
