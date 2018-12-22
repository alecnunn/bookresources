#ifndef __ENTITIES_HPP__
#define __ENTITIES_HPP__

class CAIBrain;
class CAIApp;

enum EEntityType
{
	ENTITY_TROLL=1,
	ENTITY_FOOD,

};

#include "base_class.hpp"
//In a real app we would sub class different types of entity.
class CEntity : public CBaseObject
{
protected:
	int				ID;
	CAIBrain*		PBrain;
	std::string		Name;
	CAIApp&			App;
	C2DCoordF		Pos;   
	float			Speed;
	EEntityType		Type;
	bool			Dead; 
	signed int		Tiredness;

protected:

public:
	 CEntity(CAIApp& app, int id, EEntityType type, const C2DCoordF& pos);
	 ~CEntity();
		 
	 void Update(void);
	 void Render();

	 CEntity*	FindNearestEntity(EEntityType type) const;
	 C2DCoordF	FindWanderPos(void) const;
	 bool		MoveTowardsPos(const C2DCoordF& pos);

	 const std::string&	GetName() const	{ return(Name); };
	 const C2DCoordF&	GetPos() const	{ return(Pos); }
	 int				GetID() const	{ return(ID); };
	 EEntityType		GetType() const	{ return(Type); };
	 void				Kill()			{ Dead = true; };
	 bool				IsDead() const	{ return(Dead); };

	 int				GetTiredness() const {return(Tiredness); };
	 void				Sleep(int amount) { Tiredness-=amount; if(Tiredness<0) Tiredness=0; }; 
};

#endif //__ENTITIES_HPP__