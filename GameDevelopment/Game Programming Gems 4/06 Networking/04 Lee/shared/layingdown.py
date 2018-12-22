"""
layingdown.py
"""

import shared
import shared.characterstatedata as _characterstatedata
import shared.stopped as _stopped
import shared.idle as _idle

shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.LAYINGDOWN

def GetStateTypeId():
  return _characterstatedata.POSTURE

def OnEnterState(actor):
  # stop any movement and actions
  actor.TransitionTo(_stopped)
  actor.TransitionTo(_idle)
  
  # no movement or action when layingdown
  actor.BlockMovement()
  actor.BlockAction()

def OnExitState(actor):
  # restore movement, action when exiting layingdown
  actor.UnblockMovement()
  actor.UnblockAction()
  