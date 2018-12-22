#include "aiNavigation.h"
#include "aiMap.h"
#include "aiObstacle.h"

static int _nMaxTrys=10;
static int _nMaxRouteSplits=5;
static float _fFriction=1.2f;
static float _fTurnThreshold=.7f;
static float _fMaxStraightAngle=.2f;

extern aiMap AIMAP;

inline short aiNavigation::CheckPt(int nIdx)
{
	if(nIdx<0 || nIdx>=m_nNumWayPts)
	{
		return -1;
	}
	return m_panWayPts[nIdx];
}

aiNavigation::aiNavigation()
{
}

aiRouteNode::aiRouteNode() {
	Reset();
}

void aiRouteNode::Reset() {
	m_pObstacle=0;
	Vtx.Zero();
	m_fCumAngle=0.0f;
	m_fCumDist=0.0f;
	m_nSRd=0;
	m_nSVIdx=0;
	m_nType=0;
	m_nTurnId=0;
	m_nObsType=0;
	m_nOnRoad=0;	// 0: Off the road, 1: On the road, 2: On the sidewalk
}

aiNavigation::~aiNavigation()
{
}

void aiNavigation::Init(int nId,char *pGeoFile) 
{
	m_Car.Init(nId,pGeoFile); 

	Vector3 Min=m_Car.GetBoxMin();
	Vector3 Max=m_Car.GetBoxMax();
	m_fFrontBumperDistance=-Min.z;
	m_fBackBumperDistance=Max.z;
	m_fLSideDist=-Min.x;
	m_fRSideDist=Max.x;

	Reset();
}

void aiNavigation::Reset() 
{
	m_nLastState=-1;
	m_nState=kForward;
	m_Car.Reset();
	m_bFinished=false;

	// Not sure if these are good default values, but for replays sake...
	m_nWayPtIdx=1;
	m_nCurMapCompType=3;
	m_nCurMapCompIdx=1;
	m_nDestMapCompType=3;
	m_nDestMapCompId=1;
	m_fTurnCenterPtOffset=0.f;
	m_nNumWayPts=1;
	m_nBestRoute=-1;
	m_nNumRoutes=0;
	m_bDir[0]=m_bDir[1]=m_bDir[2]=0;
	m_fRdAngle[0]=m_fRdAngle[1]=0.f;
	m_fTurn[0]=m_fTurn[1]=0.f;
	m_LPos.Set(0.f,0.f,0.f);
	m_LPosition.Set(0.f,0.f,0.f);
	m_Destination.Set(0.f,0.f,0.f);
	m_DestHeading.Set(0.f,0.f,0.f);
	m_IntersectionPt[0].Set(0.f,0.f,0.f);
	m_IntersectionPt[1].Set(0.f,0.f,0.f);
	m_TurnCenterPt[0].Set(0.f,0.f,0.f);
	m_TurnCenterPt[1].Set(0.f,0.f,0.f);
	m_TurnStartDir[0].Set(0.f,0.f,0.f);
	m_TurnStartDir[1].Set(0.f,0.f,0.f);
	m_TurnEndDir[0].Set(0.f,0.f,0.f);
	m_TurnEndDir[1].Set(0.f,0.f,0.f);
	m_fTurnSetback[0]=m_fTurnSetback[1]=0.f;
	m_fRadius[0]=m_fRadius[1]=0.f;

	m_fBrake=0.0f;
	m_fThrottle=0.0f;
	m_fSteering=0.0f;

	for(int i=0;i<3;i++)
		m_pRoads[i]=0;

	for(int k=0;k<MAX_ROUTE_NODES;k++) {
		m_Route[k].Reset();
	}

	for(int j=0; j< MAX_NUM_ROUTES; j++) {
		m_naNumRouteVerts[j]=0;
		for(int i=0;i<MAX_ROUTE_NODES;i++) {
			m_AltRoute[j][i].Reset();
		}
	}
}

void aiNavigation::RegisterRoute(short *panRoute,short nNumRouteNodes,const Vector3& Destination,
									 const Vector3& DestHeading,int nNumRepeats,float fTargetSpeed,
									 float fDestOffset,float fTurnVelFactor,float fBrakeThreshold,
									 float fRoutePlanDist)
{
	aiIntersection *pInt0, *pInt1;

	m_fTurnVelFactor=fTurnVelFactor;
	m_fBrakeThreshold=fBrakeThreshold;
	m_fRoutePlanDist=fRoutePlanDist;
	
	m_panWayPts=panRoute;
	m_nNumWayPts=nNumRouteNodes;
	m_pPrevRoad=0;
	m_LPosition=m_Car.GetMatrix().Pos;
	m_LPos=m_Car.GetMatrix().Pos;
	m_nBestRoute=-1;
	m_TargetPt=m_Car.GetMatrix().Pos;

	m_pRoads[0]=0;
	m_pRoads[1]=0;
	m_pRoads[2]=0;

	m_fRdAngle[0]=0.f;
	m_fRdAngle[1]=0.f;

	m_Route[0].m_nSVIdx=0;
	m_Route[0].m_nSRd=0;

	m_Destination=Destination;
	m_DestHeading=DestHeading;
	m_fDestSpeed=fTargetSpeed;
	m_fDestOffset=fDestOffset;
	DestMapComponent(m_Destination,&m_nDestMapCompId,&m_nDestMapCompType);

	int nTargetId=-1;
	if(m_pRoads[0])
		nTargetId=m_pRoads[0]->Id();

	AIMAP.MapComponent(m_Car.GetMatrix().Pos,&m_nCurMapCompIdx,&m_nCurMapCompType,
					   m_Car.GetRoomId(),nTargetId);
	switch(m_nCurMapCompType)
	{
		case kIntersection:
		{
			m_nRdIdx=0;

			m_nWayPtIdx=0;
			m_pRoads[0]=AIMAP.Intersection(m_nCurMapCompIdx)->Road(0);
			if(m_pRoads[0]->Destination()==AIMAP.Intersection(m_nCurMapCompIdx))
				m_bDir[0]=1;	// The direction that the vehicle will traverse the road
			else
				m_bDir[0]=0;

			if(m_nNumWayPts==2)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				m_nWayPtIdx=1;
				m_nRdIdx=1;
			}
			else if(m_nNumWayPts==3)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				pInt0=AIMAP.Intersection(CheckPt(1));
				pInt1=AIMAP.Intersection(CheckPt(2));
				m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
				m_nWayPtIdx=1;
				m_nRdIdx=2;
			}
			else if(m_nNumWayPts>3)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				pInt0=AIMAP.Intersection(CheckPt(1));
				pInt1=AIMAP.Intersection(CheckPt(2));
				m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
				pInt0=AIMAP.Intersection(CheckPt(2));
				pInt1=AIMAP.Intersection(CheckPt(3));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
				m_nWayPtIdx=1;
				m_nRdIdx=3;
			}

			m_nState=kForward;
			if(m_bDir[0])
			{
				Vector3 p=m_pRoads[0]->CenterVertice(0)-m_Car.GetMatrix().Pos;
				m_fSideDist=aiDot(p,m_pRoads[0]->VertXDir(0));
			}
			else
			{
				int nNumVerts=m_pRoads[0]->NumVerts();
				Vector3 p=m_pRoads[0]->CenterVertice(nNumVerts-1)-m_Car.GetMatrix().Pos;
				m_fSideDist=aiDot(p,-m_pRoads[0]->VertXDir(nNumVerts-1));
			}
			break;
		}
		case kRoad:
		{
			int nCompIdx=m_nCurMapCompIdx;
			m_nWayPtIdx=0;
			m_bDir[0]=1;
			m_pRoads[0]=AIMAP.Path(nCompIdx);

			m_nRdIdx=0;
			if(m_nNumWayPts==2)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				m_nWayPtIdx=1;
				m_nRdIdx=1;
			}
			else if(m_nNumWayPts==3)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				pInt0=AIMAP.Intersection(CheckPt(1));
				pInt1=AIMAP.Intersection(CheckPt(2));
				m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
				m_nWayPtIdx=1;
				m_nRdIdx=2;
			}
			else if(m_nNumWayPts>3)
			{
				pInt0=AIMAP.Intersection(CheckPt(0));
				pInt1=AIMAP.Intersection(CheckPt(1));
				m_pRoads[0]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[0]);
				pInt0=AIMAP.Intersection(CheckPt(1));
				pInt1=AIMAP.Intersection(CheckPt(2));
				m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
				pInt0=AIMAP.Intersection(CheckPt(2));
				pInt1=AIMAP.Intersection(CheckPt(3));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
				m_nWayPtIdx=1;
				m_nRdIdx=3;
			}

			m_nState=kForward;
			int nVIdx=aiClamp(m_pRoads[0]->RoadVertice(m_Car.GetMatrix().Pos,m_bDir[0],-1),
							0,m_pRoads[0]->NumVerts()-1);
			if(m_bDir[0])
			{
				Vector3 p=m_pRoads[0]->CenterVertice(nVIdx)-m_Car.GetMatrix().Pos;
				m_fSideDist=aiDot(p,m_pRoads[0]->VertXDir(nVIdx));
			}
			else
			{
				int nNumVerts=m_pRoads[0]->NumVerts();
				Vector3 p=m_pRoads[0]->CenterVertice(nNumVerts-nVIdx-1)-m_Car.GetMatrix().Pos;
				m_fSideDist=aiDot(p,-m_pRoads[0]->VertXDir(nNumVerts-nVIdx-1));
			}
			break;
		}
	}
}

void aiNavigation::DriveRoute()
{
	switch(m_nState)
	{
		case kBackup:
		{
			if(m_nLastState!=m_nState)
			{
				InitBackup();
				m_nLastState=m_nState;
			}
			Backup();
			break;
		}
		case kForward:
		{
			if(m_nLastState!=m_nState)
			{
				InitForward();
				m_nLastState=m_nState;
			}
			Forward();
			break;
		}
		case kStop:
		{
			Stop();
			break;
		}
	}
}

void aiNavigation::Stop()
{
	m_TargetPt=m_Destination;
	Vector3 TargetHeading=m_Destination-m_Car.GetMatrix().Pos;
	float fX=aiDot(TargetHeading,m_Car.GetMatrix().XDir);
	float fZ=aiDot(TargetHeading,m_Car.GetMatrix().ZDir);
	float fAngle=atan2f(fX,-fZ)*1.33f;
	m_fSteering=aiClamp(fAngle,-1.f,1.f);
	m_Car.SetSteering(m_fSteering);

	m_fThrottle=0.f;
	m_fBrake=1.f;
	m_Car.SetThrottle(0.f);
	m_Car.SetBrake(1.f);

	m_naNumRouteVerts[0]=0;
	m_nNumRoutes=0;
}

void aiNavigation::InitForward()
{
	m_fSteering=0.f;
	m_Car.SetSteering(0.f);
	m_Car.SetThrottle(0.f);
	m_Car.SetBrake(0.f);
	m_TargetPt=m_Car.GetMatrix().Pos;

	m_nNumRoutes=0;
	m_nBestRoute=-1;
	m_fTurnCenterPtOffset=0.f;
	for(int ctr=0;ctr<MAX_NUM_ROUTES;ctr++)
	{
		m_naOffRoadRoute[ctr]=0;
		m_naBlockedRoute[ctr]=0;
		m_naNumRouteVerts[ctr]=0;
	}
}

void aiNavigation::Forward()
{
	if(m_Car.Stuck())
	{
		PlanRoute();
		m_nState=kBackup;
		return;
	}

	// Determine our road segment and lane
	short nTargetRdId=-1;
	if(m_nWayPtIdx<m_nNumWayPts)
	{
		bool bDir;
		if(m_nWayPtIdx==0)
		{
			aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nNumWayPts-1));
			aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			aiPath *pCurRoad=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&bDir);
			if(pCurRoad)
				nTargetRdId=pCurRoad->Id();
		}
		else
		{
			aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx-1));
			aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			aiPath *pCurRoad=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&bDir);
			if(pCurRoad)
				nTargetRdId=pCurRoad->Id();
		}
	}
	else
	{
		if(m_nDestMapCompType==kRoad)
		{
			nTargetRdId=m_nDestMapCompId;
		}
	}
	AIMAP.MapComponent(m_Car.GetMatrix().Pos,&m_nCurMapCompIdx,
					   &m_nCurMapCompType,m_Car.GetRoomId(),nTargetRdId);
	SolveRoadTargetPoint();

	// if the destination is behind me, and the dest heading is similar
	Vector3 TargetHeading;
	if(m_AltRoute[m_nBestRoute][1].m_fCumDist==9999.f &&		// going for the destination
	   aiDist2(m_Car.GetMatrix().Pos,m_Destination)<625.f &&
	   aiDot(-m_Car.GetMatrix().ZDir,m_DestHeading)>0.f &&
	   aiDot((m_Destination-m_Car.GetMatrix().Pos),-m_Car.GetMatrix().ZDir)<0.f)
	{
		TargetHeading=-(m_TargetPt-m_Car.GetMatrix().Pos);
		m_fBrake=1.f;
		m_fThrottle=0.f;
	}
	else
	{
		TargetHeading=m_TargetPt-m_Car.GetMatrix().Pos;
	}

	float fX=aiDot(TargetHeading,m_Car.GetMatrix().XDir);
	float fZ=aiDot(TargetHeading,m_Car.GetMatrix().ZDir);
	float fAngle=atan2f(fX,-fZ)*1.33f;

	fAngle*=1.428f;
	m_fSteering=aiClamp(fAngle,-1.f,1.f);

	m_Car.SetSteering(m_fSteering);
	m_Car.SetBrake(m_fBrake);
	m_Car.SetThrottle(m_fThrottle);
}

void aiNavigation::InitBackup()
{
	m_nBackupCount=0;
	m_TargetPt=m_AltRoute[m_nBestRoute][1].Vtx;
	m_Car.SetTransmissionToReverse();
}

void aiNavigation::Backup()
{
	// Calc turn angle to reach the target.
	Vector3 TargetHeading=m_TargetPt-m_Car.GetMatrix().Pos;

	float fX=TargetHeading^m_Car.GetMatrix().XDir;
	float fZ=TargetHeading^-m_Car.GetMatrix().ZDir;

	float fAngle=atan2f(fX,fZ);
	if(fAngle>0.1f || fAngle<-.1f)
	{
		float fSteering=-fAngle/.35f;
		fSteering=aiClamp(fSteering,-1.f,1.f);
		m_Car.SetSteering(fSteering);
	}
	else
	{
		m_Car.SetSteering(0.f);
		FinishedBackingUp();
		return;
	}

	if(m_nBackupCount>65)
	{
		FinishedBackingUp();
		return;
	}
	else
	{
		m_Car.SetThrottle(0.85f);
		m_Car.SetBrake(0.f);
	}
	m_nBackupCount++;
}

void aiNavigation::FinishedBackingUp()
{
	// Restore the state before the backup.
	m_nState=kForward;
	m_Car.SetThrottle(0.0f);
	m_Car.SetBrake(1.f);
}

//------Privates-----------------------------------------------------------------

void aiNavigation::CalcSpeed()
{
	m_fThrottle=1.f;
	m_fBrake=0.f;

	// Check if near the final destination which occurs at the end of the race.
	float fDist=aiDist2(m_Car.GetMatrix().Pos,m_Destination);
	if(fDist<5000.f && m_nWayPtIdx>=m_nNumWayPts)
	{
		float fTargetVel=m_fDestSpeed;
		float fDistToTarget=(float)sqrt(fDist)-m_fDestOffset;
		float fTargetTime=fDistToTarget/m_Car.Speed();
		float fBrake=0.f;
		if(m_Car.Speed()>fTargetVel)
			fBrake=(m_Car.Speed()-fTargetVel)/(fTargetTime*_fFriction*19.8f);

		if(fBrake>fDistToTarget*.014)
		{
			m_fThrottle=0.f;
			m_fBrake=fBrake;
			m_fBrake=aiClamp(m_fBrake,0.f,1.f);
		}
		if(fDistToTarget<2.5f && m_fDestSpeed<=0.f)
		{
			m_fThrottle=0.f;
			m_fBrake=1.f;
		}
		return;
	}

	// Check the intersection turns.
	if(m_pRoads[0] && (m_fRdAngle[0]<-_fTurnThreshold || _fTurnThreshold<m_fRdAngle[0]) &&
	   aiDist2(m_Car.GetMatrix().Pos,m_IntersectionPt[0])<CheckDistance(0))
	{
		float fTargetVel=(float)sqrt(_fFriction*19.8f*m_fRadius[0])*
						 m_fTurnVelFactor;

		// Set the brake and throttle
		float fBrake=0.f;
		if(m_Car.Speed()>fTargetVel)
		{
			Vector3 p=m_AltRoute[m_nBestRoute][0].Vtx-m_IntersectionPt[0];
			float fDistToTurn;
			if(m_bDir[0])
				fDistToTurn=aiDot(p,m_pRoads[0]->VertZDir(m_pRoads[0]->NumVerts()-1))-m_fTurnSetback[0];
			else
				fDistToTurn=aiDot(p,-m_pRoads[0]->VertZDir(0))-m_fTurnSetback[0];

			float fTargetTime=fDistToTurn/m_Car.Speed();

			fBrake=(m_Car.Speed()-fTargetVel)/(fTargetTime*_fFriction*19.8f);
		}

		if(fBrake>m_fBrakeThreshold && fBrake>m_fBrake)
		{
			m_fThrottle=0.f;
			m_fBrake=fBrake;
			m_fBrake=aiClamp(m_fBrake,0.f,1.f);
		}
	}
	else if(m_pRoads[1] && (m_fRdAngle[1]<-_fTurnThreshold || _fTurnThreshold<m_fRdAngle[1]) &&
			aiDist2(m_Car.GetMatrix().Pos,m_IntersectionPt[1])<CheckDistance(1))
	{
		float fTargetVel=(float)sqrt(_fFriction*19.8f*m_fRadius[1])*
						 m_fTurnVelFactor;

		// Set the brake and throttle
		float fBrake=0.f;
		if(m_Car.Speed()>fTargetVel)
		{
			float fDistToTurn=aiDist(m_AltRoute[m_nBestRoute][0].Vtx,m_IntersectionPt[1])-m_fTurnSetback[1];
				
			float fTargetTime=fDistToTurn/m_Car.Speed();
			fBrake=(m_Car.Speed()-fTargetVel)/(fTargetTime*_fFriction*19.8f);
		}

		if(fBrake>m_fBrakeThreshold && fBrake>m_fBrake)
		{
			m_fThrottle=0.f;
			m_fBrake=fBrake;
			m_fBrake=aiClamp(m_fBrake,0.f,1.f);
		}
	}
}

float aiNavigation::CheckDistance(int nId)
{
	return (m_fRoutePlanDist+m_fTurnSetback[nId])*(m_fRoutePlanDist+m_fTurnSetback[nId]);
}

void aiNavigation::SolveRoadTargetPoint()
{
	if(m_nWayPtIdx>=m_nNumWayPts)
		m_bFinished=true;
	else
		m_bFinished=false;

	// Plan out a rough route consisting of the next three road segs
	if(PlanRoute())
		return;

	// Process all the things that need to done when changing roads.
	if(m_pPrevRoad!=m_pRoads[0])
	{
		m_LPos=m_Car.GetMatrix().Pos;
		m_LPosition=m_Car.GetMatrix().Pos;
		m_pPrevRoad=m_pRoads[0];
		InitRoadTurns();
		CalcRoadTurns();
	}

	// Calc the exact route through the traffic
	CalcRoute();

	m_TargetPt=m_AltRoute[m_nBestRoute][1].Vtx;

	// Set the throttle and brake
	CalcSpeed();
}

int aiNavigation::PlanRoute()
{
	// Check if time to increment the destination intersection index
	if(m_nWayPtIdx<m_nNumWayPts && 
	  (m_nCurMapCompType==kIntersection && m_nCurMapCompIdx==CheckPt(m_nWayPtIdx)))
	{
		// Do this in case the road leading to this intersection is very short.
		// i.e. two intersections which are practically next to each other. What
		// happens is the opponents will go from one int to another without ever
		// registering on the conecting road.
		if(m_nWayPtIdx>0)
		{
			bool bDir;
			aiPath *pPath=AIMAP.DetRdSegBetweenInts(AIMAP.Intersection(CheckPt(m_nWayPtIdx-1)),
													AIMAP.Intersection(CheckPt(m_nWayPtIdx)),&bDir);
			if(m_pRoads[0]!=pPath)
				LocateWayPtFromRoad(pPath);
		}

		// Lets move on to the next intersection.
		m_nWayPtIdx++;
	}

	switch(m_nCurMapCompType)
	{
		case kRoad:
		{
			LocateWayPtFromRoad(AIMAP.Path(m_nCurMapCompIdx));
			break;
		}
		case kIntersection:
		{
			if(m_nRdIdx<3)
			{
				if(m_nDestMapCompType==kRoad)
				{
					if(m_nNumWayPts>0)
					{
						for(int i=0;i<AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->NumRoads();i++)
						{
							if(AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i)==
							   AIMAP.Path(m_nDestMapCompId))
							{
								m_pRoads[m_nRdIdx]=AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i);
								if(m_pRoads[m_nRdIdx]->Departure()==AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1]))
									m_bDir[m_nRdIdx]=1;
								else
									m_bDir[m_nRdIdx]=0;
								return 0;
							}
						}
					}
					else
					{
						for(int i=0;i<AIMAP.Intersection(m_nCurMapCompIdx)->NumRoads();i++)
						{
							if(AIMAP.Intersection(m_nCurMapCompIdx)->Road(i)==
							   AIMAP.Path(m_nDestMapCompId))
							{
								m_pRoads[m_nRdIdx]=AIMAP.Intersection(m_nCurMapCompIdx)->Road(i);
								if(m_pRoads[m_nRdIdx]->Departure()==AIMAP.Intersection(m_nCurMapCompIdx))
									m_bDir[m_nRdIdx]=1;
								else
									m_bDir[m_nRdIdx]=0;
								return 0;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

int aiNavigation::LocateWayPtFromRoad(aiPath *pCallLink)
{
	if(m_nWayPtIdx>=m_nNumWayPts)
	{
		m_pRoads[0]=pCallLink;
		m_pRoads[1]=m_pRoads[2]=0;

		int nIdx=m_pRoads[0]->RoadVertice(m_Car.GetMatrix().Pos,1,-1);
		nIdx=aiClamp(nIdx,0,m_pRoads[0]->NumVerts()-1);
		float fDir=aiDot(m_pRoads[0]->VertZDir(nIdx),m_Car.GetMatrix().ZDir);
		if(fDir>=0.f)
			m_bDir[0]=1;
		else
			m_bDir[0]=0;
		return true;
	}

	// Is the destination int the next goal? Handle missed intersections
	if(pCallLink->Destination()->Id()==CheckPt(m_nWayPtIdx+1) ||
	   pCallLink->Departure()->Id()==CheckPt(m_nWayPtIdx+1) ||
	   (m_nWayPtIdx==m_nNumWayPts-1 && m_nDestMapCompId==m_nCurMapCompIdx))
		m_nWayPtIdx++;

	// Is the destination int the current goal?
	if(pCallLink->Destination()->Id()==CheckPt(m_nWayPtIdx))
	{
		m_bDir[0]=1;
		m_pRoads[0]=pCallLink;

		m_nRdIdx=0;
		aiIntersection *pInt0, *pInt1, *pInt2;
		if(m_nWayPtIdx==m_nNumWayPts-1) 
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=0;
			pInt2=0;
			m_nRdIdx=1;
		}
		else if(m_nWayPtIdx==m_nNumWayPts-2)
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
			pInt2=0;
			m_nRdIdx=2;
		}
		else
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
			pInt2=AIMAP.Intersection(CheckPt(m_nWayPtIdx+2));
			m_nRdIdx=3;
		}

		m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
		m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt1,pInt2,&m_bDir[2]);

		if(m_nDestMapCompType==kRoad && 0<m_nRdIdx && m_nRdIdx<3)
		{
			for(int i=0;i<AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->NumRoads();i++)
			{
				if(AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i)==
				   AIMAP.Path(m_nDestMapCompId))
				{
					m_pRoads[m_nRdIdx]=AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i);
					if(m_pRoads[m_nRdIdx]->Departure()==
					   AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1]))
						m_bDir[m_nRdIdx]=1;
					else
						m_bDir[m_nRdIdx]=0;
					break;
				}
			}
		}
		return true;
	}

	// Check for wrong way down a one way road.
	if(pCallLink->Departure()->Id()==CheckPt(m_nWayPtIdx)) 
	{
		m_bDir[0]=0;
		m_pRoads[0]=pCallLink;

		m_nRdIdx=0;
		aiIntersection *pInt0, *pInt1, *pInt2;
		if(m_nWayPtIdx==m_nNumWayPts-1) 
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=0;
			pInt2=0;
			m_nRdIdx=1;
		}
		else if(m_nWayPtIdx==m_nNumWayPts-2)
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
			pInt2=0;
			m_nRdIdx=2;
		}
		else
		{
			pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
			pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
			pInt2=AIMAP.Intersection(CheckPt(m_nWayPtIdx+2));
			m_nRdIdx=3;
		}

		m_pRoads[1]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[1]);
		m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt1,pInt2,&m_bDir[2]);

		if(m_nDestMapCompType==kRoad && 0<m_nRdIdx && m_nRdIdx<3)
		{
			for(int i=0;i<AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->NumRoads();i++)
			{
				if(AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i)==
				   AIMAP.Path(m_nDestMapCompId))
				{
					m_pRoads[m_nRdIdx]=AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1])->Road(i);
					if(m_pRoads[m_nRdIdx]->Departure()==
					   AIMAP.Intersection(m_panWayPts[m_nNumWayPts-1]))
						m_bDir[m_nRdIdx]=1;
					else
						m_bDir[m_nRdIdx]=0;
					break;
				}
			}
		}
		return true;
	}

	// Check the destination intersection's intersections
	for(int ctr=0;ctr<pCallLink->Destination()->NumRoads();ctr++)
	{
		if(pCallLink->Destination()->Road(ctr)->Destination()->Id()==CheckPt(m_nWayPtIdx))
		{
			m_pRoads[0]=pCallLink;
			m_bDir[0]=1;
			m_pRoads[1]=pCallLink->Destination()->Road(ctr);
			m_bDir[1]=1;
			if(m_nWayPtIdx+1<m_nNumWayPts)
			{
				aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
				aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
			}
			else
				m_pRoads[2]=0;
			return true;
		}
		if(pCallLink->Destination()->Road(ctr)->Departure()->Id()==CheckPt(m_nWayPtIdx))
		{
			m_pRoads[0]=pCallLink;
			m_bDir[0]=1;
			m_pRoads[1]=pCallLink->Destination()->Road(ctr);
			m_bDir[1]=0;
			if(m_nWayPtIdx+1<m_nNumWayPts)
			{
				aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
				aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
			}
			else
				m_pRoads[2]=0;
			return true;
		}
	}
	
	// Check the departure intersection's intersections
	for(int ctr=0;ctr<pCallLink->Departure()->NumRoads();ctr++)
	{
		if(pCallLink->Departure()->Road(ctr)->Destination()->Id()==CheckPt(m_nWayPtIdx))
		{
			m_pRoads[0]=pCallLink;
			m_bDir[0]=0;
			m_pRoads[1]=pCallLink->Departure()->Road(ctr);
			m_bDir[1]=1;
			if(m_nWayPtIdx+1<m_nNumWayPts)
			{
				aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
				aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
			}
			else
				m_pRoads[2]=0;
			return true;
		}
		if(pCallLink->Departure()->Road(ctr)->Departure()->Id()==CheckPt(m_nWayPtIdx))
		{
			m_pRoads[0]=pCallLink;
			m_bDir[0]=0;
			m_pRoads[1]=pCallLink->Departure()->Road(ctr);
			m_bDir[1]=0;
			if(m_nWayPtIdx+1<m_nNumWayPts)
			{
				aiIntersection *pInt0=AIMAP.Intersection(CheckPt(m_nWayPtIdx));
				aiIntersection *pInt1=AIMAP.Intersection(CheckPt(m_nWayPtIdx+1));
				m_pRoads[2]=AIMAP.DetRdSegBetweenInts(pInt0,pInt1,&m_bDir[2]);
			}
			else
				m_pRoads[2]=0;
			return true;
		}
	}

	return false;
}

void aiNavigation::CalcRoute()
{
	// Initialize the route
	m_Route[0].Vtx=m_Car.GetMatrix().Pos;
	m_Route[0].Vtx.y+=1.f;
	m_Route[0].m_fCumAngle=0.f;
	m_Route[0].m_fCumDist=0.f;
	m_Route[0].m_nTurnId=0;

	if(!m_pRoads[0])
	{
		m_nBestRoute=0;
		m_AltRoute[m_nBestRoute][1].Vtx=m_Destination;
		m_AltRoute[m_nBestRoute][1].m_nTurnId=0;
		return;
	}

	m_Route[0].m_nSVIdx=m_pRoads[0]->RoadVertice(m_Car.GetMatrix().Pos,m_bDir[0],-1);
	if(m_Route[0].m_nSVIdx==m_pRoads[0]->NumVerts())
	{
		// Check if we're coming up to a sharp turn.
		if(m_pRoads[1])
		{
			m_Route[0].m_nSRd=1;
			m_Route[0].m_nSVIdx=m_pRoads[1]->RoadVertice(m_Car.GetMatrix().Pos,m_bDir[1],-1);
			if(m_Route[0].m_nSVIdx==m_pRoads[1]->NumVerts())
			{
				if(m_pRoads[2])
				{
					m_Route[0].m_nSRd=2;
					m_Route[0].m_nSVIdx=m_pRoads[2]->RoadVertice(m_Car.GetMatrix().Pos,m_bDir[2],-1);
				}
				else
				{
					m_Route[0].m_nSVIdx=m_pRoads[1]->NumVerts()-1;
				}
			}
		}
		else
		{
			m_Route[0].m_nSRd=0;
			m_Route[0].m_nSVIdx=m_pRoads[0]->NumVerts()-1;
		}
	}
	else
	{
		m_Route[0].m_nSRd=0;
	}

	// reset vars
	m_nNumRoutes=0;
	m_fTurnCenterPtOffset=0.f;
	for(int ctr=0;ctr<MAX_NUM_ROUTES;ctr++)
	{
		m_naOffRoadRoute[ctr]=0;
		m_naBlockedRoute[ctr]=0;
	}

	if(m_nWayPtIdx<m_nNumWayPts && !InSharpTurn(0))
	{
		m_Route[0].m_nType=kRoadTarget;
		m_LPosition=m_Car.GetMatrix().Pos;
	}
	else
	{
		m_Route[0].m_nType=kTurnTarget;
	}

	m_LPos=m_LPosition;
	CalcRoadTurns();
		
	// Plot a route for the next x meters.
	EnumRoutes(1);

	// Determine the best route. 
	DetermineBestRoute();
}

void aiNavigation::DetermineBestRoute()
{
	// - First, Pick the straightest route which stays on the road, and doesn't have any 
	//   blocked targets.
	// - Next, Pick the straightest route which stays on the road.
	// - Next, pick the straightest route.
	m_nBestRoute=-1;
	float fBestCumAngle=99999.f;

	for(int ctr=0;ctr<m_nNumRoutes;ctr++)
	{
		if(!m_naOffRoadRoute[ctr] && !m_naBlockedRoute[ctr] &&
		   m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle<fBestCumAngle)
		{
			fBestCumAngle=m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle;
			m_nBestRoute=ctr;
		}
	}

	if(m_nBestRoute<0)
	{
		fBestCumAngle=99999.f;
		for(int ctr=0;ctr<m_nNumRoutes;ctr++)
		{
			if(!m_naOffRoadRoute[ctr] && 
				m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle<fBestCumAngle)
			{
				fBestCumAngle=m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle;
				m_nBestRoute=ctr;
			}
		}
	}

	if(m_nBestRoute<0)
	{
		fBestCumAngle=99999.f;
		for(int ctr=0;ctr<m_nNumRoutes;ctr++)
		{
			if(m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle<fBestCumAngle)
			{
				fBestCumAngle=m_AltRoute[ctr][m_naNumRouteVerts[ctr]-1].m_fCumAngle;
				m_nBestRoute=ctr;
			}
		}
	}
}

void aiNavigation::EnumRoutes(int nRouteIdx)
{
	Vector3 Position=m_Route[nRouteIdx-1].Vtx;
	if(m_bFinished)		// Are we at the end of the route?
	{
		if(m_pRoads[m_Route[nRouteIdx-1].m_nSRd])
		{
			if(int nTurnId=InSharpTurn(nRouteIdx-1))
			{
				m_Route[nRouteIdx-1].m_nTurnId=nTurnId-1;
				int nWhichHalf=CalcSharpTurnTarget(nRouteIdx,nRouteIdx-1,Position);
				if(nWhichHalf)
				{
					float fTmpSideDist;
					m_Route[nRouteIdx].m_nTurnId++;
					m_LPos=m_Route[nRouteIdx].Vtx;
					if(m_pRoads[m_Route[nRouteIdx].m_nSRd] &&
					   m_pRoads[m_Route[nRouteIdx].m_nSRd]->IsPosOnRoad(m_Route[nRouteIdx].Vtx,2.f,&fTmpSideDist)>1)
						CalcDestinationTarget(nRouteIdx,Position);
				}
			}
			else
			{
				m_LPos=m_Route[nRouteIdx-1].Vtx;
				CalcDestinationTarget(nRouteIdx,Position);
				if(int nTurnId=InSharpTurn(nRouteIdx))
				{
					m_Route[nRouteIdx].m_nTurnId=nTurnId-1;
					CalcSharpTurnTarget(nRouteIdx,nRouteIdx,Position);
				}
			}
		}
		else
		{
			SetTargetPtToDestination(nRouteIdx);
		}
	}
	else
	{
		if(int nTurnId=InSharpTurn(nRouteIdx-1))
		{
			m_Route[nRouteIdx-1].m_nTurnId=nTurnId-1;
			CalcSharpTurnTarget(nRouteIdx,nRouteIdx-1,Position);
		}
		else
		{
			m_LPos=m_Route[nRouteIdx-1].Vtx;
			CalcRoadTarget(nRouteIdx,Position);
			if(int nTurnId=InSharpTurn(nRouteIdx))
			{
				m_Route[nRouteIdx].m_nTurnId=nTurnId-1;
				CalcSharpTurnTarget(nRouteIdx,nRouteIdx,Position);
			}
		}
	}

	// By Default All position calculated by the above on on the road
	m_Route[nRouteIdx].m_nOnRoad=1;

	// Now that we know where we want to go based on the road, lets check if there are
	// any obstacles in the way, and if the target is within the road boundaries.
	short *pnSRd=&m_Route[nRouteIdx-1].m_nSRd;
	short *pnSVIdx=&m_Route[nRouteIdx-1].m_nSVIdx;
	short *pnCRd=&m_Route[nRouteIdx].m_nSRd;
	short *pnCVIdx=&m_Route[nRouteIdx].m_nSVIdx;

	int nType;
	Vector3 ObsPos;
	aiObstacle *pObstacle;
	pObstacle=IsTargetBlocked(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx].Vtx,*pnSVIdx,
							  *pnSRd,*pnCVIdx+3,*pnCRd,VehicleLength()*2.f,&nType);
	if(pObstacle)
	{
		pObstacle->Position(ObsPos);
		if((nType!=kOppObstacle || (nType==kOppObstacle && m_nWayPtIdx>2 && m_Route[nRouteIdx-1].m_nType!=kTurnTarget)) &&
		   aiDist2(m_Route[nRouteIdx-1].Vtx,ObsPos)<m_fRoutePlanDist*m_fRoutePlanDist)
		{
			short Locations[10], TargetTypes[10];
			Vector3 Targets[10];
			aiObstacle *pAltObstacles[10];

			int bUseSidewalk=true;
			int nNumTargets=CalcObstacleAvoidPoints(pObstacle,nRouteIdx,bUseSidewalk,Targets,
													pAltObstacles,Locations,TargetTypes);
			for(int ctr=0;ctr<nNumTargets;ctr++)
				SaveTarget(nRouteIdx,Targets[ctr],pAltObstacles[ctr],Locations[ctr],nType,TargetTypes[ctr]);
		}
		else
		{
			ContinueCheck(nRouteIdx);
		}
	}
	else
	{
		ContinueCheck(nRouteIdx);
	}
}

// ASSUMPTION:
// Will always return at least one route when no acceptable targets exist, 
// which is the current road target .
int aiNavigation::CalcObstacleAvoidPoints(aiObstacle *pObstacle,int nRouteIdx,int bUseSidewalk,
												Vector3* pTargets,aiObstacle **ppAltObstacles,
												short* pLocations,short* pTargetTypes)
{
	int nNumTargets=0;
	if(m_Route[nRouteIdx-1].m_nType!=kTurnTarget)
	{
		float fZ;
		Vector3 p, LTarget, RTarget;

		short *pnSRd=&m_Route[nRouteIdx-1].m_nSRd;
		short *pnSVIdx=&m_Route[nRouteIdx-1].m_nSVIdx;

		if(nRouteIdx==1)
			pObstacle->PreAvoid(m_Route[nRouteIdx-1].Vtx,-m_Car.GetMatrix().ZDir,
								m_fRSideDist+2.f,LTarget,RTarget);
		else
			pObstacle->PreAvoid(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx,
								m_fRSideDist+2.f,LTarget,RTarget);
		
		// Now calc the left avoid points
		p=LTarget-m_Route[nRouteIdx-1].Vtx;
		if(nRouteIdx==1)
		{
			fZ=aiDot(p,-m_Car.GetMatrix().ZDir);
		}
		else
		{
			if(m_bDir[*pnSRd])
				fZ=aiDot(p,(-(m_pRoads[*pnSRd]->VertZDir(*pnSVIdx))));
			else
				fZ=aiDot(p,m_pRoads[*pnSRd]->VertZDir(m_pRoads[*pnSRd]->NumVerts()-*pnSVIdx-1));
		}

		if(fZ>=0.f)
		{
			int nNumTries=0;
			EnumTargets(LTarget,pObstacle,nRouteIdx,*pnSRd,*pnSVIdx,-1,bUseSidewalk,nNumTries,
						pTargets,ppAltObstacles,pLocations,pTargetTypes,&nNumTargets);
		}

		// Now calc the right avoid points
		p=RTarget-m_Route[nRouteIdx-1].Vtx;
		if(nRouteIdx==1)
		{
			fZ=aiDot(p,-m_Car.GetMatrix().ZDir);
		}
		else
		{
			if(m_bDir[*pnSRd])
				fZ=aiDot(p,(-(m_pRoads[*pnSRd]->VertZDir(m_pRoads[*pnSRd]->NumVerts()-*pnSVIdx-1))));
			else
				fZ=aiDot(p,m_pRoads[*pnSRd]->VertZDir(*pnSVIdx));
		}

		if(fZ>=0.f)
		{
			int nNumTries=0;
			EnumTargets(RTarget,pObstacle,nRouteIdx,*pnSRd,*pnSVIdx,1,bUseSidewalk,nNumTries,
						pTargets,ppAltObstacles,pLocations,pTargetTypes,&nNumTargets);
		}

		// Bailout: When no routes are found, ignore the obstacle and plan on braking.
		if(!nNumTargets)
		{
			pTargets[nNumTargets]=m_Route[nRouteIdx].Vtx;
			*ppAltObstacles=pObstacle;
			pTargetTypes[nNumTargets]=kBlockedTarget;
			nNumTargets=1;
		}

		return nNumTargets;
	}

	pTargets[nNumTargets]=m_Route[nRouteIdx].Vtx;
	pTargetTypes[nNumTargets]=kTurnTarget;
	*ppAltObstacles=pObstacle;
	nNumTargets=1;

	return nNumTargets;
}

void aiNavigation::EnumTargets(Vector3& OrigTarget,aiObstacle *pOrigObstacle,int nRouteIdx,
									 int nSRd,int nSVIdx,int nSide,int bUseSidewalk,int nNumTries,
									 Vector3* pTargets,aiObstacle **ppAltObstacles,short* pLocations,
									 short* pTargetTypes,int *pnNumTargets)
{
	nNumTries++;
	if(nNumTries==_nMaxTrys)
		return;

	int nCRdVtx;
	int nCRdIdx=pOrigObstacle->CurrentRoadIdx(m_pRoads,m_bDir,&nCRdVtx);
	if(nCRdIdx<0 || !m_pRoads[nCRdIdx])
		return;

	float fSideDist;
	int nOnRoad=m_pRoads[nCRdIdx]->IsPosOnRoad(OrigTarget,m_fRSideDist,&fSideDist);
	if(nOnRoad==1 || (bUseSidewalk && nOnRoad==2))
	{
		// Finally check if any other ambients block the route
		int nType;
		aiObstacle *pNewObstacle;
		pNewObstacle=IsTargetBlocked(m_Route[nRouteIdx-1].Vtx,OrigTarget,nSVIdx,nSRd,
									 nCRdVtx+2,nCRdIdx,VehicleLength()*2.f,&nType);
		if(pNewObstacle && nType!=kOppObstacle)
		{
			Vector3 Obs1Pos, Obs2Pos, LTarget, RTarget;
			if(nRouteIdx==1)
				pNewObstacle->PreAvoid(m_Route[nRouteIdx-1].Vtx,-m_Car.GetMatrix().ZDir,
									   m_fRSideDist+2.f,LTarget,RTarget);
			else
				pNewObstacle->PreAvoid(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx,
									   m_fRSideDist+2.f,LTarget,RTarget);

			// Check the distance between the two obstacles
			pOrigObstacle->Position(Obs1Pos);
			pNewObstacle->Position(Obs2Pos);
			float fSepDist=aiDist2(Obs1Pos,Obs2Pos);
			float fOrigObsDist=aiDist2(Obs1Pos,m_Car.GetMatrix().Pos);
			float fNewObsDist=aiDist2(Obs2Pos,m_Car.GetMatrix().Pos);
			if(fSepDist>225.f && fNewObsDist<fOrigObsDist)
			{
				float fZ;
				float fCheckDist=m_fRoutePlanDist*m_fRoutePlanDist;
				if(nSide==-1)
				{
					Vector3 p=RTarget-m_Route[nRouteIdx-1].Vtx;
					if(m_bDir[nSRd])
					{
						fZ=aiDot(p,(-(m_pRoads[nSRd]->VertZDir(nSVIdx))));
					}
					else
					{
						int nNumVerts=m_pRoads[nSRd]->NumVerts();
						fZ=aiDot(p,m_pRoads[nSRd]->VertZDir(nNumVerts-nSVIdx-1));
					}

					if(fZ>=0.f &&
					   aiDist2(m_Route[nRouteIdx-1].Vtx,Obs2Pos)<fCheckDist &&
					   nType==kAmbObstacle)
					{
						ppAltObstacles[*pnNumTargets]=pNewObstacle;
						pTargets[*pnNumTargets]=RTarget;
						pLocations[*pnNumTargets]=nOnRoad;
						pTargetTypes[*pnNumTargets]=kObstacleTarget;
						(*pnNumTargets)++;
					}
				}
				else
				{
					Vector3 p=LTarget-m_Route[nRouteIdx-1].Vtx;
					if(m_bDir[nSRd])
					{
						fZ=aiDot(p,(-(m_pRoads[nSRd]->VertZDir(nSVIdx))));
					}
					else
					{
						int nNumVerts=m_pRoads[nSRd]->NumVerts();
						fZ=aiDot(p,m_pRoads[nSRd]->VertZDir(nNumVerts-nSVIdx-1));
					}

					if(fZ>=0.f &&
					   aiDist2(m_Route[nRouteIdx-1].Vtx,Obs2Pos)<fCheckDist &&
					   nType==kAmbObstacle)
					{
						ppAltObstacles[*pnNumTargets]=pNewObstacle;
						pTargets[*pnNumTargets]=LTarget;
						pLocations[*pnNumTargets]=nOnRoad;
						pTargetTypes[*pnNumTargets]=kObstacleTarget;
						(*pnNumTargets)++;
					}
				}
			}

			// Now calc the specified avoid points.
			float fZ;
			if(nSide==-1)
			{
				Vector3 p=LTarget-m_Route[nRouteIdx-1].Vtx;
				if(m_bDir[nSRd])
				{
					fZ=aiDot(p,(-(m_pRoads[nSRd]->VertZDir(nSVIdx))));
				}
				else
				{
					int nNumVerts=m_pRoads[nSRd]->NumVerts();
					fZ=aiDot(p,m_pRoads[nSRd]->VertZDir(nNumVerts-nSVIdx-1));
				}

				if(fZ>=0.f)
				{
					EnumTargets(LTarget,pNewObstacle,nRouteIdx,nSRd,nSVIdx,-1,bUseSidewalk,nNumTries,
								pTargets,ppAltObstacles,pLocations,pTargetTypes,pnNumTargets);
				}
			}
			else
			{
				Vector3 p=RTarget-m_Route[nRouteIdx-1].Vtx;
				if(m_bDir[nSRd])
				{
					fZ=aiDot(p,(-(m_pRoads[nSRd]->VertZDir(nSVIdx))));
				}
				else
				{
					int nNumVerts=m_pRoads[nSRd]->NumVerts();
					fZ=aiDot(p,m_pRoads[nSRd]->VertZDir(nNumVerts-nSVIdx-1));
				}

				if(fZ>=0.f)
				{
					EnumTargets(RTarget,pNewObstacle,nRouteIdx,nSRd,nSVIdx,1,bUseSidewalk,nNumTries,
								pTargets,ppAltObstacles,pLocations,pTargetTypes,pnNumTargets);
				}
			}
		}
		else
		{
			// Save off this target.
			ppAltObstacles[*pnNumTargets]=pOrigObstacle;
			pTargets[*pnNumTargets]=OrigTarget;
			pLocations[*pnNumTargets]=nOnRoad;
			pTargetTypes[*pnNumTargets]=kObstacleTarget;
			(*pnNumTargets)++;
		}
	}
}

int aiNavigation::CalcSharpTurnTarget(int& nRouteIdx,int nLastIdx,Vector3& Position)
{
	Vector3 XDir, ZDir;
	if(m_bDir[m_Route[nLastIdx].m_nTurnId])
	{
		XDir=m_pRoads[m_Route[nLastIdx].m_nTurnId]->VertXDir(m_pRoads[m_Route[nLastIdx].m_nTurnId]->NumVerts()-1);
		ZDir=m_pRoads[m_Route[nLastIdx].m_nTurnId]->VertZDir(m_pRoads[m_Route[nLastIdx].m_nTurnId]->NumVerts()-1);
	}
	else
	{
		XDir=-m_pRoads[m_Route[nLastIdx].m_nTurnId]->VertXDir(0);
		ZDir=-m_pRoads[m_Route[nLastIdx].m_nTurnId]->VertZDir(0);
	}

	Vector3 StartPt=m_TurnCenterPt[m_Route[nLastIdx].m_nTurnId]+XDir*m_fTurn[m_Route[nLastIdx].m_nTurnId]*m_fRadius[m_Route[nLastIdx].m_nTurnId];

	Vector3 MyDir=m_Route[nRouteIdx-1].Vtx-m_TurnCenterPt[m_Route[nLastIdx].m_nTurnId];
	Vector3 StartPerp;
	StartPerp.Set(-m_TurnStartDir[m_Route[nLastIdx].m_nTurnId].z,
				  m_TurnStartDir[m_Route[nLastIdx].m_nTurnId].y,
				  m_TurnStartDir[m_Route[nLastIdx].m_nTurnId].x);
	float fX=aiDot(MyDir,StartPerp);
	float fZ=aiDot(MyDir,m_TurnStartDir[m_Route[nLastIdx].m_nTurnId]);
	float fMyAngle=atan2f(fX,fZ);
	m_Route[nRouteIdx].m_nTurnId=m_Route[nLastIdx].m_nTurnId;

	if(m_fTurn[m_Route[nLastIdx].m_nTurnId]<0.f)
		fMyAngle*=-1.f;
	
	if(fMyAngle<-0.1f)
	{
		m_Route[nRouteIdx].Vtx=StartPt;
		SaveTurnTarget(nRouteIdx,1);
		m_Route[nRouteIdx].m_nSRd=m_Route[nRouteIdx].m_nTurnId;
		m_Route[nRouteIdx].m_nSVIdx=m_pRoads[m_Route[nRouteIdx].m_nSRd]->NumVerts()-1;
		m_Route[nRouteIdx].m_nType=kRoadTarget;
		return 0;
	}
	else
	{
		float fCircumfrence=m_fRadius[m_Route[nLastIdx].m_nTurnId]*m_fRdAngle[m_Route[nLastIdx].m_nTurnId]*m_fTurn[m_Route[nLastIdx].m_nTurnId];
		int nNumSteps=(int)(fCircumfrence*.1f);
		nNumSteps+=(nNumSteps%2);	// Make always even
		if(nNumSteps<2)
			nNumSteps=2;

		float fStepAngle=(m_fRdAngle[m_Route[nLastIdx].m_nTurnId]*m_fTurn[m_Route[nLastIdx].m_nTurnId])/nNumSteps;
		for(int i=1;i<nNumSteps+1;i++)
		{
			if(fMyAngle<(fStepAngle*i-.5f*fStepAngle))
			{
				float fX=m_fRadius[m_Route[nLastIdx].m_nTurnId]*cosf(fStepAngle*i);
				float fZ=m_fRadius[m_Route[nLastIdx].m_nTurnId]*sinf(fStepAngle*i);
				Vector3 Target=m_TurnCenterPt[m_Route[nLastIdx].m_nTurnId]-ZDir*fZ+
							   XDir*m_fTurn[m_Route[nLastIdx].m_nTurnId]*fX;
				Vector3 p=Target-m_Route[nRouteIdx-1].Vtx;
				float fDist=(float)fabsf(aiDot(p,m_Car.GetMatrix().XDir));

				m_Route[nRouteIdx].Vtx=Target;

				SaveTurnTarget(nRouteIdx,0);
				if(i*fStepAngle<m_fRdAngle[m_Route[nLastIdx].m_nTurnId]*m_fTurn[m_Route[nLastIdx].m_nTurnId]*.5f)
				{
					m_Route[nRouteIdx].m_nSRd=m_Route[nLastIdx].m_nTurnId;
					m_Route[nRouteIdx].m_nSVIdx=m_pRoads[m_Route[nRouteIdx].m_nSRd]->NumVerts()-1;
				}
				else
				{
					m_Route[nRouteIdx].m_nSRd=m_Route[nLastIdx].m_nTurnId+1;
					int nRdId=m_Route[nRouteIdx].m_nSRd;
					m_Route[nRouteIdx].m_nSVIdx=m_pRoads[nRdId]->RoadVertice(m_Route[nRouteIdx].Vtx,m_bDir[nRdId],-1);
					if(m_Route[nRouteIdx].m_nSVIdx>=m_pRoads[nRdId]->NumVerts())
					{
						m_Route[nRouteIdx].m_nSRd++;
						m_Route[nRouteIdx].m_nSVIdx=0;
					}
		
					if(nRdId>=m_nRdIdx)
						m_bFinished=true;
				}

				m_Route[nRouteIdx].m_nTurnId=m_Route[nLastIdx].m_nTurnId;

				float fTmpSideDist;
				if(i*2>nNumSteps && 
				   m_pRoads[m_Route[nRouteIdx].m_nTurnId+1]->IsPosOnRoad(m_Route[nRouteIdx].Vtx,0.f,&fTmpSideDist)<3)
				{
					m_Route[nRouteIdx].m_nTurnId++;
					m_LPos=m_Route[nRouteIdx].Vtx;
					break;
				}

				if(i<nNumSteps)
					nRouteIdx++;
			}
		}
	}

	if(m_pRoads[m_Route[nRouteIdx].m_nSRd])
		m_Route[nRouteIdx].m_nSVIdx=aiClamp(m_Route[nRouteIdx].m_nSVIdx,0,
										    m_pRoads[m_Route[nRouteIdx].m_nSRd]->NumVerts()-1);
	return 1;
}

void aiNavigation::SaveTurnTarget(int nRouteIdx,int bDoAngleCalc)
{
	m_Route[nRouteIdx].m_nType=kTurnTarget;
	m_Route[nRouteIdx].Vtx.y+=1.f;
	m_Route[nRouteIdx].m_fCumDist=m_Route[nRouteIdx-1].m_fCumDist+
								  aiDist(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx].Vtx);
	if(bDoAngleCalc)
	{
		if(nRouteIdx==1)
		{
			Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Car.GetMatrix().Pos;
			m_Route[nRouteIdx].m_fCumAngle=(-m_Car.GetMatrix().ZDir).Angle(TargetDir);
		}
		else
		{
			Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Route[nRouteIdx-1].Vtx;
			Vector3 PTargetDir=m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx;
			m_Route[nRouteIdx].m_fCumAngle=m_Route[nRouteIdx-1].m_fCumAngle+
										   PTargetDir.Angle(TargetDir);
		}
	}
}

void aiNavigation::InitRoadTurns()
{
	// Calc the road angles
	for(int i=0;i<2;i++)
	{
		if(m_pRoads[i] && m_pRoads[i+1])
		{
			float fX, fZ;
			Vector3 TargetDir;
			int nNumVerts=m_pRoads[i]->NumVerts();
			int nNNumVerts=m_pRoads[i+1]->NumVerts();
			if(m_bDir[i+1])
				TargetDir=m_pRoads[i+1]->CenterVertice(1)-m_pRoads[i+1]->CenterVertice(0);
			else
				TargetDir=m_pRoads[i+1]->CenterVertice(nNNumVerts-2)-m_pRoads[i+1]->CenterVertice(nNNumVerts-1);

			if(m_bDir[i])
			{
				fX=aiDot(TargetDir,(-(m_pRoads[i]->VertXDir(nNumVerts-1))));
				fZ=aiDot(TargetDir,(-(m_pRoads[i]->VertZDir(nNumVerts-1))));
			}
			else
			{
				fX=aiDot(TargetDir,m_pRoads[i]->VertXDir(0));
				fZ=aiDot(TargetDir,m_pRoads[i]->VertZDir(0));
			}
			m_fRdAngle[i]=atan2f(fX,fZ);

			// Calc the direction of the turn
			if(m_fRdAngle[i]>=_fTurnThreshold)
				m_fTurn[i]=1.f;
			else if(m_fRdAngle[i]<=-_fTurnThreshold)
				m_fTurn[i]=-1.f;
			else
				m_fTurn[i]=0.f;
		}
		else
		{
			m_fRdAngle[i]=0.f;
			m_fTurn[i]=0.f;
		}
	}
}

void aiNavigation::CalcRoadTurns()
{
	for(int i=0;i<2;i++)
	{
		if(m_fTurn[i] && m_pRoads[i] && m_pRoads[i+1])
		{
			// Calc the intersection point of the current and next roads.
			float fWidth=CalcTurnIntersection(i);
		
			float fTheta=3.14f-fabsf(m_fRdAngle[i]);
			float fHalfTheta=fTheta*.5f;

			m_fRadius[i]=fWidth/(1.f-sinf(fHalfTheta));
			m_fTurnSetback[i]=m_fRadius[i]*cosf(fHalfTheta);

			if(m_bDir[i])
				m_TurnCenterPt[i]=m_IntersectionPt[i]+
								  m_pRoads[i]->VertZDir(m_pRoads[i]->NumVerts()-1)*m_fTurnSetback[i]-
								  m_pRoads[i]->VertXDir(m_pRoads[i]->NumVerts()-1)*m_fTurn[i]*(m_fRadius[i]-fWidth);
			else
				m_TurnCenterPt[i]=m_IntersectionPt[i]+
								  -m_pRoads[i]->VertZDir(0)*m_fTurnSetback[i]-
								  -m_pRoads[i]->VertXDir(0)*m_fTurn[i]*(m_fRadius[i]-fWidth);

			Vector3 XDir, ZDir;
			if(m_bDir[i])
			{
				XDir=m_pRoads[i]->VertXDir(m_pRoads[i]->NumVerts()-1);
				ZDir=m_pRoads[i]->VertZDir(m_pRoads[i]->NumVerts()-1);
			}
			else
			{
				XDir=-m_pRoads[i]->VertXDir(0);
				ZDir=-m_pRoads[i]->VertZDir(0);
			}

			Vector3 StartPt=m_TurnCenterPt[i]+XDir*m_fTurn[i]*m_fRadius[i];
			m_TurnStartDir[i]=StartPt-m_TurnCenterPt[i];
			m_TurnStartDir[i].y=0.f;
			m_TurnStartDir[i].Normalize();

			float fX=m_fRadius[i]*cosf(m_fRdAngle[i]*m_fTurn[i]);
			float fZ=m_fRadius[i]*sinf(m_fRdAngle[i]*m_fTurn[i]);
			Vector3 EndPt=m_TurnCenterPt[i]-ZDir*fZ+XDir*m_fTurn[i]*fX;
			m_TurnEndDir[i]=EndPt-m_TurnCenterPt[i];
			m_TurnEndDir[i].y=0.f;
			m_TurnEndDir[i].Normalize();
		}
	}
}

float aiNavigation::CalcTurnIntersection(int nWhichTurn)
{
	int nNumVerts=m_pRoads[nWhichTurn]->NumVerts();
	int nNNumVerts=m_pRoads[nWhichTurn+1]->NumVerts();

	// Calc the turn offsets. This could be moved away a little from the sidewalk
	float fCRdOffset=0.f;
	float fNRdOffset=0.f;

	float t;
	Vector3 P0, P1, Q0, Q1;
	if(m_fTurn[nWhichTurn]>0.f)	// right turn
	{
		int nVerts=0;
		if(m_bDir[nWhichTurn])
		{
			P0=m_pRoads[nWhichTurn]->RBoundary(nNumVerts-1+nVerts)+m_pRoads[nWhichTurn]->VertXDir(nNumVerts-1)*fCRdOffset; 
			P1=-(m_pRoads[nWhichTurn]->VertZDir(nNumVerts-1));
		}
		else
		{
			P0=m_pRoads[nWhichTurn]->LBoundary(0+nVerts)-m_pRoads[nWhichTurn]->VertXDir(0)*fCRdOffset; 
			P1=-(m_pRoads[nWhichTurn]->VertZDir(0));
		}

		if(m_bDir[nWhichTurn+1])
		{
			Q0=m_pRoads[nWhichTurn+1]->RBoundary(0+nVerts)+m_pRoads[nWhichTurn+1]->VertXDir(0)*fNRdOffset; 
			Q1=m_pRoads[nWhichTurn+1]->VertZDir(0);
			m_IntersectionPt[nWhichTurn].y=m_pRoads[nWhichTurn+1]->RBoundary(0).y;
		}
		else
		{
			Q0=m_pRoads[nWhichTurn+1]->LBoundary(nNNumVerts-1+nVerts)-m_pRoads[nWhichTurn+1]->VertXDir(nNNumVerts-1)*fNRdOffset; 
			Q1=m_pRoads[nWhichTurn+1]->VertZDir(nNNumVerts-1);
			m_IntersectionPt[nWhichTurn].y=m_pRoads[nWhichTurn+1]->LBoundary(nNNumVerts-1).y;
		}
	}
	else
	{
		int nVerts=0;
		if(m_bDir[nWhichTurn])
		{
			P0=m_pRoads[nWhichTurn]->LBoundary(nNumVerts-1+nVerts)-m_pRoads[nWhichTurn]->VertXDir(nNumVerts-1)*fCRdOffset;
			P1=-(m_pRoads[nWhichTurn]->VertZDir(nNumVerts-1));
		}
		else
		{
			P0=m_pRoads[nWhichTurn]->RBoundary(0+nVerts)+m_pRoads[nWhichTurn]->VertXDir(0)*fCRdOffset;
			P1=-(m_pRoads[nWhichTurn]->VertZDir(0));
		}

		if(m_bDir[nWhichTurn+1])
		{
			Q0=m_pRoads[nWhichTurn+1]->LBoundary(0+nVerts)-m_pRoads[nWhichTurn+1]->VertXDir(0)*fNRdOffset;
			Q1=-(m_pRoads[nWhichTurn+1]->VertZDir(0));
			m_IntersectionPt[nWhichTurn].y=m_pRoads[nWhichTurn+1]->LBoundary(0).y;
		}
		else
		{
			Q0=m_pRoads[nWhichTurn+1]->RBoundary(nNNumVerts-1+nVerts)+m_pRoads[nWhichTurn+1]->VertXDir(nNNumVerts-1)*fNRdOffset;
			Q1=m_pRoads[nWhichTurn+1]->VertZDir(nNNumVerts-1);
			m_IntersectionPt[nWhichTurn].y=m_pRoads[nWhichTurn+1]->RBoundary(nNNumVerts-1).y;
		}
	}

	t=(P1.x*Q0.z-P1.x*P0.z-P1.z*Q0.x+P1.z*P0.x)/(P1.z*Q1.x-P1.x*Q1.z);
	m_IntersectionPt[nWhichTurn].x=Q0.x+t*Q1.x;
	m_IntersectionPt[nWhichTurn].z=Q0.z+t*Q1.z;

	// Add 1.f to the width in order to make the turn grow as the opp
	// gets closer to the turn, but only if more than 10m to the turn.
	float fOffset=0.f;
	float fDist2=(m_fTurnSetback[nWhichTurn]+12.f)*(m_fTurnSetback[nWhichTurn]+12.f);
	if(aiDist2(m_LPos,m_IntersectionPt[nWhichTurn])>fDist2)
		fOffset=1.f;

	float fWidth;
	Vector3 p=m_LPos-m_IntersectionPt[nWhichTurn];
	if(m_bDir[nWhichTurn]==1)
		fWidth=m_fTurn[nWhichTurn]*aiDot(p,m_pRoads[nWhichTurn]->VertXDir(m_pRoads[nWhichTurn]->NumVerts()-1))+fOffset;
	else
		fWidth=m_fTurn[nWhichTurn]*aiDot(p,-m_pRoads[nWhichTurn]->VertXDir(0))+fOffset;

	return fWidth;
}

int aiNavigation::InSharpTurn(int nRouteIdx)
{
	int i;
	Vector3 MyDir;

	for(i=m_Route[nRouteIdx].m_nSRd;i<3;i++)
	{
		// Test the intersections
		if(i==0 || i==1 && m_Route[nRouteIdx].m_nTurnId==1 ||
		  (i==1 && -_fMaxStraightAngle<m_fRdAngle[0] && m_fRdAngle[0]<_fMaxStraightAngle))
		{
			float fRadius2=(m_fRadius[i]+15.f)*(m_fRadius[i]+15.f);
			if(m_fTurn[i] && m_pRoads[i] && m_pRoads[i+1] && aiDist2(m_Route[nRouteIdx].Vtx,m_TurnCenterPt[i])<fRadius2)
			{
				MyDir=m_Route[nRouteIdx].Vtx-m_TurnCenterPt[i];
				
				Vector3 StartPerp;
				StartPerp.Set(-m_TurnStartDir[i].z,m_TurnStartDir[i].y,m_TurnStartDir[i].x);
				float fSX=aiDot(MyDir,StartPerp*m_fTurn[i]);
				
				Vector3 EndPerp;
				EndPerp.Set(m_TurnEndDir[i].z,m_TurnEndDir[i].y,-m_TurnEndDir[i].x);
				float fEX=aiDot(MyDir,EndPerp*m_fTurn[i]);
				if(fSX>=-0.01f && fEX>=0.01f)
					return i+1;
			}
		}
	}

	return 0;
}

void aiNavigation::SaveTarget(int nRouteIdx,Vector3& Target,aiObstacle *pObstacle,short& Location,
									int nObsType,short& TargetType)
{
	int nNVIdx;
	int nCRd=pObstacle->CurrentRoadIdx(m_pRoads,m_bDir,&nNVIdx);
	if(!m_pRoads[nCRd])
	{
		nCRd--;
		if(nCRd<0)
			return;
		nNVIdx=m_pRoads[nCRd]->NumVerts()-1;
	}
	nNVIdx=aiClamp(nNVIdx,0,m_pRoads[nCRd]->NumVerts()-1);

	m_Route[nRouteIdx].m_nType=TargetType;
	m_Route[nRouteIdx].m_nObsType=nObsType;
	m_Route[nRouteIdx].m_pObstacle=pObstacle;
	m_Route[nRouteIdx].m_nOnRoad=Location;
	m_Route[nRouteIdx].Vtx=Target;
	m_Route[nRouteIdx].Vtx.y+=1.f;

	m_Route[nRouteIdx].m_fCumDist=m_Route[nRouteIdx-1].m_fCumDist+
								  aiDist(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx].Vtx);
	if(nRouteIdx==1)
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Car.GetMatrix().Pos;
		m_Route[nRouteIdx].m_fCumAngle=(-m_Car.GetMatrix().ZDir).Angle(TargetDir);
	}
	else
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Route[nRouteIdx-1].Vtx;
		Vector3 PTargetDir=m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx;
		m_Route[nRouteIdx].m_fCumAngle=m_Route[nRouteIdx-1].m_fCumAngle+
									   PTargetDir.Angle(TargetDir);
	}
	m_Route[nRouteIdx].m_nSVIdx=nNVIdx;
	m_Route[nRouteIdx].m_nSRd=nCRd;
	ContinueCheck(nRouteIdx);
}

void aiNavigation::ContinueCheck(int nRouteIdx)
{
	// Test if we're at the end of the route and are now ready to navigate to the destination
	// point.
	if(m_Route[nRouteIdx].m_fCumDist<m_fRoutePlanDist && 
	   m_pRoads[m_Route[nRouteIdx].m_nSRd] &&
	   (m_Route[nRouteIdx].m_nSRd==m_nRdIdx ||
	   m_Route[nRouteIdx].m_nSVIdx>=m_pRoads[m_Route[nRouteIdx].m_nSRd]->NumVerts()-1 &&
	   m_Route[nRouteIdx].m_nSRd+1<=2 && m_pRoads[m_Route[nRouteIdx].m_nSRd+1]==0) &&
	   m_nNumRoutes<10 &&
	   nRouteIdx<=_nMaxRouteSplits)
	{
		nRouteIdx++;
		m_bFinished=true;
		EnumRoutes(nRouteIdx);
	}
	else if(m_Route[nRouteIdx].m_fCumDist<m_fRoutePlanDist && m_pRoads[2] &&
		    !(m_Route[nRouteIdx].m_nSRd==2 && m_Route[nRouteIdx].m_nSVIdx==m_pRoads[2]->NumVerts()) &&
			m_nNumRoutes<10 &&
			nRouteIdx<=_nMaxRouteSplits)
	{
		nRouteIdx++;
		EnumRoutes(nRouteIdx);
	}
	else
	{
		// Save off all the routes to view as alternatives
		for(int i=0;i<=nRouteIdx;i++)
		{
			m_AltRoute[m_nNumRoutes][i]=m_Route[i];
			if(m_Route[i].m_nOnRoad==2)
				m_naOffRoadRoute[m_nNumRoutes]=1;
			if(m_Route[i].m_nType==kBlockedTarget)
				m_naBlockedRoute[m_nNumRoutes]=1;
		}
		m_naNumRouteVerts[m_nNumRoutes]=nRouteIdx+1;
		m_nNumRoutes++;
	}
}

void aiNavigation::CalcDestinationTarget(int nRouteIdx,Vector3& Position)
{
	Vector3 p, LTarget, RTarget;
	float fLX, fRX, fX, fZ, fLAngle, fRAngle, fLViewAngle, fRViewAngle;
	short nCRd, nLRd, nRRd, nCVIdx, nLVIdx, nRVIdx;

	nCRd=m_Route[nRouteIdx-1].m_nSRd;
	nCVIdx=m_Route[nRouteIdx-1].m_nSVIdx;
	int nNumVerts=m_pRoads[nCRd]->NumVerts();

	if(nRouteIdx>1 && m_Route[nRouteIdx-2].m_nType!=kTurnTarget)
	{
		nCVIdx++;
		if(nCVIdx==m_pRoads[nCRd]->NumVerts())
		{
			if(m_pRoads[nCRd+1])
			{
				nCVIdx=0;
				nCRd++;
				nNumVerts=m_pRoads[nCRd]->NumVerts();
			}
			else
				nCVIdx--;
		}
	}

	switch(m_nDestMapCompType)
	{
		case kRoad:
		{

			int nIndex=nNumVerts-nCVIdx;
			if(nIndex==nNumVerts)
				nIndex--;

			Position=m_Route[nRouteIdx-1].Vtx;

			if(m_bDir[nCRd])
			{
				// Init the left side
				if(m_pRoads[nCRd]->Divider()==1)	// Do this just for jersey barriers.
					LTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
							(m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fLSideDist+1.f);
				else 
					LTarget=m_pRoads[nCRd]->LBoundary(nCVIdx)-
							(m_pRoads[nCRd]->VertXDir(nCVIdx))*m_fLSideDist;
				p=LTarget-m_Route[nRouteIdx-1].Vtx;

				fLX=aiDot(p,(-(m_pRoads[nCRd]->VertXDir(nCVIdx))));
				fZ=aiDot(p,(-(m_pRoads[nCRd]->VertZDir(nCVIdx))));
				if(-0.01<fLX && fLX<0.01f)
					fLX=-1.f;
				fLViewAngle=atan2f(fLX,fZ);

				// Now init the right side
				RTarget=m_pRoads[nCRd]->RBoundary(nCVIdx)+
						(m_pRoads[nCRd]->VertXDir(nCVIdx))*m_fRSideDist;
				p=RTarget-m_Route[nRouteIdx-1].Vtx;

				fRX=aiDot(p,(-(m_pRoads[nCRd]->VertXDir(nCVIdx))));
				fZ=aiDot(p,(-(m_pRoads[nCRd]->VertZDir(nCVIdx))));
				if(-0.01<fRX && fRX<0.01f)
					fRX=1.f;
				fRViewAngle=atan2f(fRX,fZ);
			}
			else
			{
				Vector3 VDir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
							 m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1);
				VDir.Normalize();

				// Init the left side
				LTarget=m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1)+
						VDir*(m_fLSideDist+1.f);
				p=LTarget-m_Route[nRouteIdx-1].Vtx;

				fLX=aiDot(p,m_pRoads[nCRd]->VertXDir(nIndex));
				fZ=aiDot(p,m_pRoads[nCRd]->VertZDir(nIndex));

				if(fLX>0.f && nRouteIdx==1)
				{
					Position=m_Route[nRouteIdx-1].Vtx+m_pRoads[nCRd]->VertXDir(nIndex)*fLX;
					fLX=0.f;
				}
				if(fZ<1.f && nRouteIdx==1)fZ=1.f;
				fLViewAngle=atan2f(fLX,fZ);

				// Now init the right side
				RTarget=m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1)-
						VDir*(m_fRSideDist+1.f);
				p=RTarget-m_Route[nRouteIdx-1].Vtx;

				fRX=aiDot(p,m_pRoads[nCRd]->VertXDir(nIndex));
				fZ=aiDot(p,m_pRoads[nCRd]->VertZDir(nIndex));

				if(fRX<0.f && nRouteIdx==1)
				{
					Position=m_Route[nRouteIdx-1].Vtx-m_pRoads[nCRd]->VertXDir(nIndex)*fRX;
					fRX=0.f;
				}
				if(fZ<1.f && nRouteIdx==1)fZ=1.f;
				fRViewAngle=atan2f(fRX,fZ);
			}

			// Check see if we can see the destination.
			int bKnownDir=false;
			nLVIdx=nRVIdx=nCVIdx;
			nLRd=nRRd=nCRd;

			nCVIdx++;
			if(nCVIdx==m_pRoads[nCRd]->NumVerts())
			{
				if(m_pRoads[nCRd+1])
				{
					nCVIdx=0;
					nCRd++;
					nNumVerts=m_pRoads[nCRd]->NumVerts();
				}
				else
					nCVIdx--;
			}

			for(nCVIdx;nCVIdx<m_pRoads[nCRd]->NumVerts();nCVIdx++)
			{
				if(m_bDir[nCRd])
				{
					// First check the left side
					if(m_pRoads[nCRd]->Divider()==1)	// Do this just for jersey barriers.
						LTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
								(m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fLSideDist+1.f);
					else 
						LTarget=m_pRoads[nCRd]->LBoundary(nCVIdx)-
								(m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fLSideDist+1.f);
					p=LTarget-Position;

					if(!m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// reverse
					{
						int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx-1;
						if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
							nTmpIndex--;
						fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
						fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
					}
					else	// forward
					{
						fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
						fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
					}

					if(-0.01<fX && fX<0.01f)
						fX=-1.f;
					fLAngle=atan2f(fX,fZ);

					if(fLAngle>fRViewAngle)		// Road must be going right
					{
						bKnownDir=true;
						if(nRouteIdx>1 && nRRd==m_Route[nRouteIdx-1].m_nSRd && nRVIdx==m_Route[nRouteIdx-1].m_nSVIdx)
						{
							nRVIdx++;
							if(nRVIdx>m_pRoads[nRRd]->NumVerts()-1)
							{
								nRVIdx--;
							}
						}
						nCRd=nRRd;
						nCVIdx=nRVIdx;

						if(m_pRoads[nCRd]->Divider()==1)	// Do this just for jersey barriers.
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nCVIdx)+
												   (m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fRSideDist+1.f);
						else 
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->RBoundary(nCVIdx)+
												   (m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fRSideDist+1.f);
						break;
					}
					else if(fLAngle>fLViewAngle-.001f)	// Tighten up the left side of the view frustrum
					{
						fLViewAngle=fLAngle;
						nLVIdx=nCVIdx;
						nLRd=nCRd;
					}

					// Now check the right side
					RTarget=m_pRoads[nCRd]->RBoundary(nCVIdx)+
							(m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fRSideDist+1.f);
					p=RTarget-Position;
					
					if(!m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// reverse
					{
						int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx-1;
						if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
							nTmpIndex--;
						fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
						fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
					}
					else	// forward
					{
						fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
						fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
					}

					if(-0.01<fX && fX<0.01f)
						fX=1.f;
					fRAngle=atan2f(fX,fZ);

					if(fRAngle<fLViewAngle)	// Road must be going left
					{
						bKnownDir=true;
						if(nRouteIdx>1 && nLRd==m_Route[nRouteIdx-1].m_nSRd && nLVIdx==m_Route[nRouteIdx-1].m_nSVIdx)
						{
							nLVIdx++;
							if(nLVIdx>m_pRoads[nLRd]->NumVerts()-1)
							{
								nLVIdx--;
							}
						}
						nCRd=nLRd;
						nCVIdx=nLVIdx;

						if(m_pRoads[nCRd]->Divider()==1)	// Do this just for jersey barriers.
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
												   (m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fLSideDist+1.f);
						else 
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->LBoundary(nCVIdx)-
												   (m_pRoads[nCRd]->VertXDir(nCVIdx))*(m_fLSideDist+1.f);
						break;
					}
					else if(fRAngle<fRViewAngle+.001f)	// Tighten up the right side of the view frustrum
					{
						fRViewAngle=fRAngle;
						nRVIdx=nCVIdx;
						nRRd=nCRd;
					}
				}
				else
				{
					// First check the left side
					Vector3 VDir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
								 m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1);
					VDir.Normalize();

					LTarget=m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1)+
							VDir*(m_fLSideDist+1.f);
					p=LTarget-Position;

					if(!m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// reverse
					{
						int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx-1;
						if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
							nTmpIndex--;
						fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
						fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
					}
					else	// forward
					{
						fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
						fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
					}
					if(-0.01<fX && fX<0.01f)
						fX=-1.f;
					if(fZ<1.f)fZ=1.f;
					fLAngle=atan2f(fX,fZ);

					if(fLAngle>fRViewAngle)		// Road must be going right
					{
						bKnownDir=true;
						nCRd=nRRd;
						nCVIdx=nRVIdx;
						nNumVerts=m_pRoads[nCRd]->NumVerts();

						if(m_bDir[nCRd])
						{
							Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
										m_pRoads[nCRd]->RBoundary(nCVIdx);
							Dir.Normalize();
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->RBoundary(nCVIdx)+
												   Dir*(m_fRSideDist+1.f);
						}
						else
						{
							Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
										m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1);
							Dir.Normalize();
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1)+
													Dir*(m_fRSideDist+1.f);
						}
						break;
					}
					else if(fLAngle>fLViewAngle-.001f)	// Tighten up the left side of the view frustrum
					{
						fLViewAngle=fLAngle;
						nLVIdx=nCVIdx;
						nLRd=nCRd;
					}

					// Check the right side.
					VDir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
						 m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1);
					VDir.Normalize();

					RTarget=m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1)-
							(m_pRoads[nCRd]->VertXDir(nIndex))*(m_fRSideDist+1.f);
					p=RTarget-Position;
					
					if(!m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// reverse
					{
						int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx-1;
						if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
							nTmpIndex--;
						fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
						fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
					}
					else	// forward
					{
						fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
						fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
					}
					if(-0.01<fX && fX<0.01f)
						fX=1.f;
					if(fZ<1.f)fZ=1.f;
					fRAngle=atan2f(fX,fZ);

					if(fRAngle<fLViewAngle)	// Road must be going left
					{
						bKnownDir=true;
						nCRd=nLRd;
						nCVIdx=nLVIdx;
						nNumVerts=m_pRoads[nCRd]->NumVerts();

						if(m_bDir[nCRd])
						{
							Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
										m_pRoads[nCRd]->LBoundary(nCVIdx);
							Dir.Normalize();
							if(m_pRoads[nCRd]->Divider()==1)	// Do this just for jersey barriers.
								m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nCVIdx)+
													   Dir*(m_fLSideDist+1.f);
							else 
							{
								m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->LBoundary(nCVIdx)+
														Dir*(m_fLSideDist+1.f);
							}
						}
						else
						{
							Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
										m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1);
							Dir.Normalize();
							m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1)+
													Dir*(m_fLSideDist+1.f);
						}
						break;
					}
					else if(fRAngle<fRViewAngle+.001f)	// Tighten up the right side of the view frustrum
					{
						fRViewAngle=fRAngle;
						nRVIdx=nCVIdx;
						nRRd=nCRd;
					}
				}
			}

			if(nLVIdx<nRVIdx)
				nCVIdx=nLVIdx;
			else
				nCVIdx=nRVIdx;

			if(!bKnownDir)
			{
				if(!m_bDir[nRRd] && 
				   (nRRd<nCRd || nRVIdx<m_pRoads[nRRd]->NumVerts()-1))			// road goes to the right
				{
					int nRd=m_Route[nRouteIdx-1].m_nSRd;
					int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
					if(nVert<1)
						nVert=1;

					if(m_bDir[nRd])
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
						float fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
						m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->LBoundary(m_pRoads[nRRd]->NumVerts()-nRVIdx-1)+
											   m_pRoads[nRd]->VertXDir(nVert)*fMySide;
					}
					else
					{
						nNumVerts=m_pRoads[nRd]->NumVerts();
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nNumVerts-nVert-1);
						float fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nNumVerts-nVert));
						m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->LBoundary(m_pRoads[nRRd]->NumVerts()-nRVIdx-1)-
											   m_pRoads[nRd]->VertXDir(nNumVerts-nVert)*fMySide;
					}
					nCRd=nRRd;
					nCVIdx=nRVIdx;
					m_bFinished=false;
				}
				else if(!m_bDir[nLRd] && 
						(nLRd<nCRd || nLVIdx<m_pRoads[nLRd]->NumVerts()-1))	// road goes to the left
				{
					int nRd=m_Route[nRouteIdx-1].m_nSRd;
					int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
					if(nVert<1)
						nVert=1;
					if(m_bDir[nRd])
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
						float fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
						m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(m_pRoads[nLRd]->NumVerts()-nLVIdx-1)+
											   m_pRoads[nRd]->VertXDir(nVert)*fMySide;
					}
					else
					{
						nNumVerts=m_pRoads[nRd]->NumVerts();
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nNumVerts-nVert-1);
						float fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nNumVerts-nVert));
						m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(m_pRoads[nLRd]->NumVerts()-nLVIdx-1)-
											   m_pRoads[nRd]->VertXDir(nNumVerts-nVert)*fMySide;
					}
					nCRd=nLRd;
					nCVIdx=nLVIdx;
					m_bFinished=false;
				}

				else if(m_bDir[nRRd] && 
				   (nRRd<nCRd || nRVIdx<m_pRoads[nRRd]->NumVerts()-1))			// road goes to the right
				{
					float fMySide;
					int nRd=m_Route[nRouteIdx-1].m_nSRd;
					int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
					if(m_bDir[nRd])
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
						fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
					}
					else
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(m_pRoads[nRd]->NumVerts()-nVert-1);
						fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(m_pRoads[nRd]->NumVerts()-nVert-1));
					}
					if(fMySide<m_fRSideDist)
						fMySide=m_fRSideDist;

					m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->RBoundary(nRVIdx)+
										   m_pRoads[nRRd]->VertXDir(nRVIdx)*fMySide;

					nCRd=nRRd;
					nCVIdx=nRVIdx;
					m_bFinished=false;
				}
				else if(m_bDir[nLRd] && 
						(nLRd<nCRd || nLVIdx<m_pRoads[nLRd]->NumVerts()-1))	// road goes to the left
				{
					float fMySide;
					int nRd=m_Route[nRouteIdx-1].m_nSRd;
					int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
					if(m_bDir[nRd])
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nVert);
						fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nVert));
					}
					else
					{
						p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(m_pRoads[nRd]->NumVerts()-nVert-1);
						fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(m_pRoads[nRd]->NumVerts()-nVert-1));
					}
					if(fMySide<m_fLSideDist)
						fMySide=m_fLSideDist;

					m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(nLVIdx)-
										   m_pRoads[nLRd]->VertXDir(nLVIdx)*fMySide;
					nCRd=nLRd;
					nCVIdx=nLVIdx;
					m_bFinished=false;
				}
			}

			int nDestIdx=m_pRoads[nCRd]->RoadVertice(m_Destination,m_bDir[nCRd],-1);
			if(nCVIdx>=nDestIdx)
			{
				SetTargetPtToDestination(nRouteIdx);
				return;
			}

			m_Route[nRouteIdx].m_nType=kRoadTarget;
			m_Route[nRouteIdx].m_nSVIdx=nCVIdx;
			m_Route[nRouteIdx].m_nSRd=nCRd;
			m_Route[nRouteIdx].m_nTurnId=nCRd;
			m_Route[nRouteIdx].Vtx.y+=1.f;

			m_Route[nRouteIdx].m_fCumDist=m_Route[nRouteIdx-1].m_fCumDist+
										  aiDist(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx].Vtx);
			if(nRouteIdx==1)
			{
				Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Car.GetMatrix().Pos;
				m_Route[nRouteIdx].m_fCumAngle=(-m_Car.GetMatrix().ZDir).Angle(TargetDir);
			}
			else
			{
				Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Route[nRouteIdx-1].Vtx;
				Vector3 PTargetDir=m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx;
				m_Route[nRouteIdx].m_fCumAngle=m_Route[nRouteIdx-1].m_fCumAngle+
											   PTargetDir.Angle(TargetDir);
			}
			break;
		}
		case kIntersection:
		{
			SetTargetPtToDestination(nRouteIdx);
			return;
		}
	}
}

void aiNavigation::SetTargetPtToDestination(int nRouteIdx)
{
	if(m_fDestOffset==0.f)
		m_Route[nRouteIdx].Vtx=m_Destination;
	else
	{
		float fDist=aiDist(m_Route[nRouteIdx-1].Vtx,m_Destination);
		if(fDist<m_fDestOffset)
			m_Route[nRouteIdx].Vtx=m_Route[nRouteIdx-1].Vtx;
		else
			m_Route[nRouteIdx].Vtx.Lerp((fDist-m_fDestOffset)/fDist,
											m_Route[nRouteIdx-1].Vtx,m_Destination);
	}
	m_Route[nRouteIdx].Vtx.y+=1.f;
	m_Route[nRouteIdx].m_nType=m_Route[nRouteIdx-1].m_nType;
	m_Route[nRouteIdx].m_nSVIdx=m_Route[nRouteIdx-1].m_nSVIdx;
	m_Route[nRouteIdx].m_nSRd=m_Route[nRouteIdx-1].m_nSRd;
	m_Route[nRouteIdx].m_nTurnId=m_Route[nRouteIdx-1].m_nTurnId;

	m_Route[nRouteIdx].m_fCumDist=9999.f;
	if(nRouteIdx==1)
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Car.GetMatrix().Pos;
		m_Route[nRouteIdx].m_fCumAngle=(-m_Car.GetMatrix().ZDir).Angle(TargetDir);
	}
	else
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Route[nRouteIdx-1].Vtx;
		Vector3 PTargetDir=m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx;
		m_Route[nRouteIdx].m_fCumAngle=m_Route[nRouteIdx-1].m_fCumAngle+
									   PTargetDir.Angle(TargetDir);
	}
}

void aiNavigation::CalcRoadTarget(int nRouteIdx,Vector3& Position)
{
	Vector3 p, LTarget, RTarget;
	float fLX, fRX, fX, fZ, fLAngle, fRAngle;
	float fLViewAngle, fRViewAngle, fSideRatio, fSideDist;
	short nCRd, nLRd, nRRd, nCVIdx, nLVIdx, nRVIdx;
	int bKnownDir=false;

	nCRd=m_Route[nRouteIdx-1].m_nSRd;
	nCVIdx=m_Route[nRouteIdx-1].m_nSVIdx;
	int nNumVerts=m_pRoads[nCRd]->NumVerts();

	if(nRouteIdx>1 && m_Route[nRouteIdx-2].m_nType!=kTurnTarget)
	{
		nCVIdx++;
		if(nCVIdx>=m_pRoads[nCRd]->NumVerts())
		{
			if(m_pRoads[nCRd+1])
			{
				nCVIdx=0;
				nCRd++;
				nNumVerts=m_pRoads[nCRd]->NumVerts();
			}
			else
				nCVIdx--;
		}
	}

	int nIndex=nNumVerts-nCVIdx;
	if(nIndex==nNumVerts)
		nIndex--;

	Position=m_Route[nRouteIdx-1].Vtx;

	// Initialize the view angles.
	Vector3 VDir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
				 m_pRoads[nCRd]->RBoundary(nCVIdx);
	VDir.Normalize();

	// Determine which side of the road we are on.
	p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nCRd]->CenterVertice(nCVIdx);
	fSideDist=aiDot(p,-m_pRoads[nCRd]->VertXDir(nCVIdx));

	// Init the left side
	if(m_pRoads[nCRd]->Divider() && fSideDist>=0.f)	// Do this just for jersey barriers.
		LTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)-VDir*(m_fLSideDist+1.5f);
	else 
		LTarget=m_pRoads[nCRd]->LBoundary(nCVIdx)-VDir*(m_fLSideDist+1.f);
	p=LTarget-m_Route[nRouteIdx-1].Vtx;

	fLX=aiDot(p,(-(m_pRoads[nCRd]->VertXDir(nCVIdx))));
	fZ=aiDot(p,(-(m_pRoads[nCRd]->VertZDir(nCVIdx))));

	if(fZ<1.f && nRouteIdx==1)fZ=1.f;
	fLViewAngle=atan2f(fLX,fZ);

	// Now init the right side
	if(m_pRoads[nCRd]->Divider() && fSideDist<0.f)	// Do this just for jersey barriers.
		RTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)+VDir*(m_fRSideDist+1.5f);
	else 
		RTarget=m_pRoads[nCRd]->RBoundary(nCVIdx)+VDir*(m_fRSideDist+1.f);
	p=RTarget-m_Route[nRouteIdx-1].Vtx;

	fRX=aiDot(p,(-(m_pRoads[nCRd]->VertXDir(nCVIdx))));
	fZ=aiDot(p,(-(m_pRoads[nCRd]->VertZDir(nCVIdx))));

	if(fZ<1.f && nRouteIdx==1)fZ=1.f;
	fRViewAngle=atan2f(fRX,fZ);



	// Set the default target
	float fMaxSDist;
	fMaxSDist=m_pRoads[nCRd]->LaneWidth(m_pRoads[nCRd]->NumLanes(1)-1,1,1)-
			  m_fRSideDist-1.f;
	if(nRouteIdx==1)
	{
		if(nCVIdx>0)
		{	
			p=m_pRoads[nCRd]->CenterVertice(nCVIdx)-Position;
			m_fSideDist=aiDot(p,m_pRoads[nCRd]->VertXDir(nCVIdx));
			m_fSideDist=aiClamp(m_fSideDist,-fMaxSDist,fMaxSDist);
		}
		fSideDist=m_fSideDist;
	}
	else
	{
		p=m_pRoads[nCRd]->CenterVertice(nCVIdx)-Position;
		fSideDist=aiDot(p,m_pRoads[nCRd]->VertXDir(nCVIdx));
		fSideDist=aiClamp(fSideDist,-fMaxSDist,fMaxSDist);
	}
	fSideRatio=fSideDist/fMaxSDist;
	m_Route[nRouteIdx].Vtx.Lerp(fSideRatio,m_pRoads[nCRd]->CenterVertice(nNumVerts-1),
								m_pRoads[nCRd]->RBoundary(nNumVerts-1)+
								m_pRoads[nCRd]->VertXDir(nCVIdx)*(m_fRSideDist+1.f));


	// Check see if we can see the end of the road.
	for(nCVIdx;nCVIdx<m_pRoads[nCRd]->NumVerts();nCVIdx++)
	{
		// First check the left side
		Vector3 VDir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
					 m_pRoads[nCRd]->RBoundary(nCVIdx+nNumVerts);
		VDir.Normalize();

		if(m_pRoads[nCRd]->Divider() && fSideDist>=0.f)	// Do this just for jersey barriers.
			LTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)-VDir*(m_fLSideDist+1.5f);
		else 
			LTarget=m_pRoads[nCRd]->LBoundary(nCVIdx)-VDir*(m_fLSideDist+1.f);
		p=LTarget-Position;

		if(m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// forward
		{
			fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
			fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
		}
		else	// reverse
		{
			int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx;
			if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
				nTmpIndex--;
			fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
			fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
		}
		if(-0.01<fX && fX<0.01f)
			fX=-1.f;
		if(fZ<1.f)fZ=1.f;
		fLAngle=atan2f(fX,fZ);

		if(fLAngle>fRViewAngle)		// Road must be going right
		{
			bKnownDir=true;
			nCRd=nRRd;
			nCVIdx=nRVIdx;
			nNumVerts=m_pRoads[nCRd]->NumVerts();

			if(m_bDir[nCRd])
			{
				Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
							m_pRoads[nCRd]->RBoundary(nCVIdx);
				Dir.Normalize();
				if(m_pRoads[nCRd]->Divider() && fSideDist<0.f)	// Do this just for jersey barriers.
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nCVIdx)+Dir*(m_fRSideDist+1.5f);
				else 
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->RBoundary(nCVIdx)+
										   Dir*(m_fRSideDist+1.f);
				
				// if road is sharp turn right
				if(nCVIdx==m_pRoads[nCRd]->NumVerts()-1 && m_fRdAngle[nCRd]>_fTurnThreshold)
					m_Route[nRouteIdx].Vtx=m_IntersectionPt[nCRd];
			}
			else
			{
				Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
							m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1);
				Dir.Normalize();
				if(m_pRoads[nCRd]->Divider() && fSideDist<0.f)	// Do this just for jersey barriers.
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)+
											Dir*(m_fRSideDist+1.5f);
				else
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->LBoundary(nNumVerts-nCVIdx-1)+
											Dir*(m_fRSideDist+1.f);
			}
			break;
		}
		else if(fLAngle>fLViewAngle-.001f)	// Tighten up the left side of the view frustrum
		{
			fLViewAngle=fLAngle;
			nLVIdx=nCVIdx;
			nLRd=nCRd;
		}

		// Now check the right side
		VDir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
			 m_pRoads[nCRd]->RBoundary(nCVIdx);
		VDir.Normalize();

		if(m_pRoads[nCRd]->Divider() && fSideDist<0.f)	// Do this just for jersey barriers.
			RTarget=m_pRoads[nCRd]->CenterVertice(nCVIdx)+VDir*(m_fRSideDist+1.5f);
		else 
			RTarget=m_pRoads[nCRd]->RBoundary(nCVIdx)+VDir*(m_fRSideDist+1.f);
		p=RTarget-Position;
		
		if(m_bDir[m_Route[nRouteIdx-1].m_nSRd])	// forward
		{
			fX=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(m_Route[nRouteIdx-1].m_nSVIdx))));
			fZ=aiDot(p,(-(m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(m_Route[nRouteIdx-1].m_nSVIdx))));
		}
		else	// reverse
		{
			int nTmpIndex=m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts()-m_Route[nRouteIdx-1].m_nSVIdx;
			if(nTmpIndex==m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->NumVerts())
				nTmpIndex--;
			fX=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertXDir(nTmpIndex));
			fZ=aiDot(p,m_pRoads[m_Route[nRouteIdx-1].m_nSRd]->VertZDir(nTmpIndex));
		}

		if(-0.01<fX && fX<0.01f)
			fX=0.f;
		if(fZ<1.f)fZ=1.f;
		fRAngle=atan2f(fX,fZ);

		if(fRAngle<fLViewAngle)	// Road must be going left
		{
			bKnownDir=true;
			nCRd=nLRd;
			nCVIdx=nLVIdx;
			nNumVerts=m_pRoads[nCRd]->NumVerts();

			if(m_bDir[nCRd])
			{
				Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nCVIdx)-
							m_pRoads[nCRd]->LBoundary(nCVIdx);
				Dir.Normalize();
				if(m_pRoads[nCRd]->Divider() && fSideDist>=0.f)	// Do this just for jersey barriers.
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nCVIdx)+
											Dir*(m_fLSideDist+1.5f);
				else 
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->LBoundary(nCVIdx)+
											Dir*(m_fLSideDist+1.f);

				// if road is sharp turn left or right
				if(nCVIdx==m_pRoads[nCRd]->NumVerts()-1 && m_fRdAngle[nCRd]<-_fTurnThreshold)
					m_Route[nRouteIdx].Vtx=m_IntersectionPt[nCRd];
			}
			else
			{
				Vector3 Dir=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)-
							m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1);
				Dir.Normalize();
				if(m_pRoads[nCRd]->Divider() && fSideDist>=0.f)	// Do this just for jersey barriers.
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->CenterVertice(nNumVerts-nCVIdx-1)+
											Dir*(m_fLSideDist+1.5f);
				else
					m_Route[nRouteIdx].Vtx=m_pRoads[nCRd]->RBoundary(nNumVerts-nCVIdx-1)+
											Dir*(m_fLSideDist+1.f);
			}
			break;
		}
		else if(fRAngle<fRViewAngle+.001f)	// Tighten up the right side of the view frustrum
		{
			fRViewAngle=fRAngle;
			nRVIdx=nCVIdx;
			nRRd=nCRd;
		}

		// Check to see if we need to keep looking at the upcomming roads.
		if(nCVIdx==m_pRoads[nCRd]->NumVerts()-1)
		{
			// if road is sharp turn left or right
			if(nCRd<2)
			{
				if(m_fRdAngle[nCRd]<-_fTurnThreshold && nCVIdx==nLVIdx && nLRd==nCRd && 
					nCVIdx==nRVIdx && nRRd==nCRd)
				{
					if(nLVIdx==m_pRoads[nLRd]->NumVerts()-1)
					{
						// then set to the intersection point
						bKnownDir=true;
						m_Route[nRouteIdx].Vtx=m_IntersectionPt[nCRd];
					}
					break;
				}
				else if(_fTurnThreshold<m_fRdAngle[nCRd] && nCVIdx==nLVIdx && nLRd==nCRd && 
						nCVIdx==nRVIdx && nRRd==nCRd)
				{
					if(nRVIdx==m_pRoads[nRRd]->NumVerts()-1)
					{
						// then set to the intersection point
						bKnownDir=true;
						m_Route[nRouteIdx].Vtx=m_IntersectionPt[nCRd];
					}
					break;
				}
			}

			float fDist=aiDist(Position,m_Route[nRouteIdx].Vtx);
			if(m_Route[nRouteIdx-1].m_fCumDist+fDist<m_fRoutePlanDist && nCRd<2)
			{
				if(nCRd+1>=m_nRdIdx)
				{
					if(m_nDestMapCompType==kIntersection /*&& m_nDestMapCompId==m_nCurMapCompIdx*/)
					{
						SetTargetPtToDestination(nRouteIdx);
						return;
					}
					else
						m_bFinished=true;
					break;
				}
	
				if(m_pRoads[nCRd+1])
				{
					nCRd++;
					nCVIdx=-1;
					nNumVerts=m_pRoads[nCRd]->NumVerts();

					float fMaxSDist=m_pRoads[nCRd]->LaneWidth(m_pRoads[nCRd]->NumLanes(1)-1,1,1)-
									m_fRSideDist-1.f;
					if(!m_bDir[nCRd])	// reverse
					{
						p=m_pRoads[nCRd]->CenterVertice(nNumVerts-1)-m_Route[nRouteIdx].Vtx;
						float fMySDist=aiDot(p,-m_pRoads[nCRd]->VertXDir(nNumVerts-1));
						fMySDist=aiClamp(fMySDist,-fMaxSDist,fMaxSDist);
						float fSideRatio=fMySDist/fMaxSDist;

						m_Route[nRouteIdx].Vtx.Lerp(fSideRatio,m_pRoads[nCRd]->CenterVertice(0),
													m_pRoads[nCRd]->LBoundary(0)-
													m_pRoads[nCRd]->VertXDir(0)*(m_fRSideDist+1.5f));
					}
					else
					{
						p=m_pRoads[nCRd]->CenterVertice(0)-m_Route[nRouteIdx].Vtx;
						float fMySDist=aiDot(p,m_pRoads[nCRd]->VertXDir(0));
						fMySDist=aiClamp(fMySDist,-fMaxSDist,fMaxSDist);
						float fSideRatio=fMySDist/fMaxSDist;

						m_Route[nRouteIdx].Vtx.Lerp(fSideRatio,m_pRoads[nCRd]->CenterVertice(nNumVerts-1),
													m_pRoads[nCRd]->RBoundary(nNumVerts-1)+
													m_pRoads[nCRd]->VertXDir(nNumVerts-1)*(m_fRSideDist+1.5f));
					}
				}
				else
					break;
			}
			else
				break;
		}
	}

	if(!bKnownDir)
	{
		if(!m_bDir[nRRd] && 
		   (nRRd<nCRd || nRVIdx<m_pRoads[nRRd]->NumVerts()-1))			// road goes to the right
		{
			if(m_pRoads[nRRd]->Divider()==1 && fSideDist<0.f)	// Do this just for jersey barriers.
			{
				nNumVerts=m_pRoads[nRRd]->NumVerts();
				Vector3 Dir=m_pRoads[nRRd]->CenterVertice(nNumVerts-nRVIdx-1)-
							m_pRoads[nRRd]->RBoundary(nNumVerts-nRVIdx-1);
				Dir.Normalize();
				m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->CenterVertice(nNumVerts-nRVIdx-1)-Dir*(m_fRSideDist+1.5f);
			}
			else
			{
				int nRd=m_Route[nRouteIdx-1].m_nSRd;
				int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
				if(nVert<1)
					nVert=1;

				if(m_bDir[nRd])
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
					float fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
					m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->LBoundary(m_pRoads[nRRd]->NumVerts()-nRVIdx-1)+
										   m_pRoads[nRd]->VertXDir(nVert)*fMySide;
				}
				else
				{
					nNumVerts=m_pRoads[nRd]->NumVerts();
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nNumVerts-nVert-1);
					float fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nNumVerts-nVert));
					if(fMySide<0.f)
						fMySide=0.f;
					m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->LBoundary(m_pRoads[nRRd]->NumVerts()-nRVIdx-1)-
										   m_pRoads[nRd]->VertXDir(nNumVerts-nVert)*fMySide;
				}
			}
			nCRd=nRRd;
			nCVIdx=nRVIdx;
			m_bFinished=false;
		}
		else if(!m_bDir[nLRd] && 
				(nLRd<nCRd || nLVIdx<m_pRoads[nLRd]->NumVerts()-1))	// road goes to the left
		{
			if(m_pRoads[nLRd]->Divider()==1 && fSideDist>0.f)	// Do this just for jersey barriers.
			{
				nNumVerts=m_pRoads[nLRd]->NumVerts();
				Vector3 Dir=m_pRoads[nLRd]->CenterVertice(nNumVerts-nLVIdx-1)-
							m_pRoads[nLRd]->RBoundary(nNumVerts-nLVIdx-1);
				Dir.Normalize();
				m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->CenterVertice(nNumVerts-nLVIdx-1)+Dir*(m_fRSideDist+1.5f);
			}
			else
			{
				int nRd=m_Route[nRouteIdx-1].m_nSRd;
				int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
				if(nVert<1)
					nVert=1;
				if(m_bDir[nRd])
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
					float fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
					m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(m_pRoads[nLRd]->NumVerts()-nLVIdx-1)+
										   m_pRoads[nRd]->VertXDir(nVert)*fMySide;
				}
				else
				{
					nNumVerts=m_pRoads[nRd]->NumVerts();
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nNumVerts-nVert-1);
					float fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nNumVerts-nVert));
					m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(m_pRoads[nLRd]->NumVerts()-nLVIdx-1)-
										   m_pRoads[nRd]->VertXDir(nNumVerts-nVert)*fMySide;
				}
			}
			nCRd=nLRd;
			nCVIdx=nLVIdx;
			m_bFinished=false;
		}

		else if(m_bDir[nRRd] && 
		   (nRRd<nCRd || nRVIdx<m_pRoads[nRRd]->NumVerts()-1))			// road goes to the right
		{
			if(m_pRoads[nRRd]->Divider()==1 && fSideDist>=0.f)	// Do this just for jersey barriers.
			{
				Vector3 Dir=m_pRoads[nRRd]->CenterVertice(nRVIdx)-
							m_pRoads[nRRd]->RBoundary(nRVIdx);
				Dir.Normalize();
				m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->CenterVertice(nRVIdx)+Dir*(m_fRSideDist+1.5f);
			}
			else
			{
				float fMySide;
				int nRd=m_Route[nRouteIdx-1].m_nSRd;
				int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
				if(m_bDir[nRd])
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(nVert);
					fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(nVert));
				}
				else
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(m_pRoads[nRd]->NumVerts()-nVert-1);
					fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(m_pRoads[nRd]->NumVerts()-nVert-1));
				}
				if(fMySide<m_fRSideDist)
					fMySide=m_fRSideDist;

				m_Route[nRouteIdx].Vtx=m_pRoads[nRRd]->RBoundary(nRVIdx)+
									   m_pRoads[nRRd]->VertXDir(nRVIdx)*fMySide;
			}
			nCRd=nRRd;
			nCVIdx=nRVIdx;
			m_bFinished=false;
		}
		else if(m_bDir[nLRd] && 
				(nLRd<nCRd || nLVIdx<m_pRoads[nLRd]->NumVerts()-1))	// road goes to the left
		{
			if(m_pRoads[nLRd]->Divider()==1 && fSideDist>0.f)	// Do this just for jersey barriers.
				m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->CenterVertice(nLVIdx)-
									   m_pRoads[nLRd]->VertXDir(nLVIdx)*(m_fLSideDist+1.5f);
			else
			{
				float fMySide;
				int nRd=m_Route[nRouteIdx-1].m_nSRd;
				int nVert=m_Route[nRouteIdx-1].m_nSVIdx;
				if(m_bDir[nRd])
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->LBoundary(nVert);
					fMySide=aiDot(p,-m_pRoads[nRd]->VertXDir(nVert));
				}
				else
				{
					p=m_Route[nRouteIdx-1].Vtx-m_pRoads[nRd]->RBoundary(m_pRoads[nRd]->NumVerts()-nVert-1);
					fMySide=aiDot(p,m_pRoads[nRd]->VertXDir(m_pRoads[nRd]->NumVerts()-nVert-1));
				}
				if(fMySide<m_fLSideDist)
					fMySide=m_fLSideDist;

				m_Route[nRouteIdx].Vtx=m_pRoads[nLRd]->LBoundary(nLVIdx)-
									   m_pRoads[nLRd]->VertXDir(nLVIdx)*fMySide;
			}
			nCRd=nLRd;
			nCVIdx=nLVIdx;
			m_bFinished=false;
		}
	}

	m_Route[nRouteIdx].m_nType=kRoadTarget;
	m_Route[nRouteIdx].m_nSVIdx=nCVIdx;
	m_Route[nRouteIdx].m_nSRd=nCRd;
	m_Route[nRouteIdx].m_nTurnId=nCRd;
	m_Route[nRouteIdx].Vtx.y+=1.f;

	m_Route[nRouteIdx].m_fCumDist=m_Route[nRouteIdx-1].m_fCumDist+
								  aiDist(m_Route[nRouteIdx-1].Vtx,m_Route[nRouteIdx].Vtx);
	if(nRouteIdx==1)
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Car.GetMatrix().Pos;
		m_Route[nRouteIdx].m_fCumAngle=(-m_Car.GetMatrix().ZDir).Angle(TargetDir);
	}
	else
	{
		Vector3 TargetDir=m_Route[nRouteIdx].Vtx-m_Route[nRouteIdx-1].Vtx;
		Vector3 PTargetDir=m_Route[nRouteIdx-1].Vtx-m_Route[nRouteIdx-2].Vtx;
		m_Route[nRouteIdx].m_fCumAngle=m_Route[nRouteIdx-1].m_fCumAngle+
									   PTargetDir.Angle(TargetDir);
	}
}

aiObstacle* aiNavigation::IsTargetBlocked(Vector3& Pos,Vector3& Target,int nSVIdx,int nSRIdx,
											   int nTVIdx,int nTRIdx,float fMaxDist,int *pnType)
{
	if(m_pRoads[nTRIdx] && nTVIdx>m_pRoads[nTRIdx]->NumVerts())
	{
		nTRIdx++;
		nTVIdx=1;
	}

	// Check the ambient vehicles
	aiObstacle *pObstacle, *pOrigObstacle, *pTheObstacle=0;
	int nStartVert=nSVIdx;
	float fMinDist=99999.f;
	for(int ctr=nSRIdx;ctr<nTRIdx+1;ctr++)
	{
		if(m_pRoads[ctr])
		{
			for(int ctr1=nStartVert;ctr1<m_pRoads[ctr]->NumVerts();ctr1++)
			{
				// Check the vehicles on the right side
				if(m_bDir[ctr]==1)
				{
					if(ctr1==0)
						pObstacle=m_pRoads[ctr]->Departure()->Vehicles();
					else
						pObstacle=m_pRoads[ctr]->Vehicles(ctr1,1);
				}
				else
				{
					if(ctr1==0)
						pObstacle=m_pRoads[ctr]->Destination()->Vehicles();
					else
						pObstacle=m_pRoads[ctr]->Vehicles(m_pRoads[ctr]->NumVerts()-ctr1,1);
				}

				pOrigObstacle=pObstacle;
				while(pObstacle)
				{
					float fObsDist=pObstacle->IsBlockingTarget(Pos,Target,fMaxDist,VehicleWidth());
					if(fObsDist>-1.f)
					{
						if(fObsDist<fMinDist)
						{
							fMinDist=fObsDist;
							pTheObstacle=pObstacle;
							*pnType=kAmbObstacle;
						}
					}  
					pObstacle=pObstacle->NextObstacle();
					if(pOrigObstacle==pObstacle)
						break;
				}

				if(ctr1==0)	// The vehicle is in the interseciton
				{
				}
				else
				{
					// Check the left side vehicles
					if(m_bDir[ctr]==1)
						pObstacle=m_pRoads[ctr]->Vehicles(m_pRoads[ctr]->NumVerts()-ctr1,-1);
					else
						pObstacle=m_pRoads[ctr]->Vehicles(ctr1,-1);
					pOrigObstacle=pObstacle;
					while(pObstacle)
					{
						float fObsDist=pObstacle->IsBlockingTarget(Pos,Target,fMaxDist,VehicleWidth());
						if(fObsDist>-1.f)
						{
							if(fObsDist<fMinDist)
							{
								fMinDist=fObsDist;
								pTheObstacle=pObstacle;
								*pnType=kAmbObstacle;
							}
						}
						pObstacle=pObstacle->NextObstacle();
						if(pOrigObstacle==pObstacle)
							break;
					}
				}

				// Test for finished case.
				if((ctr1==nTVIdx && ctr==nTRIdx) || pTheObstacle)
				{

					ctr=nTRIdx;
					ctr1=nTVIdx;
					break;
				}
			}
			nStartVert=0;
		}
	}

	return pTheObstacle;
}

