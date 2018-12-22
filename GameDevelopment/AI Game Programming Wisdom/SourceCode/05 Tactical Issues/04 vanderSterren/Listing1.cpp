/* Copyright (C) William van der Sterren, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2001"
 */

// To construct a path according to our pullback requirements, 
// the maneuver can use a cost function to evaluate the next 
// node on the path such as:

float CostsForNextNode(node next, const vector<node>& pathsegment) const 
{
  float traveltimecosts, lackofcovercosts, nobypasscosts, unsafecosts;
  traveltimecosts  = TravelTime(pathsegment.back(), next);
  lackofcovercosts = NumberOfNodesAbleToFireAtNode(pathsegment, next);
  nobypasscosts    = LackOfByPassAtNodeToNode(pathsegment.back(), next);
  unsafecosts      = TacticallyBadPosition(next);

  return   kDuration   * traveltimecosts + kCover * lackofcovercosts
         + kAmpleSpace * nobypasscosts   + kSafe  * unsafecosts;
}
