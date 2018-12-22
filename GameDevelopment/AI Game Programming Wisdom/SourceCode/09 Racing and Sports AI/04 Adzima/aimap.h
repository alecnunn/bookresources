#ifndef AI_MAP_H
#define AI_MAP_H

class aiPath;
class aiObstacle;

class aiIntersection
{
	public:
		aiIntersection();

		int Id();									// Return the intersection identification
		int NumRoads();								// Return the number of roads connected to this intersection
		aiPath* Road(int nWhich);					// Return a road connected to this intersection

		aiObstacle* Vehicles();
};

class aiPath
{
	public:
		aiPath();

		int Id();									// Return the road identification
		int NumVerts();
		int NumLanes(int nDirection);
		bool Divider();								// Returns true when the road is divided and cannot be crossed.
		Vector3& CenterVertice(int nWhich);			
		Vector3& VertXDir(int nWhich);	
		Vector3& VertYDir(int nWhich);	
		Vector3& VertZDir(int nWhich);	
		Vector3& LBoundary(int nWhich);
		Vector3& RBoundary(int nWhich);

		aiObstacle* Vehicles(int nVert,int nSide);
		int  RoadVertice(const Vector3& Pos,int nDir,int nVertHint);
		int  IsPosOnRoad(const Vector3& Pos,float fVehHalfWidth,float *pfSideDist);
		float LaneWidth(int nIdx,int nInner,int nSide);

		aiIntersection* Destination();				// Return the destination intersection
		aiIntersection* Departure();				// Return the departure intersection
};

class aiMap
{
	public:
		aiMap(){};

		aiPath*	Path(int nWhich);					// Return the requested road.
		aiIntersection* Intersection(int nWhich);	// Return the requested intersection.

		aiPath* DetRdSegBetweenInts(aiIntersection *pInt0,aiIntersection *pInt1,bool bDir);
		int MapComponent(const Vector3& Position,short *pnCompId,short *pnCompType,short nHint,
						 short nTargetRdId);
};

enum aiMapCompTypes{kRoad,kIntersection};

#endif
