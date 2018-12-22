#ifndef __CCMPPuzzleLogic_H
#define __CCMPPuzzleLogic_H

#include "ICmpPuzzleLogic.h"
#include <map>
#include <vector>
#include <set>

class CCmpPuzzleLogic : public ICmpPuzzleLogic
{
protected:
	// Forward declaration
	struct StateInfo;
public:
	CCmpPuzzleLogic();
	virtual ~CCmpPuzzleLogic();

	// Static methods
	static void			RegisterComponentType(void);
	static IComponent	*CreateMe();
	static bool			DestroyMe(IComponent *);

	// Virtual IComponent methods
	virtual bool		Init(CObjectId, CParameterNode &);
	virtual void		Deinit(void);
	virtual EMessageResult		HandleMessage(const CMessage &);
	virtual EComponentTypeId	GetComponentTypeId(void);

	// ICmpPuzzleLogic methods

	// Gets the current state
	virtual CHash	GetState();
	// Sets the state of this component. This will affect the description of the object (if there
	// is a description component). If silent is false, there will be feedback to the player that 
	// the state was changed
	virtual bool	SetState(CHash targetState, bool silent);
	// Sets a new state for this component if it can find a valid state transition that doesn't
	// require an object.
	virtual bool	Use();
	// Sets a new state for this component using object to help.
	virtual bool	UseWith(CObjectId userId, CHash object);

private:
	// Helper method for handling the MT_DESCRIBE_OBJECT message
	void DescribeObject(CObjectId viewer) const;
	// If the object has a description component, this method will set it according to stateName
	bool SetDescriptionForState(CHash stateName);
	// Adds a transition from fromState to toState (for the use command). If neededObject is valid
	// the transition will only happen if use A with neededObject is entered
	bool AddStateTransition(CHash fromState, CHash toState, CObjectId neededObject);
	// Adds description info about the given state
	bool AddStateInfo(CHash state, StateInfo &stateInfo);
	// Gets description info about the given state
	StateInfo *GetStateInfo(CHash state);

protected:
	struct StateInfo
	{
		char		mShortDesc[MAX_STR_LEN];
		char		mLongDesc[MAX_STR_LEN];
		char		mChangeToStateString[MAX_STR_LEN];
	};

	struct StateTransitionInfo
	{
		CHash		mEndState;
		CObjectId	mNeededObject;
	};

	typedef std::map<CHash, StateTransitionInfo>	StateTransitionMap;
	typedef std::map<CHash, StateInfo>	StateInfoMap;
	StateTransitionMap					mStateTransitionMap;
	StateInfoMap						mStateInfoMap;
	CHash								mCurrentState;
};

#endif //__CCMPPuzzleLogic_H