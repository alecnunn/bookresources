#ifndef AI_NAVIGATION_H
#define AI_NAVIGATION_H

#include "aiMath.h"
#include "aiVehicle.h"

class aiPath;
class aiObstacle;

struct aiRouteNode
{
	aiRouteNode();
	void Reset();
	aiObstacle *m_pObstacle;
	Vector3 Vtx;
	float m_fCumAngle;
	float m_fCumDist;

	short m_nSRd;
	short m_nSVIdx;
	short m_nType;
	short m_nTurnId;
	short m_nObsType;
	short m_nOnRoad;	// 0: Off the road, 1: On the road, 2: On the sidewalk
};

class aiNavigation 
{
	public:
		aiNavigation();
		~aiNavigation();

		void Init(int nId,char *pGeoFile);
		void Reset();
		void RegisterRoute(short *panRoute,short nNumRouteNodes,const Vector3& Destination,
						   const Vector3& DestHeading,int nNumRepeats=0,float fTargetSpeed=0.f,
						   float fDestOffset=0.f,float fTurnVelFactor=1.f,float fBrakeThreshold=.7f,
						   float fRoutePlanDist=75.f);

		void DriveRoute();

		int  State(){return m_nState;}
		void State(int nState){m_nState=nState;}

		float FrontBumperDistance(){return m_fFrontBumperDistance;}
		float BackBumperDistance(){return m_fBackBumperDistance;}
		float LSideDistance(){return m_fLSideDist;}
		float RSideDistance(){return m_fRSideDist;}
		float VehicleWidth(){return m_fLSideDist+m_fRSideDist;}
		float VehicleLength(){return m_fBackBumperDistance+m_fFrontBumperDistance;}

		short CheckPt(int nIdx);
	
		enum States{kForward,kBackup,kShortcut,kStop};
		enum TargetTypes{kRoadTarget, kTurnTarget, kObstacleTarget, kBlockedTarget,};
		enum ObstacleTypes{kPlayerObstacle,kAmbObstacle,kOppObstacle,kCopObstacle,kPedObstacle,kBangerObstacle};

	private:
		int LocateWayPtFromRoad(aiPath *pCallLink);
		int LocateWayPtFromInt(int nId);

		void SolveRoadTargetPoint();
		void SolveShortcutTargetPoint();
		int  PlanRoute();
		void CalcRoute();
		void CalcRoadTarget(int nRouteIdx,Vector3& Position);
		void CalcDestinationTarget(int nRouteIdx,Vector3& Position);
		void DestMapComponent(const Vector3& Pos,short *pnDestMapCompId,short *pnDestMapCompType);
		void EnumRoutes(int nRouteIdx);
		void ContinueCheck(int nRouteIdx);
		void SaveTurnTarget(int nRouteIdx,int bDoAngleCalc);
		void SaveTarget(int nRouteIdx,Vector3& Target,aiObstacle *pObstacle,short& Location,
						int nObsType,short& TargetType);
		aiObstacle* IsTargetBlocked(Vector3& Pos,Vector3& Target,int nSVIdx,int nSRIdx,
								   int nTVIdx,int nTRIdx,float fMaxDist,int *pnType);
		int CalcObstacleAvoidPoints(aiObstacle *pObstacle,int nRouteIdx,int bUseSidewalk,
									Vector3 *pTargets,aiObstacle **ppAltObstacles,short* pLocations,
									short* pTargetTypes);
		void EnumTargets(Vector3& OrigTarget,aiObstacle *pOrigObstacle,int nRouteIdx,
						 int nSRd,int nSVIdx,int nSide,int bUseSidewalk,int nNumTries,
						 Vector3* pTargets,aiObstacle **ppAltObstacles,short* pLocations,
						 short* pTargetTypes,int *pnNumTargets);
		void DetermineBestRoute();
	
		void  CalcSpeed();
		void  CalcRoadSpeed(); 
		float CheckDistance(int nWhichTurn);

		void  InitRoadTurns();
		void  CalcRoadTurns();
		float CalcTurnIntersection(int nWhichTurn);

		void CheckForShortcut();
		int  InSharpTurn(int nRouteIdx);
		int  CalcSharpTurnTarget(int& nRouteIdx,int nLastIdx,Vector3& Position);

		void Stop();
		void InitShortcut();
		void Shortcut();
		void InitBackup();
		void Backup();
		void FinishedBackingUp();
		void SetTargetPtToDestination(int nRouteIdx);

		void InitForward();
		void Forward();

		Vehicle m_Car;
		bool m_bDir[3];
		aiPath* m_pRoads[3];
		aiPath* m_pPrevRoad;

		short m_nState;
		short m_nLastState;

		enum { MAX_ROUTE_NODES = 40 };
		enum { MAX_NUM_ROUTES = 25 };

		aiRouteNode m_Route[MAX_ROUTE_NODES];
		aiRouteNode m_AltRoute[MAX_NUM_ROUTES][MAX_ROUTE_NODES];
		int m_nBestRoute;
		int m_naBlockedRoute[MAX_NUM_ROUTES];
		int m_naOffRoadRoute[MAX_NUM_ROUTES];
		int m_naNumRouteVerts[MAX_NUM_ROUTES];
		int m_nNumRoutes;

		Vector3 m_Destination;
		Vector3 m_DestHeading;
		short m_nDestMapCompType;
		short m_nDestMapCompId;
		short m_bFinished;

		Vector3 m_TargetPt;
		short *m_panWayPts;
		short m_nNumWayPts;
		short m_nWayPtIdx;
		short m_nCurMapCompType;
		short m_nCurMapCompIdx;
		short m_nBackupCount;
		short m_nRdIdx;

		float m_fBrake;
		float m_fThrottle;
		float m_fSteering; 
		float m_fTurnCenterPtOffset;
		float m_fRdAngle[2];
		float m_fTurn[2];
		Vector3 m_LPos;
		Vector3 m_LPosition;
		Vector3 m_IntersectionPt[2];
		Vector3 m_TurnCenterPt[2];
		Vector3 m_TurnStartDir[2];
		Vector3 m_TurnEndDir[2];
		float m_fTurnSetback[2];
		float m_fRadius[2];

		float m_fSideDist;
		float m_fDestSpeed;
		float m_fDestOffset;

		float m_fFrontBumperDistance;
		float m_fBackBumperDistance;
		float m_fLSideDist;
		float m_fRSideDist;

		float m_fTurnVelFactor;
		float m_fBrakeThreshold;
		float m_fRoutePlanDist;
};

#endif
