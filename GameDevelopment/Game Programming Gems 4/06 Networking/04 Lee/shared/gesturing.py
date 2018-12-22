"""
gesturing.py
"""

import shared
import shared.characterstatedata as _characterstatedata

shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.GESTURING

def GetStateTypeId():
  return _characterstatedata.ACTION

def OnEnterState(actor):
  # only a transition to idle allowed now
  actor.BlockAction()

def OnExitState(actor):
  # leaving to idle, action allowed now
  actor.UnblockAction()