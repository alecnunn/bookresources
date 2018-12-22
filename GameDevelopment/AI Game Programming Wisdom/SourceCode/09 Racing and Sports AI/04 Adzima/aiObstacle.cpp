#include "aiObstacle.h"
#include "aiMath.h"

aiObstacle::aiObstacle(float Front,float Back,float LSide,float RSide,Matrix34 *pMatrix)
{
	m_fFrontBumperDistance=Front;
	m_fBackBumperDistance=Back;
	m_fLSideDistance=LSide;
	m_fRSideDistance=RSide;

	m_pMatrix=pMatrix;	// Position and Orientation
}

bool aiObstacle:: IsBlockingTarget(Vector3& Pos,Vector3& Target,float fMaxDist,float fVWidth)
{
	Vector3 TargetDir=Target-Pos;
	TargetDir.y=0.f;
	TargetDir.Normalize();
	Vector3 TargetDirPerp;
	TargetDirPerp.Set(-TargetDir.z,TargetDir.y,TargetDir.x);
	float fTargetDist=aiDist(Pos,Target);

	// Check the four corners of the obstacle
	Vector3 Corner[4];
	Corner[0]=m_pMatrix->Pos-m_pMatrix->ZDir*m_fFrontBumperDistance-m_pMatrix->XDir*m_fLSideDistance;
	Corner[1]=m_pMatrix->Pos-m_pMatrix->ZDir*m_fFrontBumperDistance+m_pMatrix->XDir*m_fRSideDistance;
	Corner[2]=m_pMatrix->Pos+m_pMatrix->ZDir*m_fBackBumperDistance-m_pMatrix->XDir*m_fLSideDistance;
	Corner[3]=m_pMatrix->Pos+m_pMatrix->ZDir*m_fBackBumperDistance+m_pMatrix->XDir*m_fRSideDistance;

	float fVehicleHalfWidth=fVWidth*.5f+1.f;
	for(int i=0;i<4;i++)
	{
		Vector3 p=Corner[i]-Pos;
		float fSideDist=aiDot(p,TargetDirPerp);
		float fForwardDist=aiDot(p,TargetDir);
		if(fSideDist>-fVehicleHalfWidth && fSideDist<fVehicleHalfWidth &&
		    fForwardDist>0.f && fForwardDist<fTargetDist+fMaxDist)
		{
			return true;
		}
	}
	return false;
}

