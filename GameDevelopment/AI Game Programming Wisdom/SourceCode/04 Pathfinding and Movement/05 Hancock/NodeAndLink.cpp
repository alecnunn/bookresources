#include "NodeAndLink.h"

bool PathNode::PathExists(const PathNode *n2, const Character *pChar) const{
	bool anyPath = NodeMap::RegionConnection(GetRegion(Connected), n2->GetRegion(Connected));
	if (!anyPath || !pChar) //if there's not a path, or no character to check further, return the result
		return anyPath;
	if (!ReachableWithoutJumping(n2) && !pChar->CanJump())
		return false;
	if (!NoDoorsBetween(n2) && !pChar->CanUseDoors())
		return false;
	if (!NoElevatorsBetween(n2) && pChar->CanUseElevators())
		return false;
	return true;
}

bool PathNode::NoDoorsBetween(const PathNode *n2) const{
	return NodeMap::DoorRegionConnection(GetRegion(NoDoor), n2->GetRegion(NoDoor));
}

bool PathNode::NoElevatorsBetween(const PathNode *n2) const{
	return NodeMap::ElevatorRegionConnection(GetRegion(NoElevator), n2->GetRegion(NoElevator));
}

bool PathNode::ReachableWithoutForceJumping(const PathNode *n2) const {
	return NodeMap::JumpRegionConnection(GetRegion(NoJump), n2->GetRegion(NoJump));
}


Vector3 PathLink::ClosestPointOnLinkLine(const Vector3& q, float &fraction) const{
	Vector3 dir = next->pos - origin->pos;
	fraction = dir.dot(q - origin->pos) / dir.dot(dir);
	Vector3 a = origin->pos + fraction * dir;
	return a;
}

bool PathLink::InsideFatLink(const Vector3& p) const{

	float fraction;
	Vector3 a = ClosestPointOnLinkLine(p, fraction);
	Vector3 q = p;
	q.y = a.y; //set the vertical position to be the same as the projection
	if (fraction < 0.0f)
		return (q.distance2(origin->pos) < (origin->radius * origin->radius));
	else if (fraction > 1.0f)
		return (q.distance2(next->pos) < (next->radius * next->radius));
	float localWidth = fraction * next->radius + (1.0f - fraction) * origin->radius;
	CVector diff = q;
	diff -= a;
	return (diff.length2() < Sqr(localWidth));
}