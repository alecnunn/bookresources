"""
sitting.py
"""

import shared
import shared.characterstatedata as _characterstatedata
import shared.fighting as _fighting
import shared.casting as _casting
import shared.stopped as _stopped
import shared.idle as _idle


shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.SITTING

def GetStateTypeId():
  return _characterstatedata.POSTURE

def OnEnterState(actor):
  # stop any movement and actions
  actor.TransitionTo(_stopped)
  actor.TransitionTo(_idle)
  
  # cannot move while sitting
  actor.BlockMovement()
  # cannot fight or cast either
  actor.BlockState(_fighting)
  actor.BlockState(_casting)

def OnExitState(actor):
  # restore movement on exit from sitting
  actor.UnblockMovement()
  # restore fighting and casting
  actor.UnblockState(_fighting)
  actor.UnblockState(_casting)