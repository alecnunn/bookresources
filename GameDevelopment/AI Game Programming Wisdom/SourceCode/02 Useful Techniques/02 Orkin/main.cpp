#include "TriggerSystem.h"
#include "Agent.h"
#include <windows.h>
#include <mmsystem.h>

CAgent* g_pAgentList[100];
unsigned long g_nNumAgents;

void Read();



void main()
{
	Read();

	CTriggerSystem myTriggerSystem;

	// The player constantly emits the EnemyNear trigger, by setting the duration to zero,
	// and setting the bDynamicSourcePos flag.
	myTriggerSystem.RegisterTrigger(kTrig_EnemyNear, 20, 0, g_pAgentList[0]->GetPosition(), 150.f, 0.f, true);


	bool bExplosion = false;
	bool bGunfire = false;
	bool bMovePlayer = false;

	unsigned long nCurTime;
	unsigned long nStartTime = timeGetTime();
	while(1)
	{
		nCurTime = timeGetTime();

		// After 4 seconds, end the program.
		if( nCurTime >= (nStartTime + 4000) )
		{
			break;
		}

		// After 3 seconds, the player moves closer to enemy2.
		else if( !bMovePlayer && (nCurTime >= (nStartTime + 3000)) )
		{
			bMovePlayer = true;
			printf("\nPlayer moves at 0:03\n");
			g_pAgentList[0]->SetPosition( Vector(300.f, 120.f, 100.f) );
		}

		// After 2 seconds, enemy2 fires his weapon.
		else if( !bGunfire && (nCurTime >= (nStartTime + 2000)) )
		{
			bGunfire = true;
			printf("\nGunfire by Enemy2 at 0:02\n");
			myTriggerSystem.RegisterTrigger(kTrig_Gunfire, 6, 2, g_pAgentList[2]->GetPosition(), 250.f, 0.4f, false);
		}

		// After 1 second, enemy3 causes an explosion.
		else if( !bExplosion && (nCurTime >= (nStartTime + 1000)) )
		{
			bExplosion = true;
			printf("\nExplosion by Enemy3 at 0:01\n");
			myTriggerSystem.RegisterTrigger(kTrig_Explosion, 10, 3, g_pAgentList[3]->GetPosition(), 300.f, 0.5f, false);
		}

		myTriggerSystem.Update();
	}
}

void Read()
{
	// Properties of the agents should really be read from external files.
	// Values are hardcoded here to simplify the sampel code.

	CAgent* agtPlayer = new(CAgent);
	CAgent* agtEnemy1 = new(CAgent);
	CAgent* agtEnemy2 = new(CAgent);
	CAgent* agtEnemy3 = new(CAgent);

	g_pAgentList[0] = agtPlayer;
	g_pAgentList[1] = agtEnemy1;
	g_pAgentList[2] = agtEnemy2;
	g_pAgentList[3] = agtEnemy3;

	g_nNumAgents = 4;

	unsigned long curTime = timeGetTime();

	agtPlayer->SetID(0);
	agtPlayer->SetPosition( Vector(100.f, 100.f, 100.f) );
	agtPlayer->SetNextTriggerUpdate( curTime + 100);
	agtPlayer->SetTriggerFlags(kTrig_None);

	agtEnemy1->SetID(1);
	agtEnemy1->SetPosition( Vector(200.f, 100.f, 100.f) );
	agtEnemy1->SetNextTriggerUpdate( curTime + 150);
	agtEnemy1->SetTriggerFlags(kTrig_Explosion | kTrig_Gunfire);

	agtEnemy2->SetID(2);
	agtEnemy2->SetPosition( Vector(300.f, 100.f, 100.f) );
	agtEnemy2->SetNextTriggerUpdate( curTime + 200);
	agtEnemy2->SetTriggerFlags(kTrig_Explosion | kTrig_Gunfire | kTrig_EnemyNear);

	agtEnemy3->SetID(3);
	agtEnemy3->SetPosition( Vector(400.f, 100.f, 100.f) );
	agtEnemy3->SetNextTriggerUpdate( curTime + 250);
	agtEnemy3->SetTriggerFlags(kTrig_Gunfire);
}
