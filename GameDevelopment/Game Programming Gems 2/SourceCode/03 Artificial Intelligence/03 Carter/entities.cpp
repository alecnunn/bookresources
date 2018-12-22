#include "standard_headers.hpp"
#include "entities.hpp"
#include "ai_brain.hpp"
#include "ai_behaviour_wander.hpp"
#include "ai_behaviour_hungry.hpp"
#include "ai_behaviour_sleep.hpp"
#include "ai_app.hpp"

//***************************************************************************************
CEntity::CEntity(CAIApp& app, int id, EEntityType type, const C2DCoordF& pos)
: App(app), Pos(pos), Type(type), Dead(false), Tiredness(0)
{
	PBrain = NULL;
	ID = id;
	Speed = 0.0f;
	switch(type)
	{
	case ENTITY_TROLL:
		Name = "TROLL";
		PBrain = new CAIBrain(*this);
		Speed = 0.02f;
		PBrain->AddBehaviour(10, new CAIBehaviourWander, "WANDER");
		PBrain->AddBehaviour(11, new CAIBehaviourHungry, "HUNGRY");
		PBrain->AddBehaviour(12, new CAIBehaviourSleep, "SLEEP");
		
		break;
	case ENTITY_FOOD:
		Name = "FOOD";
		break;
		
	}
}	

//***************************************************************************************
CEntity::~CEntity()
{
	if(PBrain!=NULL)
	{
		delete PBrain;
		PBrain = NULL;
	}
}


//***************************************************************************************
void CEntity::Update()
{
	if(PBrain!=NULL)
	{
		PBrain->Update();
	}

	Tiredness++;
}

//***************************************************************************************
void CEntity::Render()
{
	float x = (Pos.X*App.GetDisplayWidth())/App.GetMapWidth();
	float y = (Pos.Y*App.GetDisplayHeight())/App.GetMapHeight();

	char t[200];
	sprintf(t, "-%s", Name.c_str());
	if(PBrain)
	{
		strcat(t, "-");
		strcat(t, PBrain->GetCurrentBehaviourName().c_str());
	}
	App.GetFont().DrawText(x, y, 0xffffffff, t, 0);

}

//***************************************************************************************
CEntity* CEntity::FindNearestEntity(EEntityType type) const
{
	return(App.FindNearestEntity(type, Pos));
}

//***************************************************************************************
C2DCoordF CEntity::FindWanderPos(void) const 
{
	C2DCoordF pos;
	pos.X = (rand()*App.GetMapWidth())/RAND_MAX;
	pos.Y = (rand()*App.GetMapHeight())/RAND_MAX;

	return(pos);
}


//***************************************************************************************
bool	CEntity::MoveTowardsPos(const C2DCoordF& pos)
{		
	if(fabs(pos.X-Pos.X) < Speed)
	{
		Pos.X = pos.X;
	}
	else if(pos.X < Pos.X)
	{
		Pos.X-=Speed;
	}
	else if(pos.X > Pos.X)
	{
		Pos.X+=Speed;
	}

	if(fabs(pos.Y-Pos.Y) < Speed)
	{
		Pos.Y = pos.Y;
	}
	else if(pos.Y < Pos.Y)
	{
		Pos.Y-=Speed;
	}
	else if(pos.Y > Pos.Y)
	{
		Pos.Y+=Speed;
	}

	if(pos.X==Pos.X && pos.Y==Pos.Y)
	{
		return(true);
	}

	return(false);
	
}


