"""
standing.py
"""

import shared
import shared.characterstatedata as _characterstatedata

shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.STANDING

def GetStateTypeId():
  return _characterstatedata.POSTURE

def OnEnterState(actor):
  pass

def OnExitState(actor):
  pass