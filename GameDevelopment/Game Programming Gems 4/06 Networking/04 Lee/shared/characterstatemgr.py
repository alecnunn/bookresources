"""
characterstatemgr.py - CharacterStateMgr class
"""

import shared
import shared.characterstatedata as _characterstatedata
import shared.moveforward as _moveforward
import shared.moveleft as _moveleft
import shared.movebackward as _movebackward
import shared.moveright as _moveright
import shared.stopped as _stopped
import shared.standing as _standing
import shared.sitting as _sitting
import shared.layingdown as _layingdown
import shared.dead as _dead
import shared.idle as _idle
import shared.casting as _casting
import shared.fighting as _fighting
import shared.crafting as _crafting
import shared.gesturing as _gesturing
import string

"""
CharacterStateMgr - implements 3 parallel-state machines for controlling the
                    visual appearance of in-game actors in clients while
                    allowing management of game play on the server
"""

class CharacterStateMgr:

  def __init__(self):
    """
    Initialize the three state machines to the default states, set
    up the dictionary to track blocked states
    """
    self.__stateMachineDict = {}
    self.__stateMachineDict[_characterstatedata.POSTURE]  = _standing
    self.__stateMachineDict[_characterstatedata.MOVEMENT] = _stopped
    self.__stateMachineDict[_characterstatedata.ACTION]   = _idle
    self.__blockedStateDict = {}


  def _Transition(self, newState):
    """
    Transitions to the specified newState by first exiting the current
    state on the state machine that newState belongs to.  Make newState the
    current state and call its setup method
    """
    assert self.__stateMachineDict.has_key(newState.GetStateTypeId()), "Error: Unknown state type"
    
    self.__stateMachineDict[newState.GetStateTypeId()].OnExitState(self)
    self.__stateMachineDict[newState.GetStateTypeId()] = newState
    self.__stateMachineDict[newState.GetStateTypeId()].OnEnterState(self)

    
  def RequestTransition(self, state):
    """
    Transitions to the given state, while honoring any blocks in place.
    Returns 1 if sucessfully transtioned, 0 otherwise
    """
    if not self.CanTransitionTo(state):
      return 0

    self._Transition(state)
    return 1
  
    
  def TransitionTo(self, state):
    """
    Force transtion into the specified state, ignoring any blocks. Use
    with care.
    """
    self._Transition(state)    


  def GetAllStateIds(self):
    """
    Returns the ids of the current states in each layer as a tuple
    """
    return (self.GetPostureState().GetStateId(),
            self.GetMovementState().GetStateId(),
            self.GetActionState().GetStateId())


  def SetAllStateIds(self, stateIdList):
    """
    Sets each of the states based on the list of ids in stateIdList
    """
    for stateId in stateIdList:
      state = shared._GetStateModuleById(stateId)
      self.TransitionTo(state)    


  def ForceDefaultStates(self):
    """
    Forces the actor back to the starting states when created,
    where they are doing "nothing"
    """
    self.TransitionTo(_standing)
    self.TransitionTo(_stopped)
    self.TransitionTo(_idle)

      
  def CanTransitionTo(self, state):
    """
    Returns whether a transition to the specified state is blocked or not.
    """
    return self.IsBlocked(state) == 0


  def _GetCurrentState(self, stateTypeId):
    """
    Return the current state for the specified stateTypeId (layer)
    """
    assert self.__stateMachineDict.has_key(stateTypeId), "Error: Unknown state type"
    return self.__stateMachineDict[stateTypeId]


  def GetPostureState(self):
    """
    Returns current posture state
    """
    return self.__stateMachineDict[_characterstatedata.POSTURE]


  def GetMovementState(self):
    """
    Returns the current movement state
    """
    return self.__stateMachineDict[_characterstatedata.MOVEMENT]


  def GetActionState(self):
    """
    Returns the current action state
    """
    return self.__stateMachineDict[_characterstatedata.ACTION]
  

  def BlockState(self, state):
    """
    Blocks transitions to the specified state
    """
    self.__blockedStateDict[state] = self.__blockedStateDict.get(state, 0) + 1
    

  def UnblockState(self, state):
    """
    removes a block on the specified state
    """
    self.__blockedStateDict[state] = self.__blockedStateDict[state] - 1
    assert self.__blockedStateDict[state] >= 0, "Tried to unblock a state that was not previously blocked"    


  def IsBlocked(self, state):
    """
    Returns whether the specified state is blocked
    """
    return self.__blockedStateDict.get(state, 0) > 0


  def GetBlockedStateDict(self):
    """ 
    Returns the blocked state dictionary
    """
    return self.__blockedStateDict


  def SetBlockedStateDict(self, blockedDict):
    """
    Sets the blocked state dictonary, usually used
    in cases where a correction/sync up nees to happen
    """
    self.__blockedStateDict = blockedDict


  def BlockCharacterInput(self):
    """
    Helper method to block the actor from doing anything
    """
    self.BlockMovement()
    self.BlockPosture()
    self.BlockAction()


  def UnblockCharacterInput(self):
    """
    Helper method to undo effects of previous call to
    BlockCharacterInput
    """
    self.UnblockMovement()
    self.UnblockPosture()
    self.UnblockAction()


  def BlockMovement(self):
    """
    Helper method to block all transitions in the movement
    state machine
    """
    self.BlockState(_stopped)
    self.BlockState(_movebackward)
    self.BlockState(_moveforward)
    self.BlockState(_moveleft)
    self.BlockState(_moveright)


  def UnblockMovement(self):
    """
    Helper method to undo effects of previous call to
    BlockCharacterMovement
    """
    self.UnblockState(_stopped)
    self.UnblockState(_movebackward)
    self.UnblockState(_moveforward)
    self.UnblockState(_moveleft)
    self.UnblockState(_moveright)


  def BlockPosture(self):
    """
    Helper method to block transitions in the posture
    state machine
    """
    self.BlockState(_standing)
    self.BlockState(_sitting)
    self.BlockState(_layingdown)
    self.BlockState(_dead)

  
  def UnblockPosture(self):
    """
    Helper method undo effects of previous call to
    BlockCharacterPosture
    """
    self.UnblockState(_standing)
    self.UnblockState(_sitting)
    self.UnblockState(_layingdown)
    self.UnblockState(_dead)


  def BlockAction(self):
    """
    Helper method to block transitions in the action
    state machine.  Idle not blocked because actor
    must be idle to try any other action
    """
    self.BlockState(_casting)
    self.BlockState(_fighting)
    self.BlockState(_gesturing)
    self.BlockState(_crafting)


  def UnblockAction(self):
    """
    Helper method to undo effects of previous call to
    BlockCharacterAction
    """
    self.UnblockState(_casting)
    self.UnblockState(_fighting)
    self.UnblockState(_gesturing)
    self.UnblockState(_crafting)
 
