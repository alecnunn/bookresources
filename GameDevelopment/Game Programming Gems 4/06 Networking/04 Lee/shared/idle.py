"""
idle.py
"""

import shared
import shared.characterstatedata as _characterstatedata

shared.RegisterState(__name__)

def GetStateId():  
  return _characterstatedata.IDLE

def GetStateTypeId():
  return _characterstatedata.ACTION

def OnEnterState(actor):
  pass

def OnExitState(actor):
  pass