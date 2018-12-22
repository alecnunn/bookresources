import string
import shared.characterstatedata as _characterstatedata

def RegisterState(stateName): 
  # Import the module.
  module = __import__(stateName, {}, {}, ["nothing"])

  # Determine the state id from the module name.
  stateIdName = string.upper(stateName)
  stateId = getattr(_characterstatedata, string.upper(string.split(stateName, ".")[-1]))
  _characterstatedata.stateLookup[stateId] = module
  #print 'modules registred = %s' % _characterstatedata.stateLookup

def _GetStateModuleById(stateId):
  return _characterstatedata.stateLookup.get(stateId, None)