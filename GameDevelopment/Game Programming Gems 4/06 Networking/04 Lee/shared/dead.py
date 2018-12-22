"""
dead.py
"""

import shared
import shared.characterstatedata as _characterstatedata
import shared.stopped as _stopped
import shared.idle as _idle

shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.DEAD

def GetStateTypeId():
  return _characterstatedata.POSTURE

def OnEnterState(actor):
  # stop any movement and actions
  actor.TransitionTo(_stopped)
  actor.TransitionTo(_idle)
  
  # no activity allowed when dead
  actor.BlockCharacterInput()

def OnExitState(actor):
  # mrestore activity when no longer dead
  actor.UnblockCharacterInput()