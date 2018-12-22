/********************************************************************
 *   Title: GameObjectItem.h
 *    Desc: Item in the game.  This class encapsulates all items
 *          in the game, from weapons to ammo to health boxes.
 *  Author: Cuban
 * Started: 
 *******************************************************************/

#ifndef GAMEOBJECTITEM
#define GAMEOBJECTITEM

#include "..\GameObject.h"
#include "..\ResourceMgr.h"

enum eItemType
{
	it_Health5,
	it_Health20,
	it_Health50,
	it_Ammo
};

class cGameObjectItem : public cGameObject  
{

public:
	cGameObjectItem();
	virtual ~cGameObjectItem();

	//==========--------------------------  Interface routines
	virtual bool		IsObject( eObjectType objType );

	//==========--------------------------  Scene-Management routines
	virtual void		AddToScene();
	virtual void		RemoveFromScene();

	//==========--------------------------  Processing functions
	virtual void		Tick( float tickLength );
	virtual void		Draw();

	//==========--------------------------  Object-Info functions
	
	virtual bool IsGhost(){ return false; }
	virtual bool IsFloater(){ return true; }
	virtual bool IsNoclip(){ return false; }

	virtual void		RespondToCollision( cGameObject* pOther );

	//static cGameObjectItem* CreateItem( 
};
#endif // GAMEOBJECTITEM
