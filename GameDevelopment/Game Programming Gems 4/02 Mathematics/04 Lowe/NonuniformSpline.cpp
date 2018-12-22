#include "NonuniformSpline.h"

Matrix hermite = // Matrix H in article
{ 
  {
   { 2.f,-2.f, 1.f, 1.f},
   {-3.f, 3.f,-2.f,-1.f},
   { 0.f, 0.f, 1.f, 0.f},
   { 1.f, 0.f, 0.f, 0.f}
  }
};

// cubic curve defined by 2 positions and 2 velocities
Vector3 GetPositionOnCubic(const Vector3 &startPos, const Vector3 &startVel, const Vector3 &endPos, const Vector3 &endVel, float time)
{
  Matrix m;
  m.SetRow0(startPos); 
  m.SetRow1(endPos);
  m.SetRow2(startVel); 
  m.SetRow3(endVel);
  m.Multiply4x4(&hermite, &m); // multiply by the mixer

  Vector3 timeVector = {time*time*time, time*time, time, 1.f};
  return ApplyMatrix(timeVector, &m); // vector * matrix
}

/*********************************** R N S **************************************************/

// adds node and updates segment length
void RNS::AddNode(const Vector3 &pos)
{
  if (nodeCount == 0)
    maxDistance = 0.f;
  else
  {
    node[nodeCount-1].distance = (node[nodeCount-1].position - pos).Magnitude();
    maxDistance += node[nodeCount-1].distance;
  }
  node[nodeCount++].position = pos;
}

// called after all nodes added. This function calculates the node velocities
void RNS::BuildSpline()
{
  for (int i = 1; i<nodeCount-1; i++)
  {
    // split the angle (figure 4)
    node[i].velocity = Normalise(node[i+1].position - node[i].position) - 
                       Normalise(node[i-1].position - node[i].position);
    node[i].velocity.Normalise();
  }
  // calculate start and end velocities
  node[0].velocity = GetStartVelocity(0);
  node[nodeCount-1].velocity = GetEndVelocity(nodeCount-1);
}

// spline access function. time is 0 -> 1
Vector3 RNS::GetPosition(float time)
{
  float distance = time * maxDistance;
  float currentDistance = 0.f;
  int i = 0;
  while (currentDistance + node[i].distance < distance
    && i < 100)
  {
    currentDistance += node[i].distance;
    i++;
  }
  float t = distance - currentDistance;
  t /= node[i].distance; // scale t in range 0 - 1
  Vector3 startVel = node[i].velocity * node[i].distance;
  Vector3 endVel = node[i+1].velocity * node[i].distance;   
  return GetPositionOnCubic(node[i].position, startVel,
                         node[i+1].position, endVel, t);
}

// internal. Based on Equation 14 
Vector3 RNS::GetStartVelocity(int index)
{
  Vector3 temp = 3.f*(node[index+1].position - node[index].position)/node[index].distance;
  return (temp - node[index+1].velocity)*0.5f;
}

// internal. Based on Equation 15 
Vector3 RNS::GetEndVelocity(int index)
{
  Vector3 temp = 3.f*(node[index].position - node[index-1].position)/node[index-1].distance;
  return (temp - node[index-1].velocity)*0.5f;
}

/*********************************** S N S **************************************************/

// smoothing filter.
void SNS::Smooth()
{
  Vector3 newVel;
  Vector3 oldVel = GetStartVelocity(0);
  for (int i = 1; i<nodeCount-1; i++)
  {
    // Equation 12
    newVel = GetEndVelocity(i)*node[i].distance +
             GetStartVelocity(i)*node[i-1].distance;
    newVel /= (node[i-1].distance + node[i].distance);
    node[i-1].velocity = oldVel;
    oldVel = newVel;
  }
  node[nodeCount-1].velocity = GetEndVelocity(nodeCount-1);
  node[nodeCount-2].velocity = oldVel;
}

/*********************************** T N S **************************************************/

// as with RNS but use timePeriod in place of actual node spacing
// ie time period is time from last node to this node
void TNS::AddNode(const Vector3 &pos, float timePeriod)
{
  if (nodeCount == 0)
    maxDistance = 0.f;
  else
  {
    node[nodeCount-1].distance = timePeriod;
    maxDistance += node[nodeCount-1].distance;
  }
  node[nodeCount++].position = pos;
}

// stabilised version of TNS
void TNS::Constrain()
{
  for (int i = 1; i<nodeCount-1; i++)
  {
    // Equation 13
    float r0 = (node[i].position-node[i-1].position).Magnitude()/node[i-1].distance;
    float r1 = (node[i+1].position-node[i].position).Magnitude()/node[i].distance;
    node[i].velocity *= 4.f*r0*r1/((r0+r1)*(r0+r1));
  }
}

