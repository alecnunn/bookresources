#ifndef AI_OBSTACLE_H
#define AI_OBSTACLE_H

class Vector3;
class Matrix34;
class aiPath;

class aiObstacle
{
	public:
		aiObstacle(float Front,float Back,float LSide,float RSide,Matrix34 *pMatrix);
		~aiObstacle();

		bool IsBlockingTarget(Vector3& Pos,Vector3& Target,float fMaxDist,float fVWidth);
		void PreAvoid(const Vector3& Pos,const Vector3& Heading,float fVehHWidth,Vector3& LTarget,Vector3& RTarget);
		int  CurrentRoadIdx(aiPath **ppRoads,bool baDir[3],int *pnVIdx);

		void Position(Vector3& Pos);
		aiObstacle* NextObstacle();	// return the next obstacle in the current chain.

	protected:
		Matrix34 *m_pMatrix;

		float m_fFrontBumperDistance;
		float m_fBackBumperDistance;
		float m_fLSideDistance;
		float m_fRSideDistance;

		aiObstacle *m_pNObstacle;
};

#endif
